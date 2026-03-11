## Why

当前系统使用 10 线程并发分析股票，每只股票调用 3 次 API（日线/周线/月线），瞬时并发可达 30 次请求，远超 Tushare API 的 500次/分钟限制。这导致频繁触发 API 限流，出现连接超时错误，最终引发程序崩溃并由 Supervisor 自动重启。

日志证据：
```
[2026-03-12 01:22:29.173] [error] [513] POST / 失败: Connection timed out
```

## What Changes

- 新增 `RateLimiter` 类：实现令牌桶算法，全局控制 API 调用频率
- 修改 `TushareClient::query()`：在发送请求前先获取令牌
- 新增配置项：`TUSHARE_RATE_LIMIT`（每秒请求数）、`TUSHARE_BURST_SIZE`（突发容量）
- 新增 API 限流响应处理：检测到 `code=-2` 时暂停 60 秒后重试
- 改进异常处理：支持程序退出时中断等待中的线程

## Capabilities

### New Capabilities

- `api-rate-limiting`: 全局 API 调用限流能力，确保所有 Tushare API 调用不超过设定频率

### Modified Capabilities

- 无现有 capability 的需求变更

## Impact

**新增文件：**
- `cpp/utils/RateLimiter.h` - 限流器头文件
- `cpp/utils/RateLimiter.cpp` - 限流器实现

**修改文件：**
- `cpp/network/http/TushareClient.cpp` - 在 `query()` 中调用限流器
- `cpp/network/http/TushareClient.h` - 添加限流器引用
- `cpp/config/Config.h` - 新增限流配置读取方法
- `cpp/config/Config.cpp` - 实现限流配置读取
- `cpp/.env` - 新增限流配置项

**依赖关系：**
- 限流器依赖 `<mutex>`, `<condition_variable>`, `<chrono>` (C++17 标准库)
- 无新增外部依赖

**风险评估：**
- 低风险：限流器为独立模块，不影响现有业务逻辑
- 可回滚：可通过配置关闭限流（设置极高频率）