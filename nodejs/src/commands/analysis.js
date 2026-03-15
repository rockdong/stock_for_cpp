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