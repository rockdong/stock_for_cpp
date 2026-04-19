# Node.js API Service Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Replace C++ HTTP API with Node.js + NestJS + Prisma service providing 14 REST API endpoints.

**Architecture:** NestJS single-service with modular structure (stocks, analysis, charts modules). Prisma ORM for MySQL. Docker containerization.

**Tech Stack:** Node.js 20, TypeScript, NestJS, Prisma, MySQL, Docker

---

## Task 1: Create Backend Project Skeleton

**Files:**
- Create: `backend/package.json`
- Create: `backend/tsconfig.json`
- Create: `backend/nest-cli.json`
- Create: `backend/.env`

**Step 1: Create backend directory and NestJS project**

```bash
mkdir -p backend
cd backend
npm init -y
```

**Step 2: Install NestJS CLI and dependencies**

```bash
npm install @nestjs/core @nestjs/common @nestjs/platform-express rxjs reflect-metadata
npm install @nestjs/config @prisma/client
npm install -D typescript @types/node @types/express ts-node nodemon jest @types/jest ts-jest
```

**Step 3: Create package.json scripts**

Edit `backend/package.json`:

```json
{
  "name": "stock-api-server",
  "version": "1.0.0",
  "scripts": {
    "build": "nest build",
    "start": "node dist/main.js",
    "dev": "nest start --watch",
    "test": "jest"
  },
  "dependencies": {
    "@nestjs/common": "^11.0.0",
    "@nestjs/core": "^11.0.0",
    "@nestjs/platform-express": "^11.0.0",
    "@nestjs/config": "^3.0.0",
    "@prisma/client": "^5.0.0",
    "reflect-metadata": "^0.2.0",
    "rxjs": "^7.8.0"
  },
  "devDependencies": {
    "@nestjs/cli": "^11.0.0",
    "@types/node": "^20.0.0",
    "@types/express": "^4.17.0",
    "typescript": "^5.0.0",
    "ts-node": "^10.9.0",
    "nodemon": "^3.0.0",
    "jest": "^29.0.0",
    "ts-jest": "^29.0.0"
  }
}
```

**Step 4: Create tsconfig.json**

```json
{
  "compilerOptions": {
    "module": "commonjs",
    "declaration": true,
    "removeComments": true,
    "emitDecoratorMetadata": true,
    "experimentalDecorators": true,
    "allowSyntheticDefaultImports": true,
    "target": "ES2021",
    "sourceMap": true,
    "outDir": "./dist",
    "baseUrl": "./",
    "incremental": true,
    "skipLibCheck": true,
    "strictNullChecks": true,
    "noImplicitAny": true,
    "strictBindCallApply": true,
    "forceConsistentCasingInFileNames": true,
    "noFallthroughCasesInSwitch": true
  }
}
```

**Step 5: Create nest-cli.json**

```json
{
  "collection": "@nestjs/schematics",
  "sourceRoot": "src",
  "compilerOptions": {
    "deleteOutDir": true
  }
}
```

**Step 6: Create .env**

```env
DATABASE_URL=mysql://stock_user:stock_password@mysql:3306/stock_db
STRATEGIES=EMA25_GREATER_17_PRICE_MATCH
PORT=3001
TZ=Asia/Shanghai
```

**Step 7: Commit**

```bash
git add backend/
git commit -m "feat: create NestJS backend project skeleton"
```

---

## Task 2: Setup Prisma

**Files:**
- Create: `backend/prisma/schema.prisma`
- Run: Prisma generate

**Step 1: Initialize Prisma**

```bash
cd backend
npx prisma init
```

**Step 2: Write Prisma schema**

Replace `backend/prisma/schema.prisma`:

