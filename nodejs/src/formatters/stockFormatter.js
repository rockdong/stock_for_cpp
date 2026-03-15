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
