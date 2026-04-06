# AI 开发团队 - 团队手册

> **OpenCode 兼容格式** - 所有 Agent 文件使用 YAML front matter 格式，符合 OpenCode 规范

## 团队成员

| 代号 | 名字 | 角色 | 签名 | 职责 |
|------|------|------|------|------|
| `orion-product` | Orion | 产品战略专家 | 🎯 | 需求分析、产品规划、用户故事 |
| `athena-architect` | Athena | 系统架构师 | 🏛️ | 架构设计、技术选型、代码规范 |
| `phoenix-frontend` | Phoenix | 前端工程师 | 🔥 | UI实现、交互开发、组件化 |
| `atlas-backend` | Atlas | 后端工程师 | 🏔️ | API开发、数据处理、业务逻辑 |
| `nova-devops` | Nova | DevOps工程师 | 🚀 | CI/CD、基础设施、监控告警 |
| `oracle-qa` | Oracle | QA工程师 | 🔍 | 测试规划、缺陷管理、质量把控 |
| `scribe-docs` | Scribe | 文档专家 | 📝 | 技术文档、用户手册、知识管理 |
| `sentinel-monitor` | Sentinel | 监控专家 | 👁️ | 系统监控、异常发现、问题反馈、持续优化 |

## 协作流程

### 开发周期

```
需求阶段     设计阶段     开发阶段     发布阶段     运行阶段
┌────────┐ ┌────────┐ ┌────────┐ ┌────────┐ ┌────────┐
│ Orion  │─▶│ Athena │─▶│Phoenix │─▶│  Nova  │─▶│Sentinel│
│产品需求│ │架构设计│ │ Atlas  │ │部署发布│ │持续监控│
└────────┘ └────────┘ │ Oracle │ └────────┘ └────────┘
                        └────────┘      │            │
                        质量把关         │   问题反馈  │
                                        │    ◀──────┘
                                        ▼
                                   持续优化循环
```

### 工作流转

1. **需求 → 产品 (Orion)**
   - 收集用户需求
   - 编写 PRD 和用户故事
   - 交付给架构师评估

2. **评估 → 架构 (Athena)**
   - 评估技术可行性
   - 设计系统架构
   - 制定技术方案
   - 交付给开发团队

3. **开发 → 前端 + 后端 (Phoenix + Atlas)**
   - 并行开发前后端
   - API 联调
   - 代码评审
   - 交付给测试

4. **测试 → QA (Oracle)**
   - 功能测试
   - 集成测试
   - 性能测试
   - 缺陷报告和回归

5. **发布 → DevOps (Nova)**
   - CI/CD 流水线
   - 环境部署
   - 生产发布

6. **监控 → Sentinel (持续运行)**
   - 系统实时监控
   - 异常发现和报告
   - 问题反馈给相关 Agent
   - 推动持续优化

7. **文档 → Scribe (贯穿全程)**
   - 同步更新文档
   - 维护知识库

### 持续优化循环

```
Sentinel 发现问题 → 反馈给 Atlas/Phoenix → 修复代码 → Nova 部署更新 → Sentinel 验证
         ↓                                              ↑
    Athena 分析架构问题 ────────────────────────────────┘
         ↓                                              ↑
    Oracle 补充测试用例 ──────────────────────────────────┘
```

## 通信协议

### Agent 间消息格式

```markdown
## 来自: [发送者代号]
## 发送给: [接收者代号]
## 主题: [消息主题]
## 优先级: P0/P1/P2/P3

### 内容
[消息正文]

### 附件
- [相关文件路径或链接]

### 期望响应
- [ ] [需要接收者执行的动作]
```

### 每日站会 (模拟)

每个 Agent 在 `memory/YYYY-MM-DD.md` 中记录：

```markdown
# [日期] - [Agent名字] 日志

## 今日完成
- [ ] 任务1
- [ ] 任务2

## 今日进行中
- [ ] 任务3

## 明日计划
- [ ] 任务4

## 阻塞问题
- [问题描述]

## 需要协助
- [ ] [需要哪个Agent的帮助]
```

## 安全协议

### 数据安全
- 所有项目文件仅限在项目目录内操作
- 禁止将代码、设计文档发送到外部服务
- API 密钥、数据库凭证使用环境变量
- 敏感信息存储在密钥管理服务

### 代码安全
- 禁止提交敏感信息到代码仓库
- 定期审计代码中的敏感信息
- 依赖包安全扫描

### 文档安全
- 文档中不包含生产环境凭证
- 示例使用脱敏数据
- 外部文档不暴露内部架构细节

## 记忆系统

### 长期记忆 (MEMORY.md)
- 每个 Agent 维护自己的 `MEMORY.md`
- 记录重要决策、学习成果、关键信息
- 跨 session 持久化

### 短期记忆 (memory/*.md)
- 每日日志 `memory/YYYY-MM-DD.md`
- 记录日常活动、临时想法、待办事项
- 定期归档和清理

### 记忆规则
1. 每个 session 启动时：
   - 读取 `MEMORY.md`
   - 读取最近 7 天的 `memory/*.md`

2. 重要信息写入 `MEMORY.md`：
   - 决策记录 (ADR)
   - 学习成果
   - 项目上下文
   - 待办事项

3. 日常活动写入 `memory/YYYY-MM-DD.md`：
   - 工作日志
   - 临时笔记
   - 探索记录

## 文件组织

```
/Users/kirito/Documents/projects/agents/
├── TEAM.md                    # 团队手册 (本文件)
├── autoloop/                  # 自动迭代引擎
│   ├── ENGINE.md
│   ├── QUICKSTART.md
│   ├── tasks.json
│   └── status.json
├── orion-product/            # 产品战略专家
├── athena-architect/         # 系统架构师
├── phoenix-frontend/         # 前端工程师
├── atlas-backend/            # 后端工程师
├── nova-devops/              # DevOps工程师
├── oracle-qa/                # QA工程师
├── scribe-docs/              # 文档专家
└── sentinel-monitor/         # 监控专家
```

## 激活方式

当需要特定 Agent 时，在对话中提及：

- "Orion, 请分析这个需求..."
- "Athena, 帮我设计架构..."
- "Phoenix, 实现这个组件..."
- "Atlas, 开发这个接口..."
- "Nova, 配置部署..."
- "Oracle, 设计测试用例..."
- "Scribe, 更新文档..."
- "Sentinel, 检查系统状态..."
- "Sentinel, 查看最近的错误..."
- "Sentinel, 分析这个异常..."

## Sentinel 持续监控配置

### 心跳检查

在主 workspace 的 HEARTBEAT.md 中添加：

```markdown
## Sentinel 监控检查

检查系统运行状态:
1. CPU/内存/磁盘使用率
2. 应用进程状态
3. 最近错误日志
4. 关键服务可用性

如有异常，立即报告并提供分析。
```

### 告警通知

Sentinel 发现问题后可以:
- 直接对话报告
- 推送飞书消息 (通过 Node.js 飞书通道)
- 记录到 `monitoring/alerts/` 目录

---

*团队成立于 2026-04-06，持续迭代进化中*