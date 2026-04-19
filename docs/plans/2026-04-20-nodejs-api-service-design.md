# Node.js API 服务替换 C++ HTTP API 设计文档

**日期**: 2026-04-20
**状态**: 设计完成，待实施

## 背景

现有 C++ HTTP API 服务（cpp/api/）使用 cpp-httplib 提供 14 个 REST 接口。为提高可维护性和开发效率，决定使用 Node.js + TypeScript + NestJS 替换 C++ HTTP API，C++ 只保留核心分析引擎功能。

## 技术栈

- **后端**: Node.js 20 + TypeScript
- **框架**: NestJS
- **ORM**: Prisma
- **数据库**: MySQL 8.0
- **容器**: Docker

## 架构设计

### 单服务架构

```
backend/
├── src/
│   ├── app.module.ts           # 主模块
│   ├── main.ts                 # 入口文件 (端口 3001)
│   ├── modules/
│   │   ├── stocks/             # 股票模块
│   │   ├── analysis/           # 分析结果模块
│   │   ├── charts/             # 图表数据模块
│   │   ├── strategies/         # 策略模块
│   ├── common/                 # 通用模块
│   │   ├── filters/            # 异常过滤器
│   │   ├── interceptors/       # 拦截器
│   │   ├── decorators/         # 自定义装饰器
│   ├── prisma/                 # Prisma 服务
│   ├── config/                 # 配置服务
├── prisma/
│   ├── schema.prisma           # 数据库模型
├── Dockerfile
├── package.json
```

## API 路径规划

与 C++ ApiRouter.cpp 保持一致：

| 模块 | API 路径 | 说明 |
|------|----------|------|
| **StocksModule** | | |
| | `GET /api/stocks` | 获取股票列表 (limit 参数) |
| | `GET /api/stocks/search` | 搜索股票 (keyword 参数) |
| | `GET /api/stocks/:code` | 获取单个股票详情 |
| **AnalysisModule** | | |
| | `GET /api/analysis/signals` | 获取分析信号列表 (label, limit 参数) |
| | `GET /api/analysis/:code` | 获取股票分析结果 |
| | `GET /api/analysis/progress` | 获取分析进度 |
| | `GET /api/analysis/process` | 获取分析过程记录 (ts_code, start_date, end_date, limit) |
| | `GET /api/analysis/process/strategies` | 获取策略列表 (从 STRATEGIES 环境变量) |
| | `GET /api/analysis/process/:id` | 获取单个分析记录 |
| | `GET /api/analysis/process/chart/:tsCode` | 获取图表数据 (strategy, freq 参数) |
| **ChartsModule** | | |
| | `GET /api/charts/:code` | 获取 K 线数据 (freq, limit 参数) |

### 响应格式

保持与 C++ API 一致的响应格式：

```json
{
  "success": true,
  "data": { ... }
}
```

错误响应：

```json
{
  "success": false,
  "error": "错误信息"
}
```

## Prisma 数据模型

### Stock

```prisma
model Stock {
  id          Int       @id @default(autoincrement())
  tsCode      String    @unique @map("ts_code")
  symbol      String?
  name        String?
  area        String?
  industry    String?
  market      String?
  listStatus  String?   @map("list_status")
  createdAt   DateTime  @default(now()) @map("created_at")
  updatedAt   DateTime  @updatedAt @map("updated_at")
  @@map("stocks")
}
```

### AnalysisResult

```prisma
model AnalysisResult {
  id           Int      @id @default(autoincrement())
  tsCode       String   @map("ts_code")
  strategyName String   @map("strategy_name")
  tradeDate    DateTime @map("trade_date")
  label        String
  freq         String?
  createdAt    DateTime @default(now()) @map("created_at")
  @@unique([tsCode, strategyName, tradeDate, freq])
  @@map("analysis_results")
}
```

### AnalysisProgress

```prisma
model AnalysisProgress {
  id                Int      @id @default(1)
  phase1Status      String   @default("idle") @map("phase1_status")
  phase1Total       Int      @default(0)
  phase1Completed   Int      @default(0)
  phase1Qualified   Int      @default(0)
  phase2Status      String   @default("idle") @map("phase2_status")
  phase2Total       Int      @default(0)
  phase2Completed   Int      @default(0)
  phase2Failed      Int      @default(0)
  startedAt         DateTime? @map("started_at")
  updatedAt         DateTime @updatedAt @map("updated_at")
  @@map("analysis_progress")
}
```

### AnalysisProcessRecord

```prisma
model AnalysisProcessRecord {
  id         Int      @id @default(autoincrement())
  tsCode     String   @map("ts_code")
  stockName  String?  @map("stock_name")
  tradeDate  DateTime @map("trade_date")
  data       Json     @default("{\"strategies\":[]}")
  createdAt  DateTime @default(now()) @map("created_at")
  expiresAt  DateTime? @map("expires_at")
  @@unique([tsCode, tradeDate])
  @@map("analysis_process_records")
}
```

### ChartData

```prisma
model ChartData {
  id          Int      @id @default(autoincrement())
  tsCode      String   @map("ts_code")
  freq        String
  analysisDate DateTime @map("analysis_date")
  data        Json
  updatedAt   DateTime @updatedAt @map("updated_at")
  @@unique([tsCode, freq, analysisDate])
  @@map("chart_data")
}
```

## Docker 配置

### Dockerfile

```dockerfile
FROM node:20-alpine AS builder
WORKDIR /app
COPY package*.json ./
COPY prisma ./prisma/
RUN npm ci
RUN npx prisma generate
COPY . .
RUN npm run build

FROM node:20-alpine
WORKDIR /app
COPY --from=builder /app/dist ./dist
COPY --from=builder /app/node_modules ./node_modules
COPY --from=builder /app/prisma ./prisma
EXPOSE 3001
CMD ["node", "dist/main.js"]
```

### docker-compose.yml 更新

新增 api-server 服务：

```yaml
services:
  api-server:
    build: ./backend
    container_name: stock-api-server
    ports:
      - "3001:3001"
    environment:
      - DATABASE_URL=mysql://stock_user:stock_password@mysql:3306/stock_db
      - STRATEGIES=EMA25_GREATER_17_PRICE_MATCH
      - TZ=Asia/Shanghai
    depends_on:
      mysql:
        condition: service_healthy
    restart: unless-stopped
```

## 与 C++ 后端的协作

### 数据共享

- C++ 分析引擎写入 MySQL 数据库
- Node.js API 从 MySQL 读取数据
- 通过数据库实现数据共享，无需进程间通信

### 环境变量共享

| 变量 | 说明 | 共用 |
|------|------|------|
| `DATABASE_URL` | MySQL 连接字符串 | Node.js 专用 |
| `STRATEGIES` | 策略列表 | C++ 和 Node.js 共用 |

### 移除 C++ HTTP API

实施完成后，从 CMakeLists.txt 移除：
- `api/HttpServer.cpp`
- `api/ApiRouter.cpp`
- `api_lib` 库

## 实施步骤

1. 创建 backend 目录和 NestJS 项目骨架
2. 配置 Prisma 和数据库模型
3. 实现 stocks、analysis、charts 模块
4. 配置 Docker 和 docker-compose
5. 验证 API 接口功能
6. 移除 C++ HTTP API 相关代码
7. 更新 web-frontend API 配置指向新服务

## 验收标准

- 所有 14 个 API 接口正常响应
- 响应格式与 C++ API 一致
- Docker 部署成功
- web-frontend 能正常调用新 API