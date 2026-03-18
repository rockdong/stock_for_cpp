const { buildRichTextCard } = require('../messageBuilder');

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
  buildStockElement,
  buildGroupCard,
  FREQ_ICONS,
  FREQ_LABELS
};