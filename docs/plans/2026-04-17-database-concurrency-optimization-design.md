# 数据库并发架构优化设计

**日期**: 2026-04-17  
**状态**: 待审批 → 实施计划  
**优先级**: 高  

---

## 1. 问题分析

### 1.1 现状问题

当前系统存在数据库访问阻塞问题：

```
前端请求 → Node.js API → SQLite (g_db)
                      ↓
                   互斥锁竞争
                      ↓
分析任务 → C++ ThreadPool → SQLite (同一连接)
```

**具体问题：**
- Node.js 和 C++ 共用同一个 SQLite 全局连接（`g_db`）
- 所有数据库操作通过 `db_mutex_` 串行化
- 分析写入阻塞前端读取
- 前端请求超时（30s timeout exceeded）

### 1.2 错误日志证据

```
[2026-04-17 23:10:13.925] [error] [55] POST / 失败: Could not establish connection
index-BBS0W67j.js:53 加载记录失败: AxiosError: timeout of 30000ms exceeded
```

---

## 2. 设计目标

### 2.1 主要目标

- **前端响应优先**: 前端请求不被分析任务阻塞
- **中等改动规模**: 不引入新的外部依赖（如 Redis）
- **Node.js 改为调用 C++ API**: 统一数据访问路径

### 2.2 成功指标

| 指标 | 优化前 | 优化后目标 |
|------|--------|-----------|
| 前端 API 响应时间 | 10s+ (阻塞时) | < 500ms |
| 并发请求处理能力 | 串行（1个） | 并发（5+） |
| 分析任务吞吐量 | 受 DB 阻塞影响 | 不阻塞，持续执行 |

---

## 3. 架构设计

### 3.1 新架构

```
前端 → Node.js (转发层) → C++ HTTP API (3001)
                              │
                              ▼
                    ConnectionPool (读写分离)
                    ┌───┐ ┌───┐ ┌───┐ ┌───┐ ┌───┐
                    │R1│ │R2│ │R3│ │R4│ │R5│   ← 读连接池（并发）
                    └───┘ └───┘ └───┘ └───┘ └───┘
                    ┌───┐
                    │ W │                     ← 写连接（单例）
                    └───┘
                              │
                              ▼
                    SQLite (WAL 模式)
                    /app/stock.db

分析任务 → ThreadPool → WriteQueue → 异步写入
```

### 3.2 核心改变

1. **Node.js 变成转发层**: 不再直接访问数据库
2. **C++ API 统一入口**: 所有数据库访问通过 C++ HTTP API
3. **连接池分离读写**: 读并发，写串行化
4. **异步写入队列**: 分析结果写入不阻塞读请求

---

## 4. 组件设计

### 4.1 新增组件

| 组件 | 文件 | 职责 |
|------|------|------|
| ConnectionPool | `cpp/data/database/ConnectionPool.h/cpp` | 读连接池 + 写连接管理 |
| WriteQueue | `cpp/data/database/WriteQueue.h/cpp` | 异步写入队列 |
| NodeProxy | `nodejs/src/proxy.js` | Node.js 转发层 |

### 4.2 ConnectionPool

```cpp
class ConnectionPool {
public:
    // 获取读连接（从池中）
    std::shared_ptr<SQLiteConnection> acquireRead();
    
    // 释放读连接
    void releaseRead(std::shared_ptr<SQLiteConnection> conn);
    
    // 获取写连接（单例）
    std::shared_ptr<SQLiteConnection> acquireWrite();
    
    // 释放写连接
    void releaseWrite();
    
private:
    std::vector<std::shared_ptr<SQLiteConnection>> readPool_;
    std::mutex readMutex_;
    std::condition_variable readAvailable_;
    
    std::shared_ptr<SQLiteConnection> writeConn_;
    std::mutex writeMutex_;
    
    int poolSize_ = 5;  // 默认 5 个读连接
};
```

**关键特性：**
- 读连接池：5 个连接并发读取
- 写连接：单例，写入串行化保证一致性
- WAL 模式：读不阻塞写

### 4.3 WriteQueue

```cpp
class WriteQueue {
public:
    void enqueue(WriteTask task);  // 异步入队
    void start();                   // 启动后台线程
    void stop();                    // 停止并等待完成
    
private:
    std::queue<WriteTask> queue_;
    std::mutex queueMutex_;
    std::condition_variable queueCV_;
    std::thread writerThread_;
    void processQueue();            // 后台处理
};

struct WriteTask {
    std::string table;
    std::string operation;  // INSERT/UPDATE/DELETE
    json data;
    std::function<void(bool)> callback;  // 完成回调
};
```

**关键特性：**
- 分析任务立即入队返回，不阻塞
- 后台线程串行化写入
- 失败重试 3 次

### 4.4 Node.js Proxy

