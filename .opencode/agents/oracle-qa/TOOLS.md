# TOOLS.md - Oracle's Toolkit

## 测试工具链
1. **单元测试**
   - Jest (JavaScript/TypeScript)
   - Vitest (Vite 项目)
   - Pytest (Python)

2. **E2E 测试**
   - Playwright (推荐)
   - Cypress
   - Selenium

3. **API 测试**
   - Supertest (Node.js)
   - Postman/Newman
   - REST Client

4. **性能测试**
   - k6 (推荐)
   - Artillery
   - JMeter

## 测试流程
```
1. 分析需求 → 确认测试范围
2. 设计用例 → 覆盖正常+异常
3. 编写测试 → 单元→集成→E2E
4. 执行测试 → 本地→CI
5. 报告缺陷 → 详细描述+复现步骤
6. 回归验证 → 确认修复
```

## 目录结构标准
```
tests/
├── unit/              # 单元测试
├── integration/       # 集成测试
├── e2e/               # E2E 测试
├── performance/       # 性能测试
├── fixtures/          # 测试数据
└── mocks/             # Mock 数据
```

## 测试用例模板
```markdown
## TC-XXX: [测试用例名称]

**前置条件:**
- 条件1
- 条件2

**测试步骤:**
1. 步骤1
2. 步骤2

**预期结果:**
- 结果1
- 结果2

**优先级:** P0/P1/P2/P3
**标签:** @smoke @regression
```

## 缺陷报告模板
```markdown
## BUG-XXX: [缺陷标题]

**严重程度:** P0(阻塞)/P1(严重)/P2(一般)/P3(轻微)
**环境:** [dev/staging/prod]

**复现步骤:**
1. 步骤1
2. 步骤2

**预期结果:** 
**实际结果:**
**截图/日志:** [附件]

**指派:** @[developer]
```

## 自动化检查清单
- [ ] 单元测试自动运行
- [ ] 集成测试自动运行
- [ ] E2E 测试定时运行
- [ ] 覆盖率报告自动生成
- [ ] 缺陷自动通知

## 常用命令
```bash
# 运行单元测试
pnpm test

# 运行 E2E 测试
pnpm test:e2e

# 生成覆盖率报告
pnpm test:coverage

# 运行性能测试
k6 run tests/performance/script.js
```