## 1. RateLimiter 核心实现

- [x] 1.1 创建 `cpp/utils/RateLimiter.h` 头文件，定义 RateLimiter 类接口
- [x] 1.2 创建 `cpp/utils/RateLimiter.cpp` 实现文件，实现令牌桶算法
- [x] 1.3 实现单例模式 `getInstance()`
- [x] 1.4 实现 `configure(rate_per_second, burst_size)` 配置方法
- [x] 1.5 实现 `acquire()` 阻塞获取令牌方法
- [x] 1.6 实现 `stop()` 停止方法，支持程序退出时中断等待线程
- [x] 1.7 实现令牌补充逻辑 `refill()`

## 2. 配置支持

- [x] 2.1 在 `cpp/config/Config.h` 添加 `getTushareRateLimit()` 方法声明
- [x] 2.2 在 `cpp/config/Config.h` 添加 `getTushareBurstSize()` 方法声明
- [x] 2.3 在 `cpp/config/Config.cpp` 实现配置读取方法
- [x] 2.4 在 `cpp/.env` 添加 `TUSHARE_RATE_LIMIT=8` 配置项
- [x] 2.5 在 `cpp/.env` 添加 `TUSHARE_BURST_SIZE=10` 配置项

## 3. TushareClient 集成

- [x] 3.1 在 `cpp/network/http/TushareClient.cpp` 的 `query()` 方法中调用 `RateLimiter::acquire()`
- [x] 3.2 在 `query()` 方法中添加 API 限流响应检测逻辑
- [x] 3.3 实现限流响应后的 60 秒等待和重试逻辑
- [x] 3.4 添加相关日志记录

## 4. 程序退出处理

- [x] 4.1 在 `cpp/main.cpp` 的信号处理函数中调用 `RateLimiter::stop()`
- [x] 4.2 确保等待中的线程能正确抛出异常并退出

## 5. CMake 构建配置

- [x] 5.1 在 `cpp/CMakeLists.txt` 中添加 RateLimiter.cpp 到源文件列表

## 6. 测试验证

- [x] 6.1 编译项目，确保无编译错误
- [ ] 6.2 运行程序，观察日志确认限流器生效
- [ ] 6.3 验证 API 调用频率不超过 8 次/秒
- [ ] 6.4 验证程序退出时能正常中断等待线程
- [ ] 6.5 长时间运行测试，确认无超时错误