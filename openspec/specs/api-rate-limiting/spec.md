# API Rate Limiting

## Purpose

控制系统对 Tushare API 的调用频率，防止因超出 API 限制而导致请求失败。

## Requirements

### Requirement: 限流器控制 API 调用频率
系统 SHALL 通过令牌桶算法限制 Tushare API 调用频率，确保每秒请求数不超过配置值。

#### Scenario: 正常限流
- **WHEN** 多个线程同时调用 Tushare API
- **THEN** 系统以配置的速率（默认 8 次/秒）逐个处理请求
- **AND** 等待中的线程按 FIFO 顺序获取令牌

#### Scenario: 突发流量处理
- **WHEN** 限流器令牌桶中有可用令牌
- **THEN** 请求立即通过，无需等待
- **AND** 令牌数量减少

#### Scenario: 令牌补充
- **WHEN** 时间流逝
- **THEN** 令牌以固定速率补充
- **AND** 令牌数量不超过最大容量（burst_size）

### Requirement: 配置化限流参数
系统 SHALL 支持通过环境变量配置限流参数。

#### Scenario: 读取限流配置
- **WHEN** 系统初始化
- **THEN** 从 `.env` 读取 `TUSHARE_RATE_LIMIT`（默认 8）
- **AND** 从 `.env` 读取 `TUSHARE_BURST_SIZE`（默认 10）

### Requirement: API 限流响应处理
系统 SHALL 检测 Tushare API 返回的限流错误并自动重试。

#### Scenario: 检测限流响应
- **WHEN** API 返回 `code=-2` 且消息包含 "500次" 或 "限流"
- **THEN** 系统暂停 60 秒
- **AND** 自动重试请求

#### Scenario: 正常错误响应
- **WHEN** API 返回其他错误码
- **THEN** 系统按原有错误处理流程处理
- **AND** 不触发额外等待

### Requirement: 程序退出时中断等待
系统 SHALL 支持在程序退出时中断所有等待令牌的线程。

#### Scenario: 优雅退出
- **WHEN** 程序收到 SIGTERM 或 SIGINT 信号
- **THEN** 限流器设置停止标志
- **AND** 唤醒所有等待线程
- **AND** 等待线程抛出异常退出

#### Scenario: 正常运行
- **WHEN** 程序正常运行
- **THEN** 限流器正常工作
- **AND** 不触发停止机制

### Requirement: 线程安全
系统 SHALL 确保限流器在多线程环境下的线程安全。

#### Scenario: 并发访问
- **WHEN** 多个线程同时调用 `acquire()`
- **THEN** 令牌数量正确递减
- **AND** 无竞态条件
- **AND** 无死锁

#### Scenario: 并发配置
- **WHEN** 多个线程同时运行时调用 `configure()`
- **THEN** 配置变更安全生效
- **AND** 不影响正在等待的线程