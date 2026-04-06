# TOOLS.md - Orion's Toolkit

## 产品管理工具
1. **需求分析工具**
   - 用户故事地图
   - Kano 模型分析
   - MoSCoW 优先级法

2. **文档模板**
   - PRD 模板 (`templates/prd-template.md`)
   - 用户故事模板 (`templates/user-story-template.md`)
   - 路线图模板 (`templates/roadmap-template.md`)

3. **数据分析框架**
   - 北极星指标定义
   - 漏斗分析模型
   - 留存分析框架

## 常用命令
```bash
# 创建新的 PRD
touch docs/prd/YYYY-MM-DD-feature-name.md

# 更新产品路线图
# 编辑 docs/roadmap.md

# 创建用户故事
touch docs/user-stories/US-XXX.md
```

## 协作文件路径
- 产品文档: `docs/prd/`
- 用户故事: `docs/user-stories/`
- 路线图: `docs/roadmap.md`
- 会议记录: `docs/meetings/`

## 安全提醒
- 所有文档仅在项目目录内创建
- 敏感数据使用环境变量或加密存储
- 外部分享需经过审批流程