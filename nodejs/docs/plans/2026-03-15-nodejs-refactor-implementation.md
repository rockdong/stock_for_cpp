# Node.js 飞书机器人重构实施计划

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** 重构 Node.js 飞书机器人，增加命令扩展机制，消除代码重复，修复卡片回调

**Architecture:** 轻量级分层架构，commands/ 处理命令路由，formatters/ 处理格式化，services/ 封装消息发送

**Tech Stack:** Node.js, @larksuiteoapi/node-sdk, better-sqlite3, winston

---

## Task 1: 创建飞书 Client 单例

**Files:**
- Create: `nodejs/src/client.js`

**Step 1: 创建 client.js**

```javascript
const lark = require('@larksuiteoapi/node-sdk');
const config = require('./config');

const client = new lark.Client({
  appId: config.appId,
  appSecret: config.appSecret,
  appType: lark.AppType.SelfBuild,
  domain: lark.Domain.Feishu,
});

module.exports = client;
```

**Step 2: 验证语法**

Run: `node -c nodejs/src/client.js`
Expected: 无输出表示语法正确

**Step 3: 提交**

```bash
git add nodejs/src/client.js
git commit -m "feat(nodejs): add lark client singleton"
```

---

## Task 2: 创建消息发送服务

**Files:**
- Create: `nodejs/src/services/messageService.js`

**Step 1: 创建 services 目录**

```bash
mkdir -p nodejs/src/services
```

**Step 2: 创建 messageService.js**

```javascript
const client = require('../client');
const logger = require('../logger');

class MessageService {
  constructor(clientInstance) {
    this.client = clientInstance || client;
  }

  async sendText(receiveId, text, idType = 'chat_id') {
    try {
      await this.client.im.message.create({
        params: { receive_id_type: idType },
        data: {
          receive_id: receiveId,
          msg_type: 'text',
          content: JSON.stringify({ text }),
        },
      });
      logger.info('文本消息发送成功');
      return { success: true };
    } catch (error) {
      logger.error('发送文本失败: ' + error.message);
      return { success: false, error };
    }
  }

  async sendCard(receiveId, card, idType = 'chat_id') {
    try {
      await this.client.im.message.create({
        params: { receive_id_type: idType },
        data: {
          receive_id: receiveId,
          msg_type: 'interactive',
          content: JSON.stringify(card),
        },
      });
      logger.info('卡片消息发送成功');
      return { success: true };
    } catch (error) {
      logger.error('发送卡片失败: ' + error.message);
      return { success: false, error };
    }
  }

  async uploadImage(imageBuffer) {
    try {
      const response = await this.client.im.image.create({
        data: {
          image_type: 'message',
          image: imageBuffer,
        },
      });
      return response.data.image_key;
    } catch (error) {
      logger.error('上传图片失败: ' + error.message);
      throw error;
    }
  }

  async sendImage(receiveId, imageBuffer, idType = 'chat_id') {
    try {
      const imageKey = await this.uploadImage(imageBuffer);
      await this.client.im.message.create({
        params: { receive_id_type: idType },
        data: {
          receive_id: receiveId,
          msg_type: 'image',
          content: JSON.stringify({ image_key: imageKey }),
        },
      });
      logger.info('图片消息发送成功');
      return { success: true };
    } catch (error) {
      logger.error('发送图片失败: ' + error.message);
      return { success: false, error };
    }
  }
}

const messageService = new MessageService();

module.exports = messageService;
module.exports.MessageService = MessageService;
```

**Step 3: 验证语法**

Run: `node -c nodejs/src/services/messageService.js`
Expected: 无输出表示语法正确

**Step 4: 提交**

```bash
git add nodejs/src/services/
git commit -m "feat(nodejs): add message service layer"
```

---

## Task 3: 修复卡片回调注册

**Files:**
- Modify: `nodejs/src/index.js`

**Step 1: 添加卡片事件注册**

修改 `nodejs/src/index.js`，在 EventDispatcher 注册中添加卡片事件：

```javascript
const eventDispatcher = new lark.EventDispatcher({}).register({
  'im.message.receive_v1': async (data) => {
    logger.debug('收到消息: ' + JSON.stringify(data));
    const message = data.message;
    if (message && message.message_type === 'text') {
      await handleMessage(data);
    }
  },
  'card.action.trigger': async (data) => {
    logger.debug('收到卡片回调: ' + JSON.stringify(data));
    const { handleCardAction } = require('./handler');
    await handleCardAction(data);
  }
});
```

