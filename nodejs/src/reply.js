const { searchStocks, findStockByTsCode, findAllStocks, findStocksByIndustry, findAnalysisResults, findAllAnalysisResults, findLatestAnalysisResults, getAnalysisProgress, getChartData } = require('./database');
const { generateChart } = require('./chartGenerator');

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

function formatAnalysisAsTable(results) {
  if (!results || results.length === 0) {
    return '未找到分析结果';
  }

  const labelEmoji = {
    'BUY': '🟢 买入',
    'SELL': '🔴 卖出',
    'HOLD': '⚪ 持有',
  };

  const header = '| 代码 | 名称 | 策略 | 日期 | 信号 |';
  const separator = '|------|------|------|------|------|';

  const rows = results.map(r => {
    const label = labelEmoji[r.label] || r.label;
    return `| ${r.ts_code} | ${r.name || '-'} | ${r.strategy_name} | ${r.trade_date} | ${label} |`;
  });

  return `📈 分析结果（共 ${results.length} 条）：\n\n${header}\n${separator}\n${rows.join('\n')}`;
}

function formatLatestAnalysisAsTable(results) {
  if (!results || results.length === 0) {
    return '未找到分析结果';
  }

  const tradeDate = results[0].trade_date;
  const header = '| 股票代码 | 股票名称 | 分析日期 | opt |';
  const separator = '|----------|----------|----------|-----|';

  const rows = results.map(r => 
    `| ${r.ts_code} | ${r.name || '-'} | ${r.trade_date} | ${r.opt || '-'} |`
  );

  return `📊 最新分析结果（${tradeDate}，共 ${results.length} 条）：\n\n${header}\n${separator}\n${rows.join('\n')}`;
}

function formatProgress(progress) {
  if (!progress) {
    return '无法获取分析进度';
  }

  const statusEmoji = {
    'idle': '⚪ 空闲',
    'running': '🔄 运行中',
    'completed': '✅ 已完成',
  };

  const status = statusEmoji[progress.status] || progress.status;
  const total = progress.total || 0;
  const completed = progress.completed || 0;
  const failed = progress.failed || 0;
  const percent = total > 0 ? Math.round((completed / total) * 100) : 0;
  
  const filled = Math.round(percent / 5);
  const empty = 20 - filled;
  const bar = '█'.repeat(filled) + '░'.repeat(empty);

  let elapsed = '';
  if (progress.started_at) {
    const start = new Date(progress.started_at);
    const now = progress.status === 'running' ? new Date() : new Date(progress.updated_at);
    const diffMs = now - start;
    const minutes = Math.floor(diffMs / 60000);
    const seconds = Math.floor((diffMs % 60000) / 1000);
    elapsed = `${minutes}分${seconds}秒`;
  }

  let result = `📊 分析进度\n\n`;
  result += `状态: ${status}\n`;
  result += `进度: ${completed} / ${total} (${percent}%)\n`;
  result += `成功: ${completed - failed}  失败: ${failed}\n`;
  if (progress.started_at) {
    result += `开始时间: ${progress.started_at}\n`;
    result += `已运行: ${elapsed}\n`;
  }
  result += `\n[${bar}] ${percent}%`;

  return result;
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
- 分析 <代码>：查询分析结果
- 分析列表：显示所有分析结果
- 分析结果：显示最近一天的分析结果
- 分析进度：显示当前分析进度
- 图表 <代码> [频率]：生成K线图

示例：
- 股票 000001
- 股票 平安银行
- 股票列表
- 行业 银行
- 分析 000001
- 分析列表
- 分析结果
- 分析进度
- 图表 000001.SZ
- 图表 000001.SZ w`;
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

  if (text === '分析列表') {
    const results = findAllAnalysisResults(20);
    return formatAnalysisAsTable(results);
  }

  if (text === '分析结果') {
    const results = findLatestAnalysisResults();
    return formatLatestAnalysisAsTable(results);
  }

  if (text === '分析进度') {
    const progress = getAnalysisProgress();
    return formatProgress(progress);
  }

  if (text.startsWith('分析 ')) {
    const keyword = text.substring(3).trim();
    const results = findAnalysisResults(keyword, null, 20);
    return formatAnalysisAsTable(results);
  }

  if (text === '股票') {
    return `📈 股票查询

请输入股票代码或名称：
- 股票 000001（代码查询）
- 股票 平安银行（名称查询）
- 股票列表（查看所有）
- 行业 银行（按行业）`;
  }

  if (text === '分析') {
    return `📊 分析结果查询

请输入股票代码：
- 分析 000001（查询单只股票）
- 分析列表（查看所有）`;
  }

  if (text.startsWith('图表 ')) {
    const parts = text.substring(3).trim().split(/\s+/);
    const tsCode = parts[0];
    const freq = parts[1] || 'd';
    
    if (!tsCode) {
      return `📊 图表查询

请输入股票代码：
- 图表 000001.SZ（日线图）
- 图表 000001.SZ w（周线图）
- 图表 000001.SZ m（月线图）`;
    }
    
    const normalizedCode = tsCode.includes('.') ? tsCode : tsCode + '.SZ';
    const validFreqs = ['d', 'w', 'm'];
    const normalizedFreq = validFreqs.includes(freq.toLowerCase()) ? freq.toLowerCase() : 'd';
    
    const chartData = getChartData(normalizedCode, normalizedFreq);
    
    if (!chartData) {
      return `❌ 未找到 ${normalizedCode} 的图表数据

可能的原因：
1. 该股票尚未分析
2. 股票代码不正确
3. 数据库中没有对应频率的数据

请确认股票代码格式如：000001.SZ`;
    }
    
    try {
      const imageBuffer = generateChart(chartData);
      return {
        type: 'image',
        buffer: imageBuffer,
        text: `📊 ${normalizedCode} ${normalizedFreq === 'd' ? '日线' : normalizedFreq === 'w' ? '周线' : '月线'}图`
      };
    } catch (error) {
      console.error('生成图表失败:', error);
      return `❌ 生成图表失败: ${error.message}`;
    }
  }

  if (text === '图表') {
    return `📊 K线图查询

请输入股票代码：
- 图表 000001.SZ（日线图）
- 图表 000001.SZ w（周线图）
- 图表 000001.SZ m（月线图）

说明：
- 日线：显示最近10个交易日
- 周线：显示最近10周
- 月线：显示最近10个月`;
  }

  return `🤖 收到消息：${text}

输入"帮助"查看可用命令。`;
}

module.exports = { getReply };