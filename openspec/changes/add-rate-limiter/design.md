## Context

当前系统使用 `ThreadPool` 进行多线程股票分析，每个线程独立创建 `TushareClient` 实例。在 `main.cpp` 中，任务提交时有 300ms 延迟，但任务执行时无任何限流控制，导致：

```
时间线:
提交:  T1(0ms) ── T2(300ms) ── T3(600ms) ── ...
执行:  10线程同时发起API请求 (30次并发)
API:   500次/分钟 = 8.3次/秒 (超出3.6倍)
```

## Goals / Non-Goals

**Goals:**
- 全局控制 Tushare API 调用频率，不超过 8 次/秒
- 支持突发流量处理（初始可快速执行部分请求）
- 线程安全，多线程共享同一限流状态
- 支持程序优雅退出时中断等待线程
- 检测 API 限流响应并自动暂停重试

**Non-Goals:**
- 不修改 ThreadPool 实现
- 不修改数据库存储逻辑（freq 字段问题另案处理）
- 不实现动态限流速率调整
- 不实现分布式限流（单机部署）

## Decisions

### 决策1: 令牌桶算法 (Token Bucket)

**选择理由:**
- 允许突发流量：桶内有令牌时可快速处理
- 长期稳定：令牌补充速率固定
- 实现简单：仅需 `mutex` + `condition_variable`
- 性能好：O(1) 时间复杂度

**备选方案:**
| 算法 | 优点 | 缺点 |
|------|------|------|
| 漏桶 (Leaky Bucket) | 严格固定速率 | 不支持突发 |
| 滑动窗口 | 精确控制 | 实现复杂，内存占用高 |
| 固定窗口 | 最简单 | 边界突发问题 |

### 决策2: 在 TushareClient::query() 中集成

**选择理由:**
- 改动最小：仅修改一处
- 覆盖全面：所有 API 调用都经过此处
- 风险最低：不影响其他模块

**备选方案:**
| 方案 | 优点 | 缺点 |
|------|------|------|
| 共享 TushareClient 单例 | 资源占用最少 | 改动大，需修改 main.cpp |
| HttpClient 层限流 | 更底层控制 | 不适用于其他 HTTP 客户端 |

### 决策3: RateLimiter 作为单例

**选择理由:**
- 全局共享状态：所有线程使用同一个限流器
- 简化调用：无需传递实例
- 配置集中：一处配置全局生效

### 决策4: 阻塞式 acquire()

**选择理由:**
- 简单可靠：线程自动等待
- 不丢失请求：所有请求最终都会执行

**备选方案:**
| 方案 | 优点 | 缺点 |
|------|------|------|
| tryAcquire + 重试 | 非阻塞 | 需调用方处理重试逻辑 |
| acquire(timeout) | 可超时退出 | 增加复杂度 |

## Risks / Trade-offs

### 风险1: 令牌等待时间过长

**场景:** 大量线程同时等待令牌

**缓解措施:**
- 设置合理的 `burst_size` (10)，允许初始突发
- 使用 `notify_one()` 逐个唤醒，避免惊群

### 风险2: 程序退出时线程阻塞

**场景:** 程序收到 SIGTERM，但有线程在 `acquire()` 中等待

**缓解措施:**
- 提供 `stop()` 方法，设置停止标志并唤醒所有等待线程
- 在 `main.cpp` 的信号处理中调用 `RateLimiter::stop()`

### 风险3: API 返回限流错误

**场景:** 即使本地限流，服务端可能因其他原因限流

**缓解措施:**
- 检测响应 `code=-2` 且包含 "500次" 或 "限流"
- 暂停 60 秒后重试

## 实现细节

### RateLimiter 类设计

```cpp
class RateLimiter {
public:
    static RateLimiter& getInstance();
    
    void configure(int rate_per_second, int burst_size);
    void acquire();  // 阻塞直到获取令牌
    void stop();     // 停止所有限流，唤醒等待线程
    
private:
    RateLimiter() = default;
    std::mutex mutex_;
    std::condition_variable cv_;
    int tokens_;
    int max_tokens_;
    int refill_rate_;
    std::chrono::steady_clock::time_point last_refill_;
    bool stopped_ = false;
    
    void refill();  // 补充令牌
};
```

### TushareClient 修改

```cpp
TushareResponse TushareClient::query(...) {
    // 1. 获取令牌
    RateLimiter::getInstance().acquire();
    
    // 2. 发送请求
    auto response = http_client_->post(...);
    
    // 3. 检测限流响应
    if (response.code == -2 && response.msg.find("500次") != std::string::npos) {
        LOG_WARN("触发 API 限流，等待 60 秒后重试");
        std::this_thread::sleep_for(std::chrono::seconds(60));
        return query(...);  // 重试
    }
    
    return response;
}
```

### 配置项

```env
# .env
TUSHARE_RATE_LIMIT=8    # 每秒请求数 (安全值，低于 8.33)
TUSHARE_BURST_SIZE=10   # 突发容量
```