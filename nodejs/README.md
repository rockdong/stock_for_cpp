# Node.js 飞书推送服务

飞书机器人推送服务 + REST API，为微信小程序提供数据接口。

## 技术栈

- **Node.js 20** - 运行时
- **Express 4** - Web 框架
- **@larksuiteoapi/node-sdk** - 飞书 SDK
- **better-sqlite3** - SQLite 数据库
- **winston** - 日志管理
- **jsonwebtoken** - JWT 认证
- **canvas** - 图表生成
- **mysql2** - MySQL 连接

## 项目结构

```
nodejs/
├── src/
│   ├── index.js           # 服务入口
│   ├── api.js             # REST API 路由
│   ├── handler.js         # 飞书消息处理
│   ├── feishu.js          # 飞书 API 封装
│   ├── config.js          # 配置管理
│   ├── database.js        # 数据库操作
│   ├── logger.js          # 日志配置
│   ├── chartGenerator.js  # 图表生成
│   ├── client.js          # 飞书客户端
│   ├── messageBuilder.js  # 消息构建
│   ├── proxy.js           # API 代理
│   ├── commands/          # 命令处理器
│   ├── formatters/        # 格式化工具
│   ├── middleware/        # Express 中间件
│   ├── routes/            # 认证路由
│   │   ├── auth.js        # 微信登录
│   │   └── feishuAuth.js  # 飞书登录
│   ├── services/          # 业务服务
│   ├── webhook/           # 微信事件处理
│   └── docs/              # 文档
│       ├── ENCRYPT.md     # 加密说明
│       └── plans/         # 设计文档
├── .env                   # 环境变量
├── Dockerfile             # Docker 构建
└── package.json
```

## 功能模块

### 飞书机器人

- WebSocket 长连接模式
- 消息接收与处理
- 卡片交互回调
- 分析结果推送

### REST API

为微信小程序和 Web 前端提供数据接口：

| 模块 | 接口 |
|------|------|
| 股票 | `/api/stocks`, `/api/stocks/search`, `/api/stocks/:code` |
| 分析 | `/api/analysis/signals`, `/api/analysis/progress`, `/api/analysis/process` |
| 图表 | `/api/charts/:code` |
| 认证 | `/api/auth/qrcode`, `/api/auth/status`, `/api/auth/token` |
| 飞书认证 | `/api/auth/feishu/qrcode`, `/api/auth/feishu/callback` |

### 微信登录

- 二维码生成
- 扫码事件处理
- JWT Token 生成

### 飞书登录

- OAuth 授权流程
- 用户信息获取
- Token 生成

## 快速开始

### 本地开发

```bash
# 1. 安装依赖
npm install

# 2. 配置环境变量
cp .env.example .env
# 编辑 .env，填写飞书和微信配置

# 3. 启动服务
npm start

# 或开发模式（自动重启）
npm run dev
```

### Docker 部署

```bash
# 构建镜像
docker build -t stock-bot .

# 运行
docker run -p 3000:3000 \
  -e FEISHU_APP_ID=xxx \
  -e FEISHU_APP_SECRET=xxx \
  stock-bot
```

## 环境变量

| 变量 | 说明 | 必填 |
|------|------|------|
| FEISHU_APP_ID | 飞书应用 ID | 是 |
| FEISHU_APP_SECRET | 飞书应用密钥 | 是 |
| FEISHU_VERIFICATION_TOKEN | 飞书验证令牌 | 是 |
| DATA_SOURCE_API_KEY | Tushare API Key | 是 |
| WECHAT_APP_ID | 微信 AppID | 否 |
| WECHAT_APP_SECRET | 微信 AppSecret | 否 |
| WECHAT_TOKEN | 微信 Token | 否 |
| JWT_SECRET | JWT 密钥 | 是 |
| DB_TYPE | 数据库类型 (sqlite/mysql) | 是 |
| DB_PATH | SQLite 数据库路径 | sqlite 必填 |
| DATABASE_URL | MySQL 连接字符串 | mysql 必填 |
| PORT | 服务端口 | 否 (默认 3000) |
| STRATEGIES | 策略列表 | 否 |
| SCHEDULER_EXECUTE_TIME | 执行时间 | 否 |

## 飞书机器人配置

1. 创建飞书企业自建应用
2. 配置事件订阅（WebSocket 模式）
3. 添加消息接收权限
4. 获取 App ID 和 App Secret

### 支持的命令

| 命令 | 说明 |
|------|------|
| `分析` | 执行股票分析 |
| `查询 [代码]` | 查询股票信息 |
| `进度` | 查看分析进度 |
| `帮助` | 显示帮助信息 |

## 微信公众号配置

1. 配置服务器地址：`https://your-domain/api/wechat/event`
2. 设置 Token 和 EncodingAESKey
3. 启用消息加密（可选）

## 日志管理

使用 Winston 日志库：

```javascript
const logger = require('./logger');

logger.info('服务启动');
logger.error('错误信息', { error: err });
```

日志级别：`error`, `warn`, `info`, `debug`

## 许可证

MIT License