**Step 2: 验证语法**

Run: `node -c nodejs/src/index.js`
Expected: 无输出表示语法正确

**Step 3: 提交**

```bash
git add nodejs/src/index.js
git commit -m "fix(nodejs): register card.action.trigger event handler"
```

---

## Task 4: 创建命令路由器

**Files:**
- Create: `nodejs/src/commands/index.js`

**Step 1: 创建 commands 目录**

```bash
mkdir -p nodejs/src/commands
```

**Step 2: 创建命令路由器**

```javascript
const logger = require('../logger');

const commands = new Map();
const commandsList = [];

function register(prefix, handler, description = '') {
  commands.set(prefix, handler);
  commandsList.push({ prefix, description });
  logger.debug(`注册命令: ${prefix}`);
}

function route(text) {
  const trimmedText = text.trim();
  
  for (const [prefix, handler] of commands) {
    if (trimmedText === prefix || trimmedText.startsWith(prefix + ' ')) {
      logger.debug(`匹配命令: ${prefix}`);
      return handler(trimmedText);
    }
  }
  
  return null;
}

function getCommandsList() {
  return commandsList;
}

module.exports = {
  register,
  route,
  getCommandsList,
};
```

**Step 3: 验证语法**

Run: `node -c nodejs/src/commands/index.js`
Expected: 无输出表示语法正确

**Step 4: 提交**

```bash
git add nodejs/src/commands/
git commit -m "feat(nodejs): add command router"
```

---

## Task 5: 创建股票命令模块

**Files:**
- Create: `nodejs/src/commands/stock.js`

**Step 1: 创建 stock.js**

```javascript
const { searchStocks, findAllStocks, findStocksByIndustry } = require('../database');
const { formatStockList, buildPromptCard } = require('../formatters/stockFormatter');
const { register } = require('./index');

function handleStock(text) {
  if (text === '股票列表') {
    const stocks = findAllStocks(20);
    return formatStockList(stocks);
  }
  
  if (text.startsWith('股票 ')) {
    const keyword = text.substring(3).trim();
    const stocks = searchStocks(keyword, 10);
    return formatStockList(stocks);
  }
  
  if (text.startsWith('行业 ')) {
    const industry = text.substring(3).trim();
    const stocks = findStocksByIndustry(industry, 20);
    return formatStockList(stocks);
  }
  
  return buildPromptCard('📈 股票查询', '请输入股票代码或名称：\n• 股票 000001\n• 股票 平安银行\n• 股票列表\n• 行业 银行');
}

register('股票', handleStock, '股票查询');
register('股票列表', handleStock, '查看股票列表');
register('行业', handleStock, '按行业筛选');

module.exports = { handleStock };
```

**Step 2: 验证语法**

Run: `node -c nodejs/src/commands/stock.js`
Expected: 无输出表示语法正确

**Step 3: 提交**

```bash
git add nodejs/src/commands/stock.js
git commit -m "feat(nodejs): add stock command module"
```

---

## Task 6: 创建股票格式化器

**Files:**
- Create: `nodejs/src/formatters/stockFormatter.js`

**Step 1: 创建 formatters 目录**

```bash
mkdir -p nodejs/src/formatters
```

**Step 2: 创建 stockFormatter.js**

```javascript
const { buildRichTextCard } = require('../messageBuilder');

function formatStockList(stocks) {
  if (!stocks || stocks.length === 0) {
    return buildRichTextCard('🔍 股票查询结果', [
      { type: 'div', content: '❌ 未找到相关股票信息' }
    ], 'grey');
  }

  const sections = [
    { type: 'div', content: `📊 共找到 **${stocks.length}** 只股票` },
    { type: 'hr' }
  ];

  stocks.forEach(s => {
    sections.push({
      type: 'div',
      content: `🏦 **${s.name}**\n\`${s.ts_code}\` | ${s.industry || '-'} | ${s.market || '-'} | ${s.exchange || '-'}`
    });
  });

  return buildRichTextCard('🔍 股票查询结果', sections, 'blue');
}

