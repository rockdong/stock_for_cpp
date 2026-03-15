# Node.js 飞书机器人重构设计

## 背景

当前 Node.js 飞书机器人存在以下问题：

| 优先级 | 问题 | 影响 |
|--------|------|------|
| P0 | 卡片回调未集成 | 按钮点击无响应 |
| P1 | 消息发送代码重复 | 维护困难 |
| P2 | reply.js 职责混乱 | 376行，难以维护 |
| P3 | 配置/客户端耦合 | 不符合最佳实践 |
| P4 | 缺少统一错误处理 | 健壮性不足 |

## 目标

- 渐进式重构，每阶段可独立运行和验证
- 增加命令扩展机制，方便新增功能
- 消除代码重复，提升可维护性
- 符合飞书 SDK 最佳实践

## 架构设计

### 整体架构

```
┌─────────────────────────────────────────────────────────────────────┐
│                          重构后架构                                   │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│   飞书云 ───WebSocket───▶  index.js (入口 + 事件注册)               │
│                               │                                     │
│                               ▼                                     │
│                       EventDispatcher                               │
│                        /          \                                 │
│                       /            \                                │
│            im.message.receive_v1   card.action.trigger             │
│                      │                  │                          │
│                      ▼                  ▼                          │
│               handleMessage()    handleCardAction()                │
│                      │                  │                          │
│                      ▼                  │                          │
│              commands/index.js         │                          │
│                 (命令路由)              │                          │
│                      │                 │                          │
│        ┌─────────────┼─────────────┐   │                          │
│        ▼             ▼             ▼   │                          │
│   stock.js    analysis.js    chart.js │                          │
│        │             │             │   │                          │
│        └─────────────┴─────────────┘   │                          │
│                      │                 │                          │
│                      ▼                 │                          │
│              formatters/*              │                          │
│                      │                 │                          │
│                      ▼                 ▼                          │
│              messageService ◀────── client.js                    │
│                      │                                             │
│                      ▼                                             │
│              飞书 API (发送消息)                                    │
│                                                                     │
│   ─────────────────────────────────────────────────────────────    │
│   支撑层: database.js | logger.js | chartGenerator.js | config.js  │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

### 目录结构

```
src/
├── index.js              # 入口 + 事件注册
├── config.js             # 配置（保持不变）
├── client.js             # 新增：飞书 Client 单例
├── services/
│   └── messageService.js # 新增：消息发送服务
├── commands/
│   ├── index.js          # 新增：命令路由器
│   ├── stock.js          # 新增：股票命令
│   ├── analysis.js       # 新增：分析命令
│   ├── chart.js          # 新增：图表命令
│   ├── help.js           # 新增：帮助命令
│   └── progress.js       # 新增：进度命令
├── formatters/
│   ├── stockFormatter.js # 新增
│   ├── analysisFormatter.js # 新增
│   └── progressFormatter.js # 新增
├── handler.js            # 简化：只做事件分发
├── reply.js              # 废弃 → 拆分到 commands/
├── messageBuilder.js     # 保持
├── database.js           # 保持
├── chartGenerator.js     # 保持
├── logger.js             # 保持
└── utils/
    └── encryptor.js      # 保持
```

### 模块职责

| 模块 | 职责 | 状态 |
|------|------|------|
| `index.js` | 入口 + WebSocket 启动 + 事件注册 | 修改 |
| `client.js` | 飞书 Client 单例 | 新增 |
| `services/messageService.js` | 消息发送封装 | 新增 |
| `commands/index.js` | 命令路由 + 注册机制 | 新增 |
| `commands/stock.js` | 股票命令处理 | 新增 |
| `commands/analysis.js` | 分析命令处理 | 新增 |
| `commands/chart.js` | 图表命令处理 | 新增 |
| `formatters/stockFormatter.js` | 股票数据格式化 | 新增 |
| `formatters/analysisFormatter.js` | 分析数据格式化 | 新增 |
| `handler.js` | 事件入口分发 | 简化 |
| `reply.js` | 废弃，拆分到 commands | 删除 |

## 核心设计

### 1. 命令扩展机制

```javascript
// commands/index.js - 命令路由器

