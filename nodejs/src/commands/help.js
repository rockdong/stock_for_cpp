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