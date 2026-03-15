const { getAnalysisProgress } = require('../database');
const { formatProgressCard } = require('../formatters/progressFormatter');
const { register } = require('./index');

function handleProgress(text) {
  const progress = getAnalysisProgress();
  return formatProgressCard(progress);
}

register('分析进度', handleProgress, '查看分析进度');

module.exports = { handleProgress };