const { searchStocks, findStockByTsCode, findAllStocks, findStocksByIndustry, findAnalysisResults, findAllAnalysisResults, findLatestAnalysisResults, getAnalysisProgress, getChartData } = require('./database');
const { generateChart } = require('./chartGenerator');
const { buildRichTextCard, buildInteractiveCard, buildTextMessage } = require('./messageBuilder');
const logger = require('./logger');

function formatStockAsCard(stocks) {
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

function formatAnalysisAsCard(results) {
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

function formatLatestAnalysisAsCards(results) {
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

function formatProgressCard(progress) {
  if (!progress) {
    return buildRichTextCard('📊 分析进度', [
      { type: 'div', content: '❌ 无法获取分析进度' }
    ], 'grey');
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

  const buttons = progress.status === 'running' ? [
    { text: '🔄 刷新', type: 'primary', action_id: 'refresh_progress', value: { action: 'refresh' } }
  ] : [
    { text: '🔄 刷新', type: 'primary', action_id: 'refresh_progress', value: { action: 'refresh' } }
  ];

  return buildInteractiveCard('📊 分析进度', content, buttons, progress.status === 'running' ? 'green' : 'blue');
}

function getProgressCard() {
  const progress = getAnalysisProgress();
  return formatProgressCard(progress);
}

function buildHelpCard() {
  return buildRichTextCard('📖 股票分析机器人 - 帮助指南', [
    { type: 'div', content: '**🔍 查询功能**\n• 股票 <代码/名称> - 查询股票\n• 股票列表 - 查看所有股票\n• 行业 <行业名> - 按行业筛选' },
    { type: 'hr' },
    { type: 'div', content: '**📊 分析功能**\n• 分析 <代码> - 查询分析结果\n• 分析列表 - 查看所有结果\n• 分析结果 - 查看最新结果\n• 分析进度 - 查看运行状态' },
    { type: 'hr' },
    { type: 'div', content: '**📈 图表功能**\n• 图表 <代码> [周期] - 生成K线\n\n**周期:** d=日线, w=周线, m=月线' }
  ], 'blue');
}

function buildQueryPrompt(title, examples) {
  return buildRichTextCard(title, [
    { type: 'div', content: examples }
  ], 'grey');
}

function getReply(messageText) {
  const text = messageText.trim();

  if (text === '帮助') {
    return buildHelpCard();
  }

  if (text === 'hello' || text === '你好') {
    return buildRichTextCard('👋 欢迎', [
      { type: 'div', content: '你好！我是股票分析机器人，已连接成功！' }
    ], 'green');
  }

  if (text === '股票列表') {
    const stocks = findAllStocks(20);
    return formatStockAsCard(stocks);
  }

  if (text.startsWith('股票 ')) {
    const keyword = text.substring(3).trim();
    const stocks = searchStocks(keyword, 10);
    return formatStockAsCard(stocks);
  }

  if (text.startsWith('行业 ')) {
    const industry = text.substring(3).trim();
    const stocks = findStocksByIndustry(industry, 20);
    return formatStockAsCard(stocks);
  }

  if (text === '分析列表') {
    const results = findAllAnalysisResults(20);
    return formatAnalysisAsCard(results);
  }

  if (text === '分析结果') {
    const results = findLatestAnalysisResults();
    return formatLatestAnalysisAsCards(results);
  }

  if (text === '分析进度') {
    const progress = getAnalysisProgress();
    return formatProgressCard(progress);
  }

  if (text.startsWith('分析 ')) {
    const keyword = text.substring(3).trim();
    const results = findAnalysisResults(keyword, null, 20);
    return formatAnalysisAsCard(results);
  }

  if (text === '股票') {
    return buildQueryPrompt('📈 股票查询', '请输入股票代码或名称：\n• 股票 000001\n• 股票 平安银行\n• 股票列表\n• 行业 银行');
  }

  if (text === '分析') {
    return buildQueryPrompt('📊 分析结果查询', '请输入股票代码：\n• 分析 000001\n• 分析列表');
  }

  if (text.startsWith('图表 ')) {
    const parts = text.substring(3).trim().split(/\s+/);
    const tsCode = parts[0];
    const freq = parts[1] || 'd';
    
    if (!tsCode) {
      return buildQueryPrompt('📊 图表查询', '请输入股票代码：\n• 图表 000001.SZ (日线)\n• 图表 000001.SZ w (周线)\n• 图表 000001.SZ m (月线)');
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

  if (text === '图表') {
    return buildQueryPrompt('📊 K线图查询', '请输入股票代码：\n• 图表 000001.SZ (日线)\n• 图表 000001.SZ w (周线)\n• 图表 000001.SZ m (月线)\n\n说明：\n日线-最近10个交易日\n周线-最近10周\n月线-最近10个月');
  }

  return buildRichTextCard('🤖 收到消息', [
    { type: 'div', content: `收到: ${text}` },
    { type: 'hr' },
    { type: 'div', content: '输入"帮助"查看可用命令' }
  ], 'grey');
}

function getChartForCard(tsCode, freq) {
  const validFreqs = ['d', 'w', 'm'];
  const normalizedFreq = validFreqs.includes(freq?.toLowerCase()) ? freq.toLowerCase() : 'd';
  
  const chartData = getChartData(tsCode, normalizedFreq);
  
  if (!chartData) {
    return buildRichTextCard('❌ 图表查询失败', [
      { type: 'div', content: `未找到 **${tsCode}** 的图表数据` }
    ], 'red');
  }
  
  try {
    const imageBuffer = generateChart(chartData);
    return {
      type: 'image',
      buffer: imageBuffer,
      text: `📊 ${tsCode} ${normalizedFreq === 'd' ? '日线' : normalizedFreq === 'w' ? '周线' : '月线'}图`
    };
  } catch (error) {
    logger.error('生成图表失败: ' + error.message);
    return buildRichTextCard('❌ 图表生成失败', [
      { type: 'div', content: `错误: ${error.message}` }
    ], 'red');
  }
}

module.exports = { getReply, getChartForCard, getProgressCard };