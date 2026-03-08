const { searchStocks, findStockByTsCode, findAllStocks, findStocksByIndustry } = require('./database');

function formatStockAsTable(stocks) {
  if (!stocks || stocks.length === 0) {
    return '未找到相关股票信息';
  }

  const header = '| 代码 | 名称 | 行业 | 市场 | 交易所 |';
  const separator = '|------|------|------|------|--------|';

  const rows = stocks.map(s => 
    `| ${s.ts_code} | ${s.name} | ${s.industry || '-'} | ${s.market || '-'} | ${s.exchange || '-'} |`
  );

  return `📊 股票查询结果（共 ${stocks.length} 条）：\n\n${header}\n${separator}\n${rows.join('\n')}`;
}

function getReply(messageText) {
  const text = messageText.trim();

  if (text === '帮助') {
    return `📖 帮助信息

支持的操作：
- 帮助：显示此帮助信息
- hello：测试连接
- 股票 <代码/名称>：查询股票信息
- 股票列表：显示所有股票
- 行业 <行业名>：按行业查询

示例：
- 股票 000001
- 股票 平安银行
- 股票列表
- 行业 银行`;
  }

  if (text === 'hello' || text === '你好') {
    return '👋 你好！我是股票分析机器人，已连接成功！';
  }

  if (text === '股票列表') {
    const stocks = findAllStocks(20);
    return formatStockAsTable(stocks);
  }

  if (text.startsWith('股票 ')) {
    const keyword = text.substring(3).trim();
    const stocks = searchStocks(keyword, 10);
    return formatStockAsTable(stocks);
  }

  if (text.startsWith('行业 ')) {
    const industry = text.substring(3).trim();
    const stocks = findStocksByIndustry(industry, 20);
    return formatStockAsTable(stocks);
  }

  if (text === '股票') {
    return `📈 股票查询

请输入股票代码或名称：
- 股票 000001（代码查询）
- 股票 平安银行（名称查询）
- 股票列表（查看所有）
- 行业 银行（按行业）`;
  }

  if (text === '分析' || text === '分析') {
    return `🔍 开始分析

正在准备分析功能...
请稍候！`;
  }

  return `🤖 收到消息：${text}

输入"帮助"查看可用命令。`;
}

module.exports = { getReply };