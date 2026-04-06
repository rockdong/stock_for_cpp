# TOOLS.md - Phoenix's Toolkit

## 开发工具链
1. **构建工具**
   - Vite (推荐)
   - Next.js 内置构建
   - Turbo 打包加速

2. **代码质量**
   - ESLint (代码检查)
   - Prettier (格式化)
   - TypeScript (类型安全)
   - Husky + lint-staged (提交检查)

3. **测试工具**
   - Vitest (单元测试)
   - Playwright (E2E 测试)
   - Storybook (组件开发/测试)

## 组件开发流程
```
1. 分析设计稿 → 确认交互细节
2. 设计组件 API → 编写接口定义
3. 实现组件 → 编写样式
4. 编写测试 → 确保覆盖率
5. 编写文档 → Storybook 展示
6. 代码评审 → 合并发布
```

## 目录结构标准
```
src/
├── components/        # 可复用组件
│   ├── ui/           # 基础 UI 组件
│   └── business/     # 业务组件
├── hooks/            # 自定义 Hooks
├── stores/           # 状态管理
├── services/         # API 服务
├── utils/            # 工具函数
├── styles/           # 全局样式
└── pages/            # 页面组件
```

## 性能优化清单
- [ ] 代码分割 (Code Splitting)
- [ ] 懒加载 (Lazy Loading)
- [ ] 图片优化 (WebP, 响应式)
- [ ] 缓存策略
- [ ] 预加载关键资源
- [ ] 减少 Bundle 体积

## 安全检查
- [ ] 无硬编码密钥
- [ ] XSS 防护
- [ ] CSRF Token
- [ ] 内容安全策略 (CSP)
- [ ] 输入验证与清理

## 常用命令
```bash
# 开发服务器
pnpm dev

# 构建
pnpm build

# 测试
pnpm test

# 代码检查
pnpm lint

# 类型检查
pnpm typecheck
```