```prisma
generator client {
  provider = "prisma-client-js"
}

datasource db {
  provider = "mysql"
  url      = env("DATABASE_URL")
}

model Stock {
  id          Int       @id @default(autoincrement())
  tsCode      String    @unique @map("ts_code") @db.VarChar(20)
  symbol      String?   @db.VarChar(10)
  name        String?   @db.VarChar(100)
  area        String?   @db.VarChar(20)
  industry    String?   @db.VarChar(50)
  market      String?   @db.VarChar(20)
  exchange    String?   @db.VarChar(20)
  listStatus  String?   @map("list_status") @db.VarChar(10)
  createdAt   DateTime  @default(now()) @map("created_at")
  updatedAt   DateTime  @updatedAt @map("updated_at")

  @@map("stocks")
}

model AnalysisResult {
  id           Int      @id @default(autoincrement())
  tsCode       String   @map("ts_code") @db.VarChar(20)
  strategyName String   @map("strategy_name") @db.VarChar(50)
  tradeDate    DateTime @map("trade_date") @db.Date
  label        String   @db.VarChar(20)
  freq         String?  @db.VarChar(5)
  createdAt    DateTime @default(now()) @map("created_at")

  @@unique([tsCode, strategyName, tradeDate, freq])
  @@index([tsCode])
  @@index([strategyName])
  @@index([tradeDate])
  @@map("analysis_results")
}

model AnalysisProgress {
  id                Int      @id
  phase1Status      String   @default("idle") @map("phase1_status") @db.VarChar(20)
  phase1Total       Int      @default(0) @map("phase1_total")
  phase1Completed   Int      @default(0) @map("phase1_completed")
  phase1Qualified   Int      @default(0) @map("phase1_qualified")
  phase2Status      String   @default("idle") @map("phase2_status") @db.VarChar(20)
  phase2Total       Int      @default(0) @map("phase2_total")
  phase2Completed   Int      @default(0) @map("phase2_completed")
  phase2Failed      Int      @default(0) @map("phase2_failed")
  startedAt         DateTime? @map("started_at")
  phase1CompletedAt DateTime? @map("phase1_completed_at")
  updatedAt         DateTime  @updatedAt @map("updated_at")

  @@map("analysis_progress")
}

model AnalysisProcessRecord {
  id         Int      @id @default(autoincrement())
  tsCode     String   @map("ts_code") @db.VarChar(20)
  stockName  String?  @map("stock_name") @db.VarChar(100)
  tradeDate  DateTime @map("trade_date") @db.Date
  data       Json     @default("{\"strategies\":[]}")
  createdAt  DateTime  @default(now()) @map("created_at")
  expiresAt  DateTime? @map("expires_at")

  @@unique([tsCode, tradeDate])
  @@index([tsCode])
  @@index([tradeDate])
  @@map("analysis_process_records")
}

model ChartData {
  id          Int      @id @default(autoincrement())
  tsCode      String   @map("ts_code") @db.VarChar(20)
  freq        String   @db.VarChar(5)
  analysisDate DateTime @map("analysis_date") @db.Date
  data        Json
  updatedAt   DateTime  @updatedAt @map("updated_at")

  @@unique([tsCode, freq, analysisDate])
  @@map("chart_data")
}
```

**Step 3: Generate Prisma client**

```bash
npx prisma generate
```

Expected: `Prisma client generated successfully`

**Step 4: Commit**

```bash
git add backend/prisma/
git commit -m "feat: setup Prisma schema for MySQL"
```

---

## Task 3: Create Core Modules (main.ts, app.module, prisma.service)

**Files:**
- Create: `backend/src/main.ts`
- Create: `backend/src/app.module.ts`
- Create: `backend/src/prisma/prisma.module.ts`
- Create: `backend/src/prisma/prisma.service.ts`

**Step 1: Create main.ts**

```typescript
import { NestFactory } from '@nestjs/core';
import { AppModule } from './app.module';

async function bootstrap() {
  const app = await NestFactory.create(AppModule);
  app.enableCors();
  await app.listen(process.env.PORT || 3001);
}
bootstrap();
```

**Step 2: Create prisma.service.ts**

```typescript
import { Injectable, OnModuleInit, OnModuleDestroy } from '@nestjs/common';
import { PrismaClient } from '@prisma/client';

@Injectable()
export class PrismaService extends PrismaClient implements OnModuleInit, OnModuleDestroy {
  async onModuleInit() {
    await this.$connect();
  }

  async onModuleDestroy() {
    await this.$disconnect();
  }
}
```

