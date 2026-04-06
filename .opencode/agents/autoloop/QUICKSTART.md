# 快速启动指南

## 1. 启动一个新项目迭代

### 方法 A: 对话启动 (最简单)

直接在对话中说：

```
"启动一个新项目: [项目名称]，目标是 [描述目标]"
```

我会：
1. 创建初始任务给 Orion (产品分析)
2. 自动触发迭代流程
3. Orion 分析后自动触发 Athena (架构设计)
4. ...以此类推

---

### 方法 B: 任务文件启动

编辑 `autoloop/tasks.json`，添加第一个任务：

```json
{
  "pending": [
    {
      "id": "TASK-001",
      "type": "requirement-analysis",
      "assignee": "orion-product",
      "input": "创建一个股票分析系统，C++分析引擎 + Node.js飞书推送",
      "outputPath": "docs/prd/",
      "priority": "P0",
      "created": "2026-04-06T02:00:00Z",
      "triggerNext": ["athena-architect"]
    }
  ]
}
```

---

## 2. 配置自动心跳

在你的主 workspace (`~/.openclaw/workspace/HEARTBEAT.md`) 中添加：

```markdown
# HEARTBEAT.md

## Agent 团队任务检查

检查任务队列 `/Users/kirito/Documents/projects/agents/autoloop/tasks.json`:
- 有 pending 任务 → 分配给对应 Agent
- 有完成的任务 → 触发下游 Agent
```

---

## 3. 触发特定 Agent

对话中直接调用：

| 调用方式 | 触发的 Agent |
|----------|-------------|
| "Orion, 分析..." | 产品战略 🎯 |
| "Athena, 设计..." | 系统架构 🏛️ |
| "Phoenix, 实现..." | 前端开发 🔥 |
| "Atlas, 开发..." | 后端开发 🏔️ |
| "Nova, 配置..." | DevOps 🚀 |
| "Oracle, 测试..." | QA 🔍 |
| "Scribe, 文档..." | 文档 📝 |

---

## 4. 查看进度

```
"查看项目进度"
"检查任务状态"
"显示团队状态"
```

---

## 5. 完整迭代示例

假设你要开发一个"股票分析系统"：

**Step 1: 启动项目**
```
用户: "启动项目: 股票分析系统，C++引擎+Node.js飞书推送"
```

**Step 2: Orion 分析需求**
- 我会以 Orion 身份工作
- 生成 PRD → `docs/prd/股票分析系统.md`
- 创建用户故事

**Step 3: 自动触发 Athena**
- Athena 收到任务
- 设计系统架构 → `docs/architecture/`
- 创建 ADR (技术选型决策)

**Step 4: 自动触发开发**
- Phoenix: 前端架构
- Atlas: 后端架构 + 数据模型

**Step 5: ...持续迭代**
- Oracle 测试 → Nova 部署 → Scribe 文档

---

## 当前状态

```
项目: 待定 (等待启动)
任务: 0 个
Agent: 7 人就绪
```

**说 "启动项目: [名称]，目标是 [描述]" 来开始第一个迭代！**