function buildPromptCard(title, examples) {
  return buildRichTextCard(title, [
    { type: 'div', content: examples }
  ], 'grey');
}

module.exports = {
  formatStockList,
  buildPromptCard,
};
```

**Step 3: 验证语法**

Run: `node -c nodejs/src/formatters/stockFormatter.js`
Expected: 无输出表示语法正确

**Step 4: 提交**

```bash
git add nodejs/src/formatters/
git commit -m "feat(nodejs): add stock formatter"
```

---

## Task 7: 创建分析命令模块

**Files:**
- Create: `nodejs/src/commands/analysis.js`

**Step 1: 创建 analysis.js**

```javascript
const { findAnalysisResults, findAllAnalysisResults, findLatestAnalysisResults } = require('../database');
const { formatAnalysisList, formatLatestAnalysis, buildAnalysisPrompt } = require('../formatters/analysisFormatter');
const { register } = require('./index');

function handleAnalysis(text) {
  if (text === '分析列表') {
    const results = findAllAnalysisResults(20);
    return formatAnalysisList(results);
  }
  
  if (text === '分析结果') {
    const results = findLatestAnalysisResults();
    return formatLatestAnalysis(results);
  }
  
  if (text.startsWith('分析 ')) {
    const keyword = text.substring(3).trim();
    const results = findAnalysisResults(keyword, null, 20);
    return formatAnalysisList(results);
  }
  
  return buildAnalysisPrompt();
}

register('分析', handleAnalysis, '分析结果查询');
register('分析列表', handleAnalysis, '查看分析列表');
register('分析结果', handleAnalysis, '查看最新分析');

module.exports = { handleAnalysis };
```

**Step 2: 验证语法**

Run: `node -c nodejs/src/commands/analysis.js`
Expected: 无输出表示语法正确

**Step 3: 提交**

```bash
git add nodejs/src/commands/analysis.js
git commit -m "feat(nodejs): add analysis command module"
```

---

## Task 8: 创建分析格式化器

**Files:**
- Create: `nodejs/src/formatters/analysisFormatter.js`

**Step 1: 创建 analysisFormatter.js**

```javascript
const { buildRichTextCard } = require('../messageBuilder');

function formatAnalysisList(results) {
  if (!results || results.length === 0) {
    return buildRichTextCard('📈 分析结果', [
      { type: 'div', content: '❌ 未找到分析结果' }
    ], 'grey');
  }

  const labelText = { 'BUY': '🟢 买入', 'SELL': '🔴 卖出', 'HOLD': '⚪ 持有' };

  const sections = [
    { type: 'div', content: `📊 共 ${results.length} 条分析结果` },
    { type: 'hr' }
  ];

  results.forEach(r => {
    const label = labelText[r.label] || r.label;
    sections.push({
      type: 'div',
      content: `${label} **${r.name || r.ts_code}**\n策略: ${r.strategy_name} | 日期: ${r.trade_date}`
    });
  });

  return buildRichTextCard('📈 分析结果', sections, 'blue');
}

function formatLatestAnalysis(results) {
  if (!results || results.length === 0) {
    return buildRichTextCard('📊 最新分析结果', [
      { type: 'div', content: '❌ 未找到分析结果' }
    ], 'grey');
  }

  const groups = {};
  results.forEach(r => {
    const key = r.strategy_name;
    if (!groups[key]) groups[key] = [];
    groups[key].push(r);
  });

  const elements = [];
  Object.entries(groups).forEach(([strategy, items]) => {
    const tradeDate = items[0].trade_date;
    elements.push({
      tag: 'div',
      text: { tag: 'lark_md', content: `📊 **${strategy}** (${tradeDate}, ${items.length}条)` }
    });
    
    items.forEach(r => {
      const shortCode = r.ts_code.split('.')[0];
      elements.push({
        tag: 'div',
        text: { tag: 'lark_md', content: `• ${shortCode} | ${r.name || '-'} | opt: ${r.opt || '-'}` }
      });
      
      const buttons = buildChartButtons(r.ts_code, r.opt);
      if (buttons.length > 0) {
        elements.push({
          tag: 'action',
          actions: buttons
        });
      }
    });
    
    elements.push({ tag: 'hr' });
  });

  elements.pop();
  
  elements.push({ tag: 'hr' });
  elements.push({
    tag: 'div',
    text: { tag: 'lark_md', content: '💡 发送「图表 代码 d/w/m」查看对应周期图表' }
  });
  
  return {
    msg_type: 'interactive',
    card: {
      config: { wide_screen_mode: true },
      header: {
        title: { tag: 'plain_text', content: '📊 最新分析结果' },
        template: 'blue'
      },
      elements: elements
    }
  };
}

