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
