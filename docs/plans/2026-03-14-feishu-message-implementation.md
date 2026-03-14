# 飞书富文本消息实现计划

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** 优化飞书股票机器人消息回复，使用富文本卡片、交互卡片替代纯文本输出

**Architecture:** 
- 新建 messageBuilder.js 消息构建器，统一处理所有消息类型的生成
- 重构 reply.js 调用消息构建器，根据场景自动选择合适的消息类型
- 修改 handler.js 支持交互卡片的按钮回调处理

**Tech Stack:** Node.js, 飞书开放平台 API, @larksuiteoapi/node-sdk

---

### Task 1: 创建消息构建器 messageBuilder.js

**Files:**
- Create: `/Users/workspace/Documents/CppProjects/stock_for_cpp/nodejs/src/messageBuilder.js`

**Step 1: 创建基础消息构建器框架**

```javascript
// /Users/workspace/Documents/CppProjects/stock_for_cpp/nodejs/src/messageBuilder.js

/**
 * 飞书富文本消息构建器
 * 支持：富文本卡片、交互卡片、菜单消息
 */

/**
 * 构建富文本卡片消息
 * @param {string} title - 卡片标题
 * @param {Array} sections - 区块数组
 * @returns {object} 消息对象
 */
function buildRichTextCard(title, sections) {
  const elements = sections.map(section => {
    if (section.type === 'div') {
      return {
        tag: 'div',
        text: {
          tag: 'lark_md',
          content: section.content
        }
      };
    }
    if (section.type === 'hr') {
      return { tag: 'hr' };
    }
    return null;
  }).filter(Boolean);

  return {
    msg_type: 'interactive',
    card: {
      config: {
        wide_screen_mode: true
      },
      header: {
        title: {
          tag: 'plain_text',
          content: title
        },
        template: 'blue'
      },
      elements: elements
    }
  };
}

/**
 * 构建交互卡片（带按钮）
 * @param {string} title - 卡片标题
 * @param {string} content - 正文内容
 * @param {Array} buttons - 按钮数组
 * @returns {object} 消息对象
 */
function buildInteractiveCard(title, content, buttons) {
  const buttonElements = buttons.map(btn => ({
    tag: 'button',
    text: {
      tag: 'plain_text',
      content: btn.text
    },
    type: btn.type || 'primary',
    value: JSON.stringify(btn.value || {})
  }));

  return {
    msg_type: 'interactive',
    card: {
      config: {
        wide_screen_mode: true
      },
      header: {
        title: {
          tag: 'plain_text',
          content: title
        },
        template: 'blue'
      },
      elements: [
        {
          tag: 'div',
          text: {
            tag: 'lark_md',
            content: content
          }
        },
        { tag: 'hr' },
        {
          tag: 'action',
          actions: buttonElements
        }
      ]
    }
  };
}

/**
 * 卡片更新响应
 * @param {string} cardId - 卡片ID
 * @param {string} token - 消息token
 * @param {object} newCard - 新的卡片内容
 */
function buildCardUpdate(cardId, token, newCard) {
  return {
    card_id: cardId,
    token: token,
    card: newCard.card
  };
}

module.exports = {
  buildRichTextCard,
  buildInteractiveCard,
  buildCardUpdate
};
```

**Step 2: 验证文件创建**

Run: `ls -la /Users/workspace/Documents/CppProjects/stock_for_cpp/nodejs/src/messageBuilder.js`
Expected: 文件存在

**Step 3: 提交**

```bash
git add nodejs/src/messageBuilder.js
git commit -m "feat: 添加飞书富文本消息构建器"
```

---

### Task 2: 重构 reply.js 使用消息构建器

**Files:**
- Modify: `/Users/workspace/Documents/CppProjects/stock_for_cpp/nodejs/src/reply.js:1-269`

**Step 1: 修改 reply.js 引入消息构建器**

在文件顶部添加：
```javascript
const { buildRichTextCard, buildInteractiveCard } = require('./messageBuilder');
```

**Step 2: 重构 formatStockAsTable 为富文本卡片格式**

原代码 (lines 4-17):
```javascript
function formatStockAsTable(stocks) {
  // ... 原有实现
}
```