function buildChartButtons(tsCode, opt) {
  const buttons = [];
  const optStr = opt || '';
  
  if (optStr.includes('☀️')) {
    buttons.push({
      tag: 'button',
      text: { tag: 'plain_text', content: '日线' },
      type: 'primary',
      action_id: 'chart_d',
      value: JSON.stringify({ action: 'chart', ts_code: tsCode, freq: 'd' })
    });
  }
  if (optStr.includes('⭐')) {
    buttons.push({
      tag: 'button',
      text: { tag: 'plain_text', content: '周线' },
      type: 'default',
      action_id: 'chart_w',
      value: JSON.stringify({ action: 'chart', ts_code: tsCode, freq: 'w' })
    });
  }
  if (optStr.includes('🌙')) {
    buttons.push({
      tag: 'button',
      text: { tag: 'plain_text', content: '月线' },
      type: 'default',
      action_id: 'chart_m',
      value: JSON.stringify({ action: 'chart', ts_code: tsCode, freq: 'm' })
    });
  }
  
  return buttons;
}

function buildAnalysisPrompt() {
  return buildRichTextCard('📊 分析结果查询', [
    { type: 'div', content: '请输入股票代码：\n• 分析 000001\n• 分析列表' }
  ], 'grey');
}

module.exports = {
  formatAnalysisList,
  formatLatestAnalysis,
  buildChartButtons,
  buildAnalysisPrompt,
};
```

**Step 2: 验证语法**

Run: `node -c nodejs/src/formatters/analysisFormatter.js`
Expected: 无输出表示语法正确

**Step 3: 提交**

```bash
git add nodejs/src/formatters/analysisFormatter.js
git commit -m "feat(nodejs): add analysis formatter"
```

---

## Task 9: 创建图表命令模块

**Files:**
- Create: `nodejs/src/commands/chart.js`

**Step 1: 创建 chart.js**

```javascript
const { getChartData } = require('../database');
const { generateChart } = require('../chartGenerator');
const { buildRichTextCard } = require('../messageBuilder');
const { register } = require('./index');
const logger = require('../logger');

function handleChart(text) {
  if (text === '图表') {
    return buildRichTextCard('📊 K线图查询', [
      { type: 'div', content: '请输入股票代码：\n• 图表 000001.SZ (日线)\n• 图表 000001.SZ w (周线)\n• 图表 000001.SZ m (月线)\n\n说明：\n日线-最近10个交易日\n周线-最近10周\n月线-最近10个月' }
    ], 'grey');
  }
  
  if (text.startsWith('图表 ')) {
    const parts = text.substring(3).trim().split(/\s+/);
    const tsCode = parts[0];
    const freq = parts[1] || 'd';
    
    if (!tsCode) {
      return buildRichTextCard('📊 图表查询', [
        { type: 'div', content: '请输入股票代码：\n• 图表 000001.SZ (日线)\n• 图表 000001.SZ w (周线)\n• 图表 000001.SZ m (月线)' }
      ], 'grey');
    }
    
    const normalizedCode = tsCode.includes('.') ? tsCode : tsCode + '.SZ';
    const validFreqs = ['d', 'w', 'm'];
    const normalizedFreq = validFreqs.includes(freq.toLowerCase()) ? freq.toLowerCase() : 'd';
    
    const chartData = getChartData(normalizedCode, normalizedFreq);
    
    if (!chartData) {
      return buildRichTextCard('❌ 图表查询失败', [
        { type: 'div', content: `未找到 **${normalizedCode}** 的图表数据` },
        { type: 'hr' },
        { type: 'div', content: '可能原因：\n1. 该股票尚未分析\n2. 股票代码不正确\n3. 数据库中没有对应频率的数据' }
      ], 'red');
    }
    
    try {
      const imageBuffer = generateChart(chartData);
      return {
        type: 'image',
        buffer: imageBuffer,
        text: `📊 ${normalizedCode} ${normalizedFreq === 'd' ? '日线' : normalizedFreq === 'w' ? '周线' : '月线'}图`
      };
    } catch (error) {
      logger.error('生成图表失败: ' + error.message);
      return buildRichTextCard('❌ 图表生成失败', [
        { type: 'div', content: `错误: ${error.message}` }
      ], 'red');
    }
  }
  
  return null;
}