**Step 3: Create prisma.module.ts**

```typescript
import { Global, Module } from '@nestjs/common';
import { PrismaService } from './prisma.service';

@Global()
@Module({
  providers: [PrismaService],
  exports: [PrismaService],
})
export class PrismaModule {}
```

**Step 4: Create app.module.ts**

```typescript
import { Module } from '@nestjs/common';
import { ConfigModule } from '@nestjs/config';
import { PrismaModule } from './prisma/prisma.module';
import { StocksModule } from './modules/stocks/stocks.module';
import { AnalysisModule } from './modules/analysis/analysis.module';
import { ChartsModule } from './modules/charts/charts.module';

@Module({
  imports: [
    ConfigModule.forRoot({ isGlobal: true }),
    PrismaModule,
    StocksModule,
    AnalysisModule,
    ChartsModule,
  ],
})
export class AppModule {}
```

**Step 5: Commit**

```bash
git add backend/src/
git commit -m "feat: create NestJS core modules (main, app, prisma)"
```

---

## Task 4: Create StocksModule

**Files:**
- Create: `backend/src/modules/stocks/stocks.module.ts`
- Create: `backend/src/modules/stocks/stocks.controller.ts`
- Create: `backend/src/modules/stocks/stocks.service.ts`
- Create: `backend/src/modules/stocks/stocks.dto.ts`

**Step 1: Create stocks.dto.ts**

```typescript
export class GetStocksDto {
  limit?: number = 100;
}

export class SearchStocksDto {
  keyword?: string;
}
```

**Step 2: Create stocks.service.ts**

```typescript
import { Injectable } from '@nestjs/common';
import { PrismaService } from '../../prisma/prisma.service';
import { GetStocksDto, SearchStocksDto } from './stocks.dto';

@Injectable()
export class StocksService {
  constructor(private prisma: PrismaService) {}

  async findAll(dto: GetStocksDto) {
    const stocks = await this.prisma.stock.findMany({
      take: dto.limit,
      select: {
        tsCode: true,
        name: true,
        industry: true,
        market: true,
      },
    });
    return stocks;
  }

  async search(dto: SearchStocksDto) {
    if (!dto.keyword) return [];
    const stocks = await this.prisma.stock.findMany({
      where: {
        OR: [
          { name: { contains: dto.keyword } },
          { tsCode: { contains: dto.keyword } },
        ],
      },
      take: 20,
      select: {
        tsCode: true,
        name: true,
        industry: true,
        market: true,
      },
    });
    return stocks;
  }

  async findByCode(code: string) {
    const stock = await this.prisma.stock.findUnique({
      where: { tsCode: code },
      select: {
        tsCode: true,
        name: true,
        industry: true,
        market: true,
      },
    });
    return stock;
  }
}
```

**Step 3: Create stocks.controller.ts**

```typescript
import { Controller, Get, Query, Param } from '@nestjs/common';
import { StocksService } from './stocks.service';
import { GetStocksDto, SearchStocksDto } from './stocks.dto';

@Controller('api/stocks')
export class StocksController {
  constructor(private stocksService: StocksService) {}

  @Get()
  async findAll(@Query() dto: GetStocksDto) {
    const data = await this.stocksService.findAll(dto);
    return { success: true, data };
  }

  @Get('search')
  async search(@Query() dto: SearchStocksDto) {
    const data = await this.stocksService.search(dto);
    return { success: true, data };
  }

  @Get(':code')
  async findByCode(@Param('code') code: string) {
    const data = await this.stocksService.findByCode(code);
    if (!data) return { success: false, error: '股票不存在' };
    return { success: true, data };
  }
}
```

**Step 4: Create stocks.module.ts**

```typescript
import { Module } from '@nestjs/common';
import { StocksController } from './stocks.controller';
import { StocksService } from './stocks.service';

@Module({
  controllers: [StocksController],
  providers: [StocksService],
})
export class StocksModule {}
```

**Step 5: Commit**