替换为:
```javascript
function formatStockAsCard(stocks) {
  if (!stocks || stocks.length === 0) {
    return buildRichTextCard('🔍 股票查询结果', [
      { type: 'div', content: '❌ 未找到相关股票信息' }
    ]);
  }

  const sections = stocks.map(s => ({
    type: 'div',
    content: `🏦 **${s.name}**\n${s.ts_code} | ${s.industry || '-'} | ${s.market || '-'} | ${s.exchange || '-'}`
  }));

  sections.unshift({ type: 'hr' });
  sections.unshift({ 
    type: 'div', 
    content: `📊 共找到 **${stocks.length}** 只股票` 
  });

  return buildRichTextCard('🔍 股票查询结果', sections);
}
```

**Step 3: 重构 formatAnalysisAsTable**

替换为 formatAnalysisAsCard 函数，返回富文本卡片格式。

**Step 4: 重构 formatProgress 为富文本卡片**

替换 formatProgress 函数，返回带按钮的交互卡片。

**Step 5: 重构 getReply 入口函数**

修改各分支调用新的格式化函数：
- 股票查询 → formatStockAsCard
- 分析结果 → formatAnalysisAsCard
- 分析进度 → formatProgress (交互卡片)
- 帮助信息 → buildRichTextCard

**Step 6: 运行测试验证**

Run: `cd /Users/workspace/Documents/CppProjects/stock_for_cpp/nodejs && node -e "const r = require('./src/reply'); console.log(JSON.stringify(r.getReply('股票列表'), null, 2))"`
Expected: 输出 JSON 格式的卡片消息

**Step 7: 提交**

```bash
git add nodejs/src/reply.js
git commit -m "refactor: 重构reply.js使用富文本消息"
```

---

### Task 3: 修改 handler.js 支持卡片消息发送

**Files:**
- Modify: `/Users/workspace/Documents/CppProjects/stock_for_cpp/nodejs/src/handler.js:37-95`

**Step 1: 修改消息发送逻辑**

原代码处理纯文本和图片，增加卡片消息处理：

```javascript
// 在 handleMessage 函数中添加卡片处理
async function handleMessage(event) {
  // ... 解析消息 ...

  const replyData = getReply(textContent);

  try {
    // 处理富文本卡片/交互卡片
    if (replyData && replyData.msg_type === 'interactive') {
      await config.client.im.message.create({
        params: { receive_id_type: 'chat_id' },
        data: {
          receive_id: chatId,
          msg_type: 'interactive',
          content: JSON.stringify(replyData.card),
        },
      });
      console.log('卡片消息发送成功');
      return;
    }

    // 处理图片 (现有逻辑)
    if (replyData && replyData.type === 'image' && replyData.buffer) {
      // ... 现有图片处理 ...
    }

    // 处理纯文本 (现有逻辑)
    // ... 现有文本处理 ...
  } catch (error) {
    console.error('发送失败:', error);
  }
}
```

**Step 2: 验证 handler.js 语法**

Run: `cd /Users/workspace/Documents/CppProjects/stock_for_cpp/nodejs && node -c src/handler.js`
Expected: 无语法错误

**Step 3: 提交**

```bash
git add nodejs/src/handler.js
git commit -m "feat: handler支持富文本卡片消息发送"
```

---

### Task 4: 添加按钮回调处理（可选功能）

**Files:**
- Modify: `/Users/workspace/Documents/CppProjects/stock_for_cpp/nodejs/src/index.js`

**Step 1: 注册卡片回调事件**

```javascript
const eventDispatcher = new lark.EventDispatcher({}).register({
  'im.message.receive_v1': async (data) => {
    // ... 现有消息处理 ...
  },
  'im.message.interactive_callback_v1': async (data) => {
    // 处理按钮点击回调
    const callback = data.callback;
    const action = JSON.parse(callback.action);
    
    if (action.type === 'refresh') {
      // 刷新当前数据
      const message = data.message;
      const originalText = // 从原始消息提取关键词
      const newReply = getReply(originalText);
      // 更新卡片
    }
  }
});
```

**Step 2: 测试回调功能**

需要实际点击按钮验证。

**Step 3: 提交**

---

## 验证步骤

1. 启动飞书机器人: `cd nodejs && node src/index.js`
2. 发送 `帮助` → 应收到富文本卡片
3. 发送 `股票列表` → 应收到股票列表卡片
4. 发送 `分析进度` → 应收到带按钮的交互卡片

---

**Plan complete and saved to `docs/plans/2026-03-14-feishu-message-implementation.md`.**

**Two execution options:**

**1. Subagent-Driven (this session)** - I dispatch fresh subagent per task, review between tasks, fast iteration

**2. Parallel Session (separate)** - Open new session with executing-plans, batch execution with checkpoints

**Which approach?**