register('图表', handleChart, 'K线图查询');

module.exports = { handleChart };
```

**Step 2: 验证语法**

Run: `node -c nodejs/src/commands/chart.js`
Expected: 无输出表示语法正确

**Step 3: 提交**

```bash
git add nodejs/src/commands/chart.js
git commit -m "feat(nodejs): add chart command module"
```

---

## Task 10: 创建进度命令模块

**Files:**
- Create: `nodejs/src/commands/progress.js`
- Create: `nodejs/src/formatters/progressFormatter.js`

**Step 1: 创建 progressFormatter.js**

```javascript
const { buildRichTextCard } = require('../messageBuilder');

function formatProgressCard(progress) {
  if (!progress) {
    return {
      msg_type: 'interactive',
      card: {
        config: { wide_screen_mode: true },
        header: {
          title: { tag: 'plain_text', content: '📊 分析进度' },
          template: 'grey'
        },
        elements: [
          { tag: 'div', text: { tag: 'lark_md', content: '❌ 无法获取分析进度' } }
        ]
      }
    };
  }

  const statusText = { 'idle': '⚪ 空闲', 'running': '🔄 运行中', 'completed': '✅ 已完成' };
  const status = statusText[progress.status] || progress.status;
  const total = progress.total || 0;
  const completed = progress.completed || 0;
  const failed = progress.failed || 0;
  const percent = total > 0 ? Math.round((completed / total) * 100) : 0;

  let elapsed = '';
  if (progress.started_at) {
    const start = new Date(progress.started_at);
    const now = progress.status === 'running' ? new Date() : new Date(progress.updated_at);
    const diffMs = now - start;
    const minutes = Math.floor(diffMs / 60000);
    const seconds = Math.floor((diffMs % 60000) / 1000);
    elapsed = `${minutes}分${seconds}秒`;
  }

  const progressBar = '█'.repeat(Math.round(percent / 5)) + '░'.repeat(20 - Math.round(percent / 5));

  const content = `**状态:** ${status}\n\n` +
    `**进度:** ${completed} / ${total} (${percent}%)\n` +
    `\`${progressBar}\` ${percent}%\n\n` +
    `✅ 成功: ${completed - failed}  |  ❌ 失败: ${failed}` +
    (progress.started_at ? `\n\n⏱️ 已运行: ${elapsed}` : '');

  return {
    msg_type: 'interactive',
    card: {
      config: { wide_screen_mode: true },
      header: {
        title: { tag: 'plain_text', content: '📊 分析进度' },
        template: progress.status === 'running' ? 'green' : 'blue'
      },
      elements: [
        { tag: 'div', text: { tag: 'lark_md', content: content } },
        { tag: 'hr' },
        {
          tag: 'action',
          actions: [
            {
              tag: 'button',
              text: { tag: 'plain_text', content: '🔄 刷新' },
              type: 'primary',
              action_id: 'refresh_progress',
              value: JSON.stringify({ action: 'refresh' })
            }
          ]
        }
      ]
    }
  };
}

module.exports = {
  formatProgressCard,
};
```

**Step 2: 创建 progress.js**

```javascript
const { getAnalysisProgress } = require('../database');
const { formatProgressCard } = require('../formatters/progressFormatter');
const { register } = require('./index');

function handleProgress(text) {
  const progress = getAnalysisProgress();
  return formatProgressCard(progress);
}

register('分析进度', handleProgress, '查看分析进度');

module.exports = { handleProgress };
```

**Step 3: 验证语法**

Run: `node -c nodejs/src/formatters/progressFormatter.js && node -c nodejs/src/commands/progress.js`
Expected: 无输出表示语法正确

**Step 4: 提交**

```bash
git add nodejs/src/formatters/progressFormatter.js nodejs/src/commands/progress.js
git commit -m "feat(nodejs): add progress command and formatter"
```

---

## Task 11: 创建帮助命令模块

**Files:**
- Create: `nodejs/src/commands/help.js`

**Step 1: 创建 help.js**

```javascript
const { buildRichTextCard } = require('../messageBuilder');
const { getCommandsList } = require('./index');
const { register } = require('./index');

