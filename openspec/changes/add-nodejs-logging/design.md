## Context

当前 Node.js 飞书机器人使用 `console.log` 输出日志，直接写入 Docker stdout。C++ 分析引擎已有完善的日志系统（文件输出、轮转、级别控制），但 Node.js 缺少类似的日志能力。

**目录结构**：
```
/app/logs/
├── app.log          ← C++ 日志
├── app.1.log
└── nodejs/
    ├── bot.log      ← Node.js 日志 (新增)
    ├── bot.1.log
    └── bot.2.log
```

## Goals / Non-Goals

**Goals:**
- 日志持久化到本地文件，与 C++ 日志分开存储
- 支持日志级别（DEBUG/INFO/WARN/ERROR）通过环境变量动态控制
- 支持 10MB 文件轮转，保留 3 个备份
- 格式化输出：`YYYY-MM-DD HH:mm:ss [LEVEL] message`
- 开发环境同时输出到控制台

**Non-Goals:**
- 不实现结构化 JSON 日志（暂不需要）
- 不实现远程日志上报
- 不实现日志聚合（ELK 等）

## Decisions

### 1. 日志库选择：winston

**选择 winston 的原因**：
- 生态成熟，文档完善
- 内置 transport 机制，易于扩展
- 原生支持文件轮转

**备选方案**：
| 方案 | 优点 | 缺点 |
|------|------|------|
| pino | 性能极高 | 配置复杂，结构化日志过于严格 |
| bunyan | 结构化日志 | 依赖重，需要 bunyan CLI 查看日志 |
| supervisor 原生 | 零依赖 | 格式不可控，无级别控制 |

### 2. 文件轮转策略：winston 内置 size-based

使用 winston 自带的 `maxsize` 和 `maxFiles` 参数，无需额外依赖。

**配置**：
- `maxsize`: 10MB (10485760 bytes)
- `maxFiles`: 3

### 3. 日志格式

```
2026-03-15 01:32:45 [INFO] 启动飞书机器人（WebSocket 长连接模式）...
2026-03-15 01:32:46 [DEBUG] 收到消息: {...}
```

使用 winston 的 `format.combine()` 实现：
- `format.timestamp({ format: 'YYYY-MM-DD HH:mm:ss' })`
- `format.printf()` 自定义输出格式

### 4. 日志级别配置

通过环境变量 `LOG_LEVEL` 控制，默认 `INFO`。

**级别优先级**：ERROR > WARN > INFO > DEBUG

**开发环境**：检测 `NODE_ENV !== 'production'` 时，同时添加 Console transport。

## Risks / Trade-offs

| 风险 | 缓解措施 |
|------|----------|
| winston 性能不如 pino | 飞书机器人日志量小，性能影响可忽略 |
| 日志文件权限问题 | Dockerfile 中确保 `/app/logs/nodejs` 目录权限正确 |
| 磁盘空间占用 | 10MB × 3 文件 = 最大 30MB，可接受 |

## Migration Plan

1. 安装依赖：`npm install winston`
2. 创建 `nodejs/src/logger.js`
3. 修改 `index.js`, `handler.js`, `reply.js` 替换 `console.log`
4. 更新 Dockerfile 创建日志目录
5. 更新 docker-compose.yml 挂载日志目录（已挂载 `./logs:/app/logs`）