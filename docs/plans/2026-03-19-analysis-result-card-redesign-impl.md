# 分析结果卡片重新设计 - 实现计划

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** 重写分析结果格式化函数，按信号类型分组显示，添加统计信息和统一交互按钮

**Architecture:** 修改 analysisFormatter.js 的 formatLatestAnalysis 函数，将结果按 BUY/SELL/HOLD 分组，每组使用独立卡片，统一提供日线/周线/月线按钮

**Tech Stack:** Node.js, 飞书开放平台 API (interactive card)

---

### Task 1: 重写 formatLatestAnalysis 函数 - 数据分组逻辑

**Files:**
- Modify: `/Users/kirito/Documents/projects/stock_for_cpp/nodejs/src/formatters/analysisFormatter.js:28-88`

**Step 1: 添加辅助函数 - 周期图标映射**

在文件顶部添加：

```javascript
const FREQ_ICONS = {
  'd': '☀️ 日线',
  'w': '⭐ 周线',
  'm': '🌙 月线'
};

const FREQ_LABELS = {
  'd': '日线',
  'w': '周线',
  'm': '月线'
};
```

**Step 2: 添加辅助函数 - 构建股票结果元素**

```javascript
function buildStockElement(stock) {
  const shortCode = stock.ts_code.split('.')[0];
  const freqIcon = FREQ_ICONS[stock.freq] || stock.freq || '-';

  const lines = [
    `▫️ **${stock.name || shortCode}** \`${stock.ts_code}\``,
    `策略: ${stock.strategy_name} · ${freqIcon}`
  ];

  return {
    tag: 'div',
    text: { tag: 'lark_md', content: lines.join('\n') }
  };
}
```

**Step 3: 添加辅助函数 - 构建图表按钮**

```javascript
function buildChartButtons(tsCode) {
  return [
    {
      tag: 'button',
      text: { tag: 'plain_text', content: '📊 日线' },
      type: 'primary',
      action_id: 'chart_d',
      value: JSON.stringify({ action: 'chart', ts_code: tsCode, freq: 'd' })
    },
    {
      tag: 'button',
      text: { tag: 'plain_text', content: '📊 周线' },
      type: 'default',
      action_id: 'chart_w',
      value: JSON.stringify({ action: 'chart', ts_code: tsCode, freq: 'w' })
    },
    {
      tag: 'button',
      text: { tag: 'plain_text', content: '📊 月线' },
      type: 'default',
      action_id: 'chart_m',
      value: JSON.stringify({ action: 'chart', ts_code: tsCode, freq: 'm' })
    }
  ];
}
```

**Step 4: 验证语法**

Run: `cd /Users/kirito/Documents/projects/stock_for_cpp/nodejs && node -c src/formatters/analysisFormatter.js`
Expected: 无语法错误

---

### Task 2: 重写 formatLatestAnalysis 函数 - 主体逻辑

**Files:**
- Modify: `/Users/kirito/Documents/projects/stock_for_cpp/nodejs/src/formatters/analysisFormatter.js:28-88`

**Step 1: 重写 formatLatestAnalysis 函数**

替换整个 `formatLatestAnalysis` 函数：

```javascript
function formatLatestAnalysis(results) {
  if (!results || results.length === 0) {
    return buildRichTextCard('📊 最新分析结果', [
      { type: 'div', content: '🔍 暂无分析结果\n\n💡 发送「分析进度」查看运行状态' }
    ], 'grey');
  }

  // 按 label 分组
  const groups = {
    'BUY': [],
    'SELL': [],
    'HOLD': []
  };

  results.forEach(r => {
    const label = (r.label || 'HOLD').toUpperCase();
    if (groups[label]) {
      groups[label].push(r);
    } else {
      groups['HOLD'].push(r);
    }
  });

  const tradeDate = results[0]?.trade_date || '-';
  const total = results.length;
  const buyCount = groups.BUY.length;
  const sellCount = groups.SELL.length;
  const holdCount = groups.HOLD.length;

  // 统计卡片
  const summaryCard = {
    msg_type: 'interactive',
    card: {
      config: { wide_screen_mode: true },
      header: {
        title: { tag: 'plain_text', content: '📊 最新分析结果' },
        template: 'blue'
      },
      elements: [
        {
          tag: 'div',
          text: {
            tag: 'lark_md',
            content: `📅 ${tradeDate} · 📈 共 ${total} 条\n✅ 买入 ${buyCount} · ❌ 卖出 ${sellCount} · ⏸️ 持有 ${holdCount}`
          }
        }
      ]
    }
  };

  // 分组卡片
  const groupCards = [];

  // 买入信号组
  if (groups.BUY.length > 0) {
    groupCards.push(buildGroupCard('🟢 买入信号', groups.BUY, 'green'));
  }

  // 卖出信号组
  if (groups.SELL.length > 0) {
    groupCards.push(buildGroupCard('🔴 卖出信号', groups.SELL, 'red'));
  }

  // 持有信号组
  if (groups.HOLD.length > 0) {
    groupCards.push(buildGroupCard('⚪ 持有', groups.HOLD, 'grey'));
  }

  return {
    summary: summaryCard,
    groups: groupCards
  };
}
```

**Step 2: 添加辅助函数 - 构建分组卡片**

```javascript
function buildGroupCard(title, stocks, template) {
  const elements = [];

  elements.push({
    tag: 'div',
    text: { tag: 'lark_md', content: `共 **${stocks.length}** 只` }
  });
  elements.push({ tag: 'hr' });

  stocks.forEach(stock => {
    elements.push(buildStockElement(stock));
    elements.push({
      tag: 'action',
      actions: buildChartButtons(stock.ts_code)
    });
    elements.push({ tag: 'hr' });
  });

  // 移除最后一个 hr
  elements.pop();

  return {
    msg_type: 'interactive',
    card: {
      config: { wide_screen_mode: true },
      header: {
        title: { tag: 'plain_text', content: `${title} (${stocks.length}只)` },
        template: template
      },
      elements: elements
    }
  };
}
```

**Step 3: 更新 module.exports**

确保导出所有新函数：

```javascript
module.exports = {
  formatAnalysisList,
  formatLatestAnalysis,
  buildChartButtons,
  buildAnalysisPrompt,
  // 新增辅助函数导出（用于测试）
  buildStockElement,
  buildGroupCard,
  FREQ_ICONS,
  FREQ_LABELS
};
```

**Step 4: 验证语法**

Run: `cd /Users/kirito/Documents/projects/stock_for_cpp/nodejs && node -c src/formatters/analysisFormatter.js`
Expected: 无语法错误

---

### Task 3: 更新消息服务支持多卡片发送

**Files:**
- Modify: `/Users/kirito/Documents/projects/stock_for_cpp/nodejs/src/services/messageService.js`

**Step 1: 添加发送多卡片方法**

在 `MessageService` 类中添加：

```javascript
async sendMultipleCards(receiveId, cards, idType = 'chat_id') {
  const results = [];
  for (const card of cards) {
    try {
      await this.client.im.message.create({
        params: { receive_id_type: idType },
        data: {
          receive_id: receiveId,
          msg_type: 'interactive',
          content: JSON.stringify(card.card),
        },
      });
      results.push({ success: true });
    } catch (error) {
      logger.error('发送卡片失败: ' + error.message);
      results.push({ success: false, error });
    }
  }
  logger.info(`发送 ${cards.length} 张卡片完成`);
  return results;
}
```

**Step 2: 更新 module.exports**

确保导出新方法。

**Step 3: 验证语法**

Run: `cd /Users/kirito/Documents/projects/stock_for_cpp/nodejs && node -c src/services/messageService.js`
Expected: 无语法错误

---

### Task 4: 更新 analysis 命令处理器

**Files:**
- Modify: `/Users/kirito/Documents/projects/stock_for_cpp/nodejs/src/commands/analysis.js`

**Step 1: 修改 handleAnalysis 函数**

修改「分析结果」分支以支持多卡片发送：

```javascript
function handleAnalysis(text, sendMultiple) {
  if (text === '分析列表') {
    const results = findAllAnalysisResults(20);
    return formatAnalysisList(results);
  }

  if (text === '分析结果') {
    const results = findLatestAnalysisResults();
    const formatted = formatLatestAnalysis(results);
    // 如果返回的是多卡片格式
    if (formatted.summary && formatted.groups) {
      return { multiple: true, cards: [formatted.summary, ...formatted.groups] };
    }
    return formatted;
  }

  if (text.startsWith('分析 ')) {
    const keyword = text.substring(3).trim();
    const results = findAnalysisResults(keyword, null, 20);
    return formatAnalysisList(results);
  }

  return buildAnalysisPrompt();
}
```

**Step 2: 更新 register 调用**

如果需要，更新注册以支持新的函数签名。

**Step 3: 验证语法**

Run: `cd /Users/kirito/Documents/projects/stock_for_cpp/nodejs && node -c src/commands/analysis.js`
Expected: 无语法错误

---

### Task 5: 更新消息处理器支持多卡片

**Files:**
- Modify: `/Users/kirito/Documents/projects/stock_for_cpp/nodejs/src/handler.js`

**Step 1: 查看当前 handler 实现**

Run: `head -100 /Users/kirito/Documents/projects/stock_for_cpp/nodejs/src/handler.js`

**Step 2: 添加多卡片发送支持**

在消息处理逻辑中添加对 `{ multiple: true, cards: [...] }` 返回值的支持：

```javascript
// 如果返回多卡片
if (replyData && replyData.multiple && replyData.cards) {
  for (const card of replyData.cards) {
    await messageService.sendCard(chatId, card);
  }
  return;
}
```

**Step 3: 验证语法**

Run: `cd /Users/kirito/Documents/projects/stock_for_cpp/nodejs && node -c src/handler.js`
Expected: 无语法错误

---

### Task 6: 提交所有更改

**Step 1: 查看更改**

Run: `git status`

**Step 2: 提交**

```bash
git add nodejs/src/formatters/analysisFormatter.js
git add nodejs/src/services/messageService.js
git add nodejs/src/commands/analysis.js
git add nodejs/src/handler.js
git commit -m "feat: 重构分析结果卡片，按信号类型分组显示

- 添加统计摘要卡片，显示买入/卖出/持有数量
- 按信号类型分组，使用不同颜色主题(绿/红/灰)
- 统一提供日线/周线/月线按钮
- 支持多卡片发送"
```

---

### Task 7: 测试验证

**Step 1: 启动服务**

Run: `cd /Users/kirito/Documents/projects/stock_for_cpp/nodejs && node src/index.js`

**Step 2: 发送测试消息**

在飞书中发送：「分析结果」

**Step 3: 验证结果**

- [ ] 收到统计摘要卡片（蓝色主题）
- [ ] 收到买入信号卡片（绿色主题，如果有）
- [ ] 收到卖出信号卡片（红色主题，如果有）
- [ ] 收到持有信号卡片（灰色主题，如果有）
- [ ] 每只股票都有三个图表按钮
- [ ] 点击按钮能生成对应图表

---

## 验收清单

- [ ] 发送「分析结果」返回分组卡片
- [ ] 买入信号显示绿色 header
- [ ] 卖出信号显示红色 header
- [ ] 持有信号显示灰色 header
- [ ] 每只股票都有日线/周线/月线按钮
- [ ] 点击按钮能生成对应周期图表
- [ ] 空状态显示友好提示
- [ ] 提交信息清晰描述改动