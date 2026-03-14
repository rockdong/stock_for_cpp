## ADDED Requirements

### Requirement: 日志文件输出
系统 SHALL 将日志输出到文件 `/app/logs/nodejs/bot.log`，与 C++ 日志分开存储。

#### Scenario: 日志文件创建
- **WHEN** Node.js 应用启动
- **THEN** 系统创建 `/app/logs/nodejs/bot.log` 文件

#### Scenario: 日志写入
- **WHEN** 应用产生日志
- **THEN** 日志内容写入 `bot.log` 文件

### Requirement: 日志轮转
系统 SHALL 支持日志文件轮转，单个文件最大 10MB，保留 3 个备份文件。

#### Scenario: 文件轮转
- **WHEN** 日志文件达到 10MB
- **THEN** 当前文件重命名为 `bot.1.log`，创建新的 `bot.log`

#### Scenario: 备份文件数量限制
- **WHEN** 已有 3 个备份文件（bot.1.log, bot.2.log, bot.3.log）
- **AND** 触发新一轮转
- **THEN** 最旧的 `bot.3.log` 被删除，其他文件依次重命名

### Requirement: 日志格式
系统 SHALL 输出格式化的日志：`YYYY-MM-DD HH:mm:ss [LEVEL] message`。

#### Scenario: 格式验证
- **WHEN** 应用产生日志消息 "启动服务"
- **AND** 日志级别为 INFO
- **THEN** 输出格式为 `2026-03-15 01:32:45 [INFO] 启动服务`

### Requirement: 日志级别控制
系统 SHALL 支持通过环境变量 `LOG_LEVEL` 控制日志级别，支持 DEBUG、INFO、WARN、ERROR 四个级别。

#### Scenario: 默认日志级别
- **WHEN** 未设置 `LOG_LEVEL` 环境变量
- **THEN** 默认使用 INFO 级别

#### Scenario: 日志级别过滤
- **WHEN** 设置 `LOG_LEVEL=ERROR`
- **THEN** 只有 ERROR 级别的日志被输出
- **AND** DEBUG、INFO、WARN 级别的日志被过滤

#### Scenario: 动态日志级别
- **WHEN** 设置 `LOG_LEVEL=DEBUG`
- **THEN** 所有级别的日志都被输出

### Requirement: 开发环境控制台输出
系统 SHALL 在非生产环境（`NODE_ENV !== 'production'`）同时输出日志到控制台。

#### Scenario: 开发环境控制台输出
- **WHEN** `NODE_ENV=development`
- **THEN** 日志同时输出到文件和控制台

#### Scenario: 生产环境无控制台输出
- **WHEN** `NODE_ENV=production`
- **THEN** 日志仅输出到文件