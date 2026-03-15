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