## Why

Node.js 飞书机器人当前使用 console.log 输出日志到 stdout，无法持久化到本地文件，也不支持日志级别控制和格式化。与 C++ 分析引擎的日志系统相比，缺少：
- 日志文件持久化
- 日志轮转
- 时间戳和日志级别格式化
- 动态日志级别调整

## What Changes

- 引入 winston 日志库，创建统一日志模块
- 日志输出到 `/app/logs/nodejs/bot.log`，与 C++ 日志分开存储
- 支持 10MB 文件轮转，保留 3 个备份
- 格式：`YYYY-MM-DD HH:mm:ss [LEVEL] message`
- 通过环境变量 `LOG_LEVEL` 控制日志级别（DEBUG/INFO/WARN/ERROR）
- 开发环境同时输出到控制台

## Capabilities

### New Capabilities
- `nodejs-logging`: Node.js 结构化日志系统，支持文件输出、轮转、级别控制

### Modified Capabilities
- 无

## Impact

- 新增依赖：winston
- 修改：nodejs/src/index.js, nodejs/src/handler.js, nodejs/src/reply.js
- 新增：nodejs/src/logger.js
- 更新：Dockerfile（创建 /app/logs/nodejs 目录）
- 更新：docker-compose.yml（挂载日志目录）