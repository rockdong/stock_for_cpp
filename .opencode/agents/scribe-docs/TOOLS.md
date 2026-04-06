# TOOLS.md - Scribe's Toolkit

## 文档工具链
1. **静态文档站点**
   - Docusaurus (推荐)
   - VitePress
   - MkDocs

2. **API 文档**
   - OpenAPI/Swagger
   - TypeDoc (TypeScript)
   - JSDoc

3. **协作工具**
   - Notion
   - Confluence
   - GitBook

4. **图表工具**
   - Mermaid (嵌入式)
   - Draw.io
   - Excalidraw

## 文档目录标准
```
docs/
├── README.md           # 文档首页
├── getting-started/    # 快速开始
│   ├── installation.md
│   ├── quick-start.md
│   └── configuration.md
├── guides/             # 使用指南
│   ├── basic-usage.md
│   └── advanced.md
├── api/                # API 文档
│   ├── overview.md
│   └── endpoints/
├── architecture/       # 架构文档
│   ├── overview.md
│   └── decisions/
├── faq/               # 常见问题
└── changelog/         # 变更日志
```

## 文档模板

### README 模板
```markdown
# 项目名称

简短描述项目是做什么的。

## 特性

- 特性1
- 特性2

## 快速开始

### 安装

\`\`\`bash
npm install package-name
\`\`\`

### 使用

\`\`\`javascript
import { something } from 'package-name'
\`\`\`

## 文档

[查看完整文档](./docs/)

## 贡献

[贡献指南](./CONTRIBUTING.md)

## 许可证

MIT
```

### API 文档模板
```markdown
# API 名称

## 描述

简短描述这个 API 的作用。

## 请求

**方法:** `GET/POST/PUT/DELETE`
**路径:** `/api/v1/resource`

### 参数

| 参数名 | 类型 | 必填 | 描述 |
|--------|------|------|------|
| id | string | 是 | 资源ID |

### 请求示例

\`\`\`bash
curl -X GET "https://api.example.com/v1/resource/123"
\`\`\`

## 响应

### 成功响应

\`\`\`json
{
  "code": 200,
  "data": {}
}
\`\`\`

### 错误响应

\`\`\`json
{
  "code": 400,
  "message": "错误信息"
}
\`\`\`
```

## 写作规范
- **标题**: 使用 ATX 风格 (`#` 开头)
- **列表**: 使用 `-` 开头
- **代码**: 指定语言高亮
- **链接**: 使用相对路径
- **图片**: 存放在 `assets/` 目录

## 常用命令
```bash
# 启动文档服务器
pnpm docs:dev

# 构建文档
pnpm docs:build

# 检查链接有效性
pnpm docs:check
```