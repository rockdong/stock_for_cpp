## ADDED Requirements

### Requirement: 请求日志记录
系统 SHALL 对所有HTTP API请求记录完整日志。

#### Scenario: 请求开始日志
- **WHEN** 收到HTTP请求
- **THEN** 记录日志: [requestId] START method url
- **AND** 记录DEBUG级别: query参数、body内容

#### Scenario: 请求结束日志
- **WHEN** 响应发送完成
- **THEN** 记录日志: [requestId] END statusCode durationMs

#### Scenario: 慢请求警告
- **WHEN** 请求耗时超过5秒
- **THEN** 记录WARN级别日志: [requestId] SLOW REQUEST: durationMs

#### Scenario: 超时警告
- **WHEN** 请求耗时超过10秒
- **THEN** 记录WARN级别日志: [requestId] TIMEOUT WARNING: durationMs

### Requirement: RequestId生成
系统 SHALL 为每个请求生成唯一RequestId。

#### Scenario: RequestId格式
- **WHEN** 生成RequestId
- **THEN** 格式为 timestamp-randomString，如 1704123456-a7b8c9

#### Scenario: RequestId唯一性
- **WHEN** 同一秒内多个请求
- **THEN** 通过randomString保证唯一性

### Requirement: 日志格式
系统 SHALL 使用统一的日志格式。

#### Scenario: 日志输出格式
- **WHEN** 记录日志
- **THEN** 格式: timestamp [LEVEL] [requestId] message
- **示例**: 2026-04-16 20:00:00 [INFO] [1704123456-a7b8c9] START GET /api/fundamentals/filter

### Requirement: 错误请求日志
系统 SHALL 对错误请求记录详细日志。

#### Scenario: API错误日志
- **WHEN** API返回错误
- **THEN** 记录ERROR级别: [requestId] ERROR: errorMessage

#### Scenario: 异常日志
- **WHEN** 发生未捕获异常
- **THEN** 记录ERROR级别并包含异常堆栈