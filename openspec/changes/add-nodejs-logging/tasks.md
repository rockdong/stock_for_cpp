## 1. 依赖安装

- [ ] 1.1 安装 winston 依赖：`npm install winston`

## 2. 日志模块创建

- [ ] 2.1 创建 `nodejs/src/logger.js`，实现 winston 日志配置
- [ ] 2.2 配置文件 transport：输出到 `/app/logs/nodejs/bot.log`
- [ ] 2.3 配置日志轮转：maxsize=10MB, maxFiles=3
- [ ] 2.4 配置日志格式：`YYYY-MM-DD HH:mm:ss [LEVEL] message`
- [ ] 2.5 支持环境变量 `LOG_LEVEL` 控制日志级别
- [ ] 2.6 开发环境添加 Console transport

## 3. 代码集成

- [ ] 3.1 修改 `nodejs/src/index.js`，替换 console.log 为 logger
- [ ] 3.2 修改 `nodejs/src/handler.js`，替换 console.log 为 logger
- [ ] 3.3 修改 `nodejs/src/reply.js`，替换 console.log 为 logger
- [ ] 3.4 修改 `nodejs/src/database.js`，替换 console.log 为 logger
- [ ] 3.5 修改 `nodejs/src/chartGenerator.js`，替换 console.log 为 logger

## 4. Docker 配置更新

- [ ] 4.1 更新 Dockerfile，创建 `/app/logs/nodejs` 目录
- [ ] 4.2 确认 docker-compose.yml 日志目录挂载（已挂载 `./logs:/app/logs`）

## 5. 验证测试

- [ ] 5.1 本地测试日志文件输出
- [ ] 5.2 测试日志轮转功能
- [ ] 5.3 测试日志级别过滤
- [ ] 5.4 Docker 环境测试