```bash
git add backend/src/modules/stocks/
git commit -m "feat: create StocksModule with 3 API endpoints"
```

---

## Task 5: Create ChartsModule

**Files:**
- Create: `backend/src/modules/charts/charts.module.ts`
- Create: `backend/src/modules/charts/charts.controller.ts`
- Create: `backend/src/modules/charts/charts.service.ts`

**Step 1: Create charts.service.ts**

```typescript
import { Injectable } from '@nestjs/common';
import { PrismaService } from '../../prisma/prisma.service';

@Injectable()
export class ChartsService {
  constructor(private prisma: PrismaService) {}

  async getChartData(tsCode: string, freq: string = 'd', limit: number = 100) {
    const records = await this.prisma.chartData.findMany({
      where: { tsCode, freq },
      orderBy: { updatedAt: 'desc' },
      take: limit,
    });
    
    const candles = records.map(r => r.data as any).flat().slice(-limit);
    return candles.reverse();
  }
}
```

**Step 2: Create charts.controller.ts**

```typescript
import { Controller, Get, Param, Query } from '@nestjs/common';
import { ChartsService } from './charts.service';

@Controller('api/charts')
export class ChartsController {
  constructor(private chartsService: ChartsService) {}

  @Get(':code')
  async getChartData(
    @Param('code') code: string,
    @Query('freq') freq: string = 'd',
    @Query('limit') limit: string = '100',
  ) {
    const data = await this.chartsService.getChartData(code, freq, parseInt(limit));
    return { success: true, data };
  }
}
```

**Step 3: Create charts.module.ts**

```typescript
import { Module } from '@nestjs/common';
import { ChartsController } from './charts.controller';
import { ChartsService } from './charts.service';

@Module({
  controllers: [ChartsController],
  providers: [ChartsService],
})
export class ChartsModule {}
```

**Step 4: Commit**

```bash
git add backend/src/modules/charts/
git commit -m "feat: create ChartsModule with chart data endpoint"
```

---

## Task 6: Create AnalysisModule

**Files:**
- Create: `backend/src/modules/analysis/analysis.module.ts`
- Create: `backend/src/modules/analysis/analysis.controller.ts`
- Create: `backend/src/modules/analysis/analysis.service.ts`

**Step 1: Create analysis.service.ts**