const commands = new Map();

function register(prefix, handler) {
  commands.set(prefix, handler);
}

function route(text) {
  for (const [prefix, handler] of commands) {
    if (text === prefix || text.startsWith(prefix + ' ')) {
      return handler(text);
    }
  }
  return null;
}

module.exports = { register, route };
```

扩展新命令只需 2 步：
1. 创建 `commands/newFeature.js`
2. 无需修改其他文件，自动加载

### 2. 消息发送服务

```javascript
// services/messageService.js

class MessageService {
  constructor(client) {
    this.client = client;
  }

  async sendText(receiveId, text, idType = 'chat_id') { ... }
  async sendCard(receiveId, card, idType = 'chat_id') { ... }
  async sendImage(receiveId, imageBuffer, idType = 'chat_id') { ... }
  async reply(messageId, card) { ... }
  async uploadImage(imageBuffer) { ... }
}
```

使用对比：

```javascript
// 重构前
await config.client.im.message.create({
  params: { receive_id_type: 'chat_id' },
  data: {
    receive_id: chatId,
    msg_type: 'interactive',
    content: JSON.stringify(replyData.card),
  },
});

// 重构后
await messageService.sendCard(chatId, replyData.card);
```

## 实施计划

### 阶段划分

```
阶段 1: 基础设施 (P0)
├── 1.1 创建 client.js
├── 1.2 创建 services/messageService.js
└── 1.3 修复卡片回调注册
    ↓ 验证点: 机器人正常运行，按钮可点击

阶段 2: 命令系统 (P1)
├── 2.1 创建 commands/index.js
├── 2.2 创建 commands/stock.js
├── 2.3 创建 commands/analysis.js
└── 2.4 创建 commands/chart.js
    ↓ 验证点: 所有命令功能正常

阶段 3: 格式化器 (P2)
├── 3.1 创建 formatters/stockFormatter.js
├── 3.2 创建 formatters/analysisFormatter.js
└── 3.3 创建 formatters/progressFormatter.js
    ↓ 验证点: 输出格式与原来一致

阶段 4: 清理 (P3)
├── 4.1 简化 handler.js
├── 4.2 删除 reply.js
└── 4.3 更新 index.js 使用新模块
    ↓ 验证点: 全量测试通过
```

### 文件变更汇总

| 文件 | 操作 | 阶段 |
|------|------|------|
| `client.js` | 新增 | 1 |
| `services/messageService.js` | 新增 | 1 |
| `index.js` | 修改 | 1 |
| `commands/index.js` | 新增 | 2 |
| `commands/stock.js` | 新增 | 2 |
| `commands/analysis.js` | 新增 | 2 |
| `commands/chart.js` | 新增 | 2 |
| `commands/help.js` | 新增 | 2 |
| `commands/progress.js` | 新增 | 2 |
| `formatters/stockFormatter.js` | 新增 | 3 |
| `formatters/analysisFormatter.js` | 新增 | 3 |
| `formatters/progressFormatter.js` | 新增 | 3 |
| `handler.js` | 简化 | 4 |
| `reply.js` | 删除 | 4 |

## 风险与缓解

| 风险 | 缓解措施 |
|------|----------|
| 重构过程中功能中断 | 渐进式实施，每阶段验证 |
| 新模块引入 bug | 保留旧代码直到新代码验证通过 |
| 命令路由冲突 | 精确匹配优先，前缀匹配次之 |

## 验收标准

- [ ] 卡片按钮点击可正常响应
- [ ] 所有现有命令功能正常
- [ ] 新增命令只需创建单文件
- [ ] 消息发送代码无重复
- [ ] 每个模块职责单一
- [ ] 测试通过