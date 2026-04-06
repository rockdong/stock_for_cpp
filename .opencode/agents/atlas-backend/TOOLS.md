# TOOLS.md - Atlas's Toolkit

## 开发工具链
1. **框架**
   - NestJS (推荐，企业级)
   - Express/Fastify (轻量级)
   - TypeORM/Prisma (ORM)

2. **代码质量**
   - ESLint + TypeScript
   - Jest (单元测试)
   - Supertest (API 测试)
   - Husky + lint-staged

3. **数据库工具**
   - Prisma Migrate
   - pgAdmin / DBeaver
   - Redis Insight

## API 开发流程
```
1. 定义 API 契约 → OpenAPI 规范
2. 设计数据模型 → ORM Schema
3. 实现业务逻辑 → Service Layer
4. 编写单元测试 → 覆盖核心逻辑
5. 编写集成测试 → 端到端验证
6. 代码评审 → 合并发布
```

## 目录结构标准
```
src/
├── modules/           # 业务模块
│   └── [module]/
│       ├── controller.ts
│       ├── service.ts
│       ├── dto.ts
│       └── entity.ts
├── common/            # 公共代码
│   ├── guards/       # 守卫
│   ├── decorators/   # 装饰器
│   ├── filters/      # 异常过滤器
│   └── interceptors/ # 拦截器
├── config/            # 配置
├── database/          # 数据库相关
└── main.ts            # 入口
```

## 数据库设计原则
- 表名使用蛇形命名: `user_orders`
- 字段名使用蛇形命名: `created_at`
- 必备字段: `id`, `created_at`, `updated_at`
- 软删除: `deleted_at`
- 合理使用索引
- 避免 N+1 查询

## 安全检查清单
- [ ] 参数验证 (class-validator)
- [ ] SQL 参数化查询
- [ ] 认证中间件
- [ ] 授权守卫
- [ ] 速率限制
- [ ] CORS 配置
- [ ] Helmet 安全头

## 常用命令
```bash
# 开发服务器
pnpm dev

# 数据库迁移
pnpm migrate:dev

# 运行测试
pnpm test

# 构建生产版本
pnpm build
```