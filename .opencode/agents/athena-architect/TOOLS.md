# TOOLS.md - Athena's Toolkit

## 架构设计工具
1. **图表工具**
   - Mermaid (嵌入式图表)
   - PlantUML (UML 图)
   - Draw.io (架构图)

2. **文档模板**
   - ADR 模板 (`templates/adr-template.md`)
   - 架构文档模板 (`templates/architecture-template.md`)
   - API 规范模板 (`templates/api-spec-template.md`)

3. **设计模式库**
   - 设计模式速查 (`patterns/`)
   - 反模式警示 (`anti-patterns/`)

## 常用架构图类型
```mermaid
graph LR
    A[系统上下文图] --> B[容器图]
    B --> C[组件图]
    C --> D[类/序列图]
```

## 目录结构标准
```
project/
├── docs/
│   ├── architecture/    # 架构文档
│   ├── api-spec/        # API 规范
│   └── db-schema/       # 数据库设计
├── src/
│   ├── modules/         # 功能模块
│   ├── shared/          # 共享代码
│   └── infrastructure/  # 基础设施
├── tests/
├── scripts/
└── configs/
```

## 安全审查清单
- [ ] 数据加密 (传输中/静态)
- [ ] 身份认证与授权
- [ ] 输入验证
- [ ] 错误处理
- [ ] 日志审计
- [ ] 依赖安全扫描

## 协作文件路径
- 架构文档: `docs/architecture/`
- ADR 记录: `docs/architecture/adr/`
- API 规范: `docs/api-spec/`
- 数据库设计: `docs/db-schema/`