```typescript
import { Injectable } from '@nestjs/common';
import { ConfigService } from '@nestjs/config';
import { PrismaService } from '../../prisma/prisma.service';

@Injectable()
export class AnalysisService {
  constructor(
    private prisma: PrismaService,
    private config: ConfigService,
  ) {}

  async getSignals(label?: string, limit: number = 50) {
    const where = label ? { label } : {};
    const results = await this.prisma.analysisResult.findMany({
      where,
      orderBy: { tradeDate: 'desc' },
      take: limit,
      select: {
        tsCode: true,
        strategyName: true,
        label: true,
        freq: true,
        tradeDate: true,
      },
    });
    return results.map(r => ({
      ...r,
      trade_date: r.tradeDate.toISOString().split('T')[0],
    }));
  }

  async getProgress() {
    const progress = await this.prisma.analysisProgress.findUnique({
      where: { id: 1 },
    });
    return {
      id: progress?.id || 1,
      phase1: {
        status: progress?.phase1Status || 'idle',
        total: progress?.phase1Total || 0,
        completed: progress?.phase1Completed || 0,
        qualified: progress?.phase1Qualified || 0,
      },
      phase2: {
        status: progress?.phase2Status || 'idle',
        total: progress?.phase2Total || 0,
        completed: progress?.phase2Completed || 0,
        failed: progress?.phase2Failed || 0,
      },
      started_at: progress?.startedAt || null,
      phase1_completed_at: progress?.phase1CompletedAt || null,
      updated_at: progress?.updatedAt?.toISOString() || '',
      execute_time: this.config.get('SCHEDULER_EXECUTE_TIME', '20:00'),
    };
  }

  async getProcessRecords(params: { ts_code?: string; start_date?: string; end_date?: string; limit?: number }) {
    const where: any = {};
    if (params.ts_code) where.tsCode = params.ts_code;
    if (params.start_date) where.tradeDate = { ...where.tradeDate, gte: new Date(params.start_date) };
    if (params.end_date) where.tradeDate = { ...where.tradeDate, lte: new Date(params.end_date) };

    const records = await this.prisma.analysisProcessRecord.findMany({
      where,
      orderBy: [{ tradeDate: 'desc' }, { createdAt: 'desc' }],
      take: params.limit || 100,
    });

    return records.map(r => ({
      id: r.id,
      ts_code: r.tsCode,
      stock_name: r.stockName,
      trade_date: r.tradeDate.toISOString().split('T')[0],
      data: r.data,
      created_at: r.createdAt.toISOString(),
    }));
  }

  async getProcessById(id: number) {
    const record = await this.prisma.analysisProcessRecord.findUnique({
      where: { id },
    });
    if (!record) return null;
    return {
      id: record.id,
      ts_code: record.tsCode,
      stock_name: record.stockName,
      trade_date: record.tradeDate.toISOString().split('T')[0],
      data: record.data,
      created_at: record.createdAt.toISOString(),
    };
  }

  async getStrategies() {
    const strategiesEnv = this.config.get('STRATEGIES', '');
    if (!strategiesEnv) return [];
    return strategiesEnv.split(';').filter(s => s.trim());
  }

  async getProcessChart(tsCode: string, strategy?: string, freq: string = 'd') {
    const record = await this.prisma.analysisProcessRecord.findFirst({
      where: { tsCode: tsCode },
      orderBy: { tradeDate: 'desc' },
    });

    if (!record) return [];

    const data = record.data as any;
    if (!data.strategies) return [];

    for (const s of data.strategies) {
      if (strategy && !s.name.includes(strategy)) continue;
      for (const f of s.freqs || []) {
        if (f.freq === freq) {
          return f.candles || [];
        }
      }
    }
    return [];
  }

  async getByCode(code: string) {
    const result = await this.prisma.analysisResult.findFirst({
      where: { tsCode: code },
      orderBy: { tradeDate: 'desc' },
    });
    if (!result) return null;
    return {
      ts_code: result.tsCode,
      strategy_name: result.strategyName,
      label: result.label,
      freq: result.freq,
      trade_date: result.tradeDate.toISOString().split('T')[0],
    };
  }
}
```

**Step 2: Create analysis.controller.ts**

```typescript
import { Controller, Get, Query, Param } from '@nestjs/common';
import { AnalysisService } from './analysis.service';

@Controller('api/analysis')
export class AnalysisController {
  constructor(private analysisService: AnalysisService) {}

  @Get('signals')
  async getSignals(
    @Query('label') label?: string,
    @Query('limit') limit?: string,
  ) {
    const data = await this.analysisService.getSignals(label, parseInt(limit || '50'));
    return { success: true, data };
  }

  @Get('progress')
  async getProgress() {
    const data = await this.analysisService.getProgress();
    return { success: true, data };
  }

  @Get('process')
  async getProcess(
    @Query('ts_code') ts_code?: string,
    @Query('start_date') start_date?: string,
    @Query('end_date') end_date?: string,
    @Query('limit') limit?: string,
  ) {
    const data = await this.analysisService.getProcessRecords({
      ts_code,
      start_date,
      end_date,
      limit: parseInt(limit || '100'),
    });
    return { success: true, data };
  }

  @Get('process/strategies')
  async getStrategies() {
    const data = await this.analysisService.getStrategies();
    return { success: true, data };
  }

  @Get('process/chart/:tsCode')
  async getProcessChart(
    @Param('tsCode') tsCode: string,
    @Query('strategy') strategy?: string,
    @Query('freq') freq?: string,
  ) {
    const data = await this.analysisService.getProcessChart(tsCode, strategy, freq || 'd');
    return { success: true, data };
  }

  @Get('process/:id')
  async getProcessById(@Param('id') id: string) {
    const data = await this.analysisService.getProcessById(parseInt(id));
    if (!data) return { success: false, error: '记录不存在' };
    return { success: true, data };
  }

  @Get(':code')
  async getByCode(@Param('code') code: string) {
    const data = await this.analysisService.getByCode(code);
    if (!data) return { success: false, error: '无分析结果' };
    return { success: true, data };
  }
}
```