function buildHelpCard() {
  return buildRichTextCard('📖 股票分析机器人 - 帮助指南', [
    { type: 'div', content: '**🔍 查询功能**\n• 股票 <代码/名称> - 查询股票\n• 股票列表 - 查看所有股票\n• 行业 <行业名> - 按行业筛选' },
    { type: 'hr' },
    { type: 'div', content: '**📊 分析功能**\n• 分析 <代码> - 查询分析结果\n• 分析列表 - 查看所有结果\n• 分析结果 - 查看最新结果\n• 分析进度 - 查看运行状态' },
    { type: 'hr' },
    { type: 'div', content: '**📈 图表功能**\n• 图表 <代码> [周期] - 生成K线\n\n**周期:** d=日线, w=周线, m=月线' }
  ], 'blue');
}

function handleHelp(text) {
  if (text === 'hello' || text === '你好') {
    return buildRichTextCard('👋 欢迎', [
      { type: 'div', content: '你好！我是股票分析机器人，已连接成功！' }
    ], 'green');
  }
  return buildHelpCard();
}

register('帮助', handleHelp, '显示帮助信息');
register('hello', handleHelp, '测试连接');
register('你好', handleHelp, '测试连接');

module.exports = { handleHelp, buildHelpCard };
```

**Step 2: 验证语法**

Run: `node -c nodejs/src/commands/help.js`
Expected: 无输出表示语法正确

**Step 3: 提交**

```bash
git add nodejs/src/commands/help.js
git commit -m "feat(nodejs): add help command module"
```

---

## Task 12: 简化 handler.js

**Files:**
- Modify: `nodejs/src/handler.js`

**Step 1: 重写 handler.js**

```javascript
const messageService = require('./services/messageService');
const { route } = require('./commands');
const { buildRichTextCard } = require('./messageBuilder');
const { generateChart } = require('./chartGenerator');
const { getChartData } = require('./database');
const logger = require('./logger');

async function handleCardAction(event) {
  const action = event.action;
  const value = JSON.parse(action.value || '{}');
  const openId = event.operator?.open_id;
  
  logger.debug('卡片按钮点击: ' + JSON.stringify(value));
  
  if (value.action === 'chart') {
    const tsCode = value.ts_code;
    const freq = value.freq;
    const validFreqs = ['d', 'w', 'm'];
    const normalizedFreq = validFreqs.includes(freq?.toLowerCase()) ? freq.toLowerCase() : 'd';
    
    const chartData = getChartData(tsCode, normalizedFreq);
    
    if (chartData) {
      try {
        const imageBuffer = generateChart(chartData);
        await messageService.sendImage(openId, imageBuffer, 'open_id');
        logger.info('图表图片发送成功');
      } catch (error) {
        logger.error('发送图表失败: ' + error.message);
      }
    }
  } else if (value.action === 'refresh') {
    const { formatProgressCard } = require('./formatters/progressFormatter');
    const { getAnalysisProgress } = require('./database');
    const progress = getAnalysisProgress();
    const progressCard = formatProgressCard(progress);
    
    await messageService.sendCard(openId, progressCard.card, 'open_id');
    logger.info('进度刷新消息发送成功');
  }
}

async function handleMessage(event) {
  const message = event.message;
  const chatId = message.chat_id;
  const messageType = message.message_type;
  
  if (messageType !== 'text') {
    logger.debug('忽略非文本消息');
    return;
  }
  
  const textContent = JSON.parse(message.content).text;
  const senderId = message.sender_id;
  
  logger.info(`收到消息 from ${senderId?.open_id}: ${textContent}`);
  
  const replyData = route(textContent);
  
  if (!replyData) {
    const defaultReply = buildRichTextCard('🤖 收到消息', [
      { type: 'div', content: `收到: ${textContent}` },
      { type: 'hr' },
      { type: 'div', content: '输入"帮助"查看可用命令' }
    ], 'grey');
    await messageService.sendCard(chatId, defaultReply.card);
    return;
  }
  
  if (replyData.msg_type === 'interactive') {
    await messageService.sendCard(chatId, replyData.card);
  } else if (replyData.type === 'image' && replyData.buffer) {
    await messageService.sendImage(chatId, replyData.buffer);
  } else if (Array.isArray(replyData)) {
    for (const msg of replyData) {
      if (msg && msg.type === 'image' && msg.buffer) {
        await messageService.sendImage(chatId, msg.buffer);
      } else {
        const replyText = typeof msg === 'string' ? msg : msg.text || '无法处理';
        await messageService.sendText(chatId, replyText);
      }
    }
  } else {
    const replyText = typeof replyData === 'string' ? replyData : replyData.text || '无法处理';
    await messageService.sendText(chatId, replyText);
  }
}

