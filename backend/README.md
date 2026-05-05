# Stock API Server - NestJS + Prisma

股票分析系统的 NestJS API 服务，提供 RESTful API 接口和数据管理功能。

## 技术栈

- **NestJS 11** - 后端框架
- **Prisma 5** - ORM 数据库访问
- **TypeScript 5** - 类型安全
- **MySQL 8** - 数据库
- **@nestjs/jwt 11** - JWT 认证
- **bcryptjs** - 密码加密

## 项目结构

```
backend/
├── src/
│   ├── modules/
│   │   ├── analysis/      # 分析模块
│   │   ├── charts/        # 图表模块
│   │   ├── stocks/        # 股票模块
│   │   └── auth/          # 认证模块（管理员登录）
│   ├── prisma/            # Prisma 服务
│   ├── main.ts            # 服务入口
│   └── app.module.ts      # 模块配置
├── prisma/
│   ├── schema.prisma      # 数据库模型定义
│   └── migrations/        # 数据库迁移文件
├── dist/                  # 编译输出
├── package.json
├── tsconfig.json
├── nest-cli.json
└── Dockerfile
```

## 数据模型

### Stock (股票信息)
- `tsCode` - 股票代码 (唯一)
- `symbol` - 股票符号
- `name` - 股票名称
- `area` - 地区
- `industry` - 行业
- `market` - 市场
- `exchange` - 交易所
- `listStatus` - 上市状态

### AnalysisResult (分析结果)
- `tsCode` - 股票代码
- `strategyName` - 策略名称
- `tradeDate` - 交易日期
- `label` - 分析标签
- `freq` - 频率

### AnalysisProgress (分析进度)
- `phase1Status` - 阶段1状态
- `phase1Total` - 阶段1总数
- `phase1Completed` - 阶段1完成数
- `phase1Qualified` - 阶段1合格数
- `phase2Status` - 阶段2状态
- `phase2Total` - 阶段2总数
- `phase2Completed` - 阶段2完成数

### AnalysisProcessRecord (分析过程记录)
- `tsCode` - 股票代码
- `stockName` - 股票名称
- `tradeDate` - 交易日期
- `data` - 分析数据 (JSON)

### ChartData (图表数据)
- `tsCode` - 股票代码
- `freq` - 频率
- `analysisDate` - 分析日期
- `data` - 图表数据 (JSON)

### User / WechatBinding / LoginSession (用户系统)
- 支持微信小程序登录
- 用户绑定和会话管理

### 玡Auth (认证系统)
- 管理员账号密码登录
- JWT Token 认证
- 密码 bcrypt 加密存储

## 快速开始

### 本地开发

```bash
# 1. 安装依赖
npm install

# 2. 配置数据库连接
# 设置 DATABASE_URL 环境变量
# 例如: mysql://stock_user:stock_password@localhost:3306/stock_db

# 3. 运行数据库迁移
npx prisma migrate dev

# 4. 启动开发服务器
npm run dev
```

### 生产部署

```bash
# 1. 编译
npm run build

# 2. 运行生产服务器
npm run start
```

### Docker 部署

```bash
# 使用 Docker Compose（推荐）
cd ..
docker-compose up -d api-server
```

## API 接口

服务运行在端口 3001，主要接口：

### 股票模块
- `GET /stocks` - 获取股票列表
- `GET /stocks/:tsCode` - 获取股票详情

### 分析模块
- `GET /analysis/results` - 获取分析结果
- `GET /analysis/progress` - 获取分析进度
- `GET /analysis/process/:tsCode` - 获取分析过程

### 图表模块
- `GET /charts/:tsCode` - 获取图表数据

### 认证模块
- `POST /auth/admin/login` - 管理员账号登录
- `POST /auth/admin/generate-hash` - 生成密码 hash
- `POST /auth/admin/verify` - 验证 Token

## 环境变量

| 变量 | 说明 | 默认值 |
|------|------|--------|
| DATABASE_URL | MySQL 连接字符串 | - |
| PORT | 服务端口 | 3001 |
| STRATEGIES | 启用的策略 | EMA25_GREATER_17_PRICE_MATCH |
| SCHEDULER_EXECUTE_TIME | 调度执行时间 | 20:00 |
| TZ | 时区 | Asia/Shanghai |
| JWT_SECRET | JWT 密钥 | (需配置) |
| JWT_EXPIRES_IN | Token 有效期 | 7d |
| ADMIN_USERNAME | 管理员用户名 | admin |
| ADMIN_PASSWORD_HASH | 管理员密码 hash | (需配置) |

## Prisma 命令

```bash
# 生成 Prisma Client
npx prisma generate

# 创建迁移
npx prisma migrate dev --name <migration_name>

# 查看数据库
npx prisma studio

# 重置数据库
npx prisma migrate reset
```

## 开发指南

### 添加新模块

1. 创建模块目录 `src/modules/<module-name>/`
2. 创建模块文件:
   - `<module>.module.ts` - 模块定义
   - `<module>.controller.ts` - 控制器
   - `<module>.service.ts` - 服务
   - `<module>.dto.ts` - 数据传输对象
3. 在 `app.module.ts` 中注册模块
4. 如需新数据模型，更新 `prisma/schema.prisma`

### 添加新数据模型

1. 在 `prisma/schema.prisma` 中添加模型定义
2. 运行迁移: `npx prisma migrate dev --name <name>`
3. 在服务中使用 Prisma Client 访问

## 许可证

MIT License