**Step 3: Create analysis.module.ts**

```typescript
import { Module } from '@nestjs/common';
import { AnalysisController } from './analysis.controller';
import { AnalysisService } from './analysis.service';

@Module({
  controllers: [AnalysisController],
  providers: [AnalysisService],
})
export class AnalysisModule {}
```

**Step 4: Commit**

```bash
git add backend/src/modules/analysis/
git commit -m "feat: create AnalysisModule with 8 API endpoints"
```

---

## Task 7: Create Dockerfile

**Files:**
- Create: `backend/Dockerfile`

**Step 1: Create Dockerfile**

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

**Step 2: Commit**

```bash
git add backend/Dockerfile
git commit -m "feat: create backend Dockerfile"
```

---

## Task 8: Update docker-compose.yml

**Files:**
- Modify: `docker-compose.yml`

**Step 1: Add api-server service to docker-compose.yml**

Add after mysql service:

```yaml
  api-server:
    build: ./backend
    container_name: stock-api-server
    platform: linux/amd64
    ports:
      - "3001:3001"
    environment:
      - DATABASE_URL=mysql://stock_user:stock_password@mysql:3306/stock_db
      - STRATEGIES=EMA25_GREATER_17_PRICE_MATCH
      - PORT=3001
      - TZ=Asia/Shanghai
    depends_on:
      mysql:
        condition: service_healthy
    ulimits:
      nofile:
        soft: 65536
        hard: 65536
    restart: unless-stopped
```

**Step 2: Commit**

```bash
git add docker-compose.yml
git commit -m "feat: add api-server service to docker-compose"
```

---

## Task 9: Update web-frontend API configuration

**Files:**
- Modify: `web-frontend/src/services/api.ts`

**Step 1: Update base URL to use new API server**

Change base URL from port 3000 to 3001 (or use environment variable):

```typescript
const API_BASE_URL = process.env.VITE_API_URL || 'http://localhost:3001';
```

**Step 2: Commit**

```bash
git add web-frontend/src/services/api.ts
git commit -m "feat: update web-frontend API to use new backend"
```

---

## Task 10: Remove C++ HTTP API code

**Files:**
- Delete: `cpp/api/HttpServer.cpp`
- Delete: `cpp/api/HttpServer.h`
- Delete: `cpp/api/ApiRouter.cpp`
- Delete: `cpp/api/ApiRouter.h`
- Modify: `cpp/CMakeLists.txt`

**Step 1: Remove api files**

```bash
rm cpp/api/HttpServer.cpp cpp/api/HttpServer.h cpp/api/ApiRouter.cpp cpp/api/ApiRouter.h
```

**Step 2: Update CMakeLists.txt to remove api_lib**

Remove lines:
- `add_library(api_lib STATIC ...)` section
- `target_link_libraries(stock_for_cpp PRIVATE ...)`中的 `api_lib`

**Step 3: Commit**

```bash
git add -A cpp/
git commit -m "refactor: remove C++ HTTP API code (replaced by Node.js)"
```

---

## Task 11: Final verification

**Step 1: Build and test**

```bash
docker-compose build api-server
docker-compose up -d api-server
docker-compose logs api-server
```

Expected: Server running on port 3001

**Step 2: Test API endpoints**

```bash
curl http://localhost:3001/api/stocks?limit=5
curl http://localhost:3001/api/analysis/progress
curl http://localhost:3001/api/analysis/process/strategies
```

Expected: JSON responses with `{ success: true, data: ... }`

**Step 3: Final commit**

```bash
git add -A
git commit -m "feat: complete Node.js API service migration"
git push origin master
```

---

## Summary

**Total commits**: 11
**Files created**: ~20
**Files deleted**: 4 (C++ API files)
**API endpoints**: 14 migrated from C++ to Node.js