module.exports = { handleMessage, handleCardAction };
```

**Step 2: 验证语法**

Run: `node -c nodejs/src/handler.js`
Expected: 无输出表示语法正确

**Step 3: 提交**

```bash
git add nodejs/src/handler.js
git commit -m "refactor(nodejs): simplify handler using message service and command router"
```

---

## Task 13: 更新 config.js 移除 client

**Files:**
- Modify: `nodejs/src/config.js`

**Step 1: 移除 client 创建**

修改 `nodejs/src/config.js`，删除 client 相关代码：

```javascript
const dotenv = require('dotenv');
const fs = require('fs');
const path = require('path');
const Encryptor = require('./utils/encryptor');
const logger = require('./logger');

function loadConfig() {
  dotenv.config();
  
  const keyFile = path.join(process.cwd(), 'secret.key');
  if (fs.existsSync(keyFile)) {
    const secretKey = fs.readFileSync(keyFile, 'utf8').trim();
    
    if (secretKey) {
      const envPath = path.join(process.cwd(), '.env');
      if (fs.existsSync(envPath)) {
        const envContent = fs.readFileSync(envPath, 'utf8');
        const lines = envContent.split('\n');
        
        for (const line of lines) {
          const trimmedLine = line.trim();
          if (trimmedLine.startsWith('#') && trimmedLine.includes('_ENC=')) {
            const match = trimmedLine.match(/# (\w+)_ENC=(.+)/);
            if (match) {
              const varName = match[1];
              const encryptedValue = match[2];
              try {
                const decryptedValue = Encryptor.decrypt(encryptedValue, secretKey);
                process.env[varName] = decryptedValue;
              } catch (error) {
                logger.error(`Config: 解密 ${varName} 失败: ` + error.message);
              }
            }
          }
        }
      }
    }
  }
}

loadConfig();

const config = {
  appId: process.env.APP_ID,
  appSecret: process.env.APP_SECRET,
  encryptKey: process.env.ENCRYPT_KEY,
  verificationToken: process.env.VERIFICATION_TOKEN,
  port: parseInt(process.env.PORT) || 3000,
  dbPath: process.env.DB_PATH || '../stock.db',
};

module.exports = config;
```

**Step 2: 验证语法**

Run: `node -c nodejs/src/config.js`
Expected: 无输出表示语法正确

**Step 3: 提交**

```bash
git add nodejs/src/config.js
git commit -m "refactor(nodejs): remove client from config, use client.js singleton"
```

---

## Task 14: 删除 reply.js

**Files:**
- Delete: `nodejs/src/reply.js`

**Step 1: 删除文件**

```bash
rm nodejs/src/reply.js
```

**Step 2: 验证所有模块可加载**

Run: `node -e "require('./nodejs/src/index.js')"`
Expected: 程序启动或输出初始化日志（无报错）

**Step 3: 提交**

```bash
git add -A
git commit -m "refactor(nodejs): remove reply.js, functionality moved to commands/"
```

---

## Task 15: 最终验证

**Step 1: 启动机器人测试**

Run: `cd nodejs && npm start`
Expected: 
- 看到 "启动飞书机器人（WebSocket 长连接模式）..."
- 看到 "飞书机器人服务已启动"

**Step 2: 发送测试消息**

在飞书中发送：
- "帮助" → 应返回帮助卡片
- "股票列表" → 应返回股票列表
- "分析结果" → 应返回最新分析结果
- 点击卡片按钮 → 应返回图表

**Step 3: 提交最终版本**

```bash
git add -A
git commit -m "refactor(nodejs): complete refactoring with command system"
```