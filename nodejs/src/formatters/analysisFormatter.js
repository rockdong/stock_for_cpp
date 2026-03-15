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