```javascript
// nodejs/src/proxy.js
const CPP_API_URL = process.env.CPP_API_URL || 'http://localhost:3001';

async function proxyRequest(req, res, endpoint) {
    try {
        const response = await axios({
            method: req.method,
            url: `${CPP_API_URL}${endpoint}`,
            params: req.query,
            data: req.body,
            timeout: 30000
        });
        res.json(response.data);
    } catch (error) {
        // 错误处理：503/504/500
    }
}

// 所有 /api/* 路由改为转发
router.get('/api/stocks', (req, res) => proxyRequest(req, res, '/api/stocks'));
router.get('/api/analysis/process', (req, res) => proxyRequest(req, res, '/api/analysis/process'));
// ... 其他路由
```

### 4.5 改动现有组件

| 文件 | 改动 |
|------|------|
| `Connection.h/cpp` | 移除 g_db，改为 ConnectionPool |
| `StockDAO.cpp` | 使用 pool.acquireRead/acquireWrite |
| `ApiRouter.cpp` | 使用连接池 |
| `main.cpp` | 初始化 ConnectionPool，启动 WriteQueue |
| `nodejs/src/api.js` | 移除 DB 访问，改为调用 proxy |

---

## 5. 数据流设计

### 5.1 读请求流程

```
前端 → Node.js → C++ API → ConnectionPool.acquireRead()
                            ↓
                    [ReadConn1-5] SELECT (并发)
                            ↓
                    pool.releaseRead()
                            ↓
                        返回数据
```

**并发场景：**
- 5 个读连接可同时执行 SELECT
- WAL 模式下读不阻塞写
- 前端请求并发执行，无需等待

### 5.2 写请求流程

```
ThreadPool工作线程 → WriteQueue.enqueue() → 立即返回
                                            ↓
后台写入线程 → processQueue() → pool.acquireWrite()
                                ↓
                        [WriteConn] INSERT
                                ↓
                        pool.releaseWrite()
                                ↓
                        callback(true)
```

**异步特性：**
- 分析线程不阻塞，立即入队后继续
- 写入串行化保证一致性
- 队列缓冲突发写入

---

## 6. 错误处理设计

### 6.1 ConnectionPool 错误

| 场景 | 处理 |
|------|------|
| 连接池耗尽 | 等待 5s 超时，返回错误 |
| 连接创建失败 | 返回 500，记录日志 |
| SQLite 错误 | 返回具体错误信息 |

### 6.2 WriteQueue 错误

| 场景 | 处理 |
|------|------|
| 写入失败 | 重试 3 次，仍失败则丢弃 |
| 队列满 | 等待 10s，超时丢弃 |
| 线程崩溃 | 自动重启 |

### 6.3 NodeProxy 错误

| 场景 | 处理 |
|------|------|
| C++ API 不可达 | 返回 503 |
| C++ API 超时 | 返回 504 |
| C++ API 错误 | 转发错误响应 |

---

## 7. 测试策略

### 7.1 单元测试

- ConnectionPool: 连接获取/释放/等待超时
- WriteQueue: 入队/处理/失败重试
- NodeProxy: 转发/超时/错误处理

### 7.2 并发测试

```bash
# 100 个并发请求
ab -n 100 -c 50 http://localhost:3001/api/stocks

# 验证：
# - 所有请求成功
# - 平均响应 < 500ms
# - 无连接池耗尽错误
```

### 7.3 回归测试

- API 功能一致性：返回格式不变
- 数据一致性：写入正确、查询正确
- 性能对比：响应时间降低 50%+

---

## 8. 实施计划

详见：`writing-plans` skill 生成的实施计划

### 8.1 实施阶段

1. **阶段 1**: ConnectionPool 实现
2. **阶段 2**: WriteQueue 实现
3. **阶段 3**: Node.js Proxy 改造
4. **阶段 4**: DAO/API 改用连接池
5. **阶段 5**: 测试验证

### 8.2 风险评估

| 风险 | 影响 | 缓解措施 |
|------|------|---------|
| Node.js 完全依赖 C++ | C++ 挂掉则前端不可用 | C++ API 健康检查 + 重启机制 |
| 连接池配置不当 | 性能不达标 | 动态调整池大小 + 监控 |
| 写入队列堆积 | 数据延迟 | 监控队列长度 + 预警 |

---

## 9. 附录

### 9.1 相关文件

**新增文件：**
- `cpp/data/database/ConnectionPool.h`
- `cpp/data/database/ConnectionPool.cpp`
- `cpp/data/database/WriteQueue.h`
- `cpp/data/database/WriteQueue.cpp`
- `nodejs/src/proxy.js`

**修改文件：**
- `cpp/data/database/Connection.h/cpp`
- `cpp/data/database/StockDAO.cpp`
- `cpp/api/ApiRouter.cpp`
- `cpp/main.cpp`
- `nodejs/src/api.js`
- `nodejs/src/index.js`

### 9.2 参考资料

- SQLite WAL 模式文档：https://www.sqlite.org/wal.html
- cpp-httplib 并发模型：https://github.com/yhirose/cpp-httplib
- 现有代码分析：见 `bg_52727ea6` 和 `bg_1a5c29d8` 探索结果

---

**设计审批**: ________________  
**实施计划**: 待 `writing-plans` skill 生成