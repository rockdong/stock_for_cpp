const messageService = require('./services/messageService');
const { route } = require('./commands');
const { buildRichTextCard } = require('./messageBuilder');
const { generateChart } = require('./chartGenerator');
const { getChartData } = require('./database');
const { formatProgressCard } = require('./formatters/progressFormatter');
const { getAnalysisProgress } = require('./database');
const logger = require('./logger');

async function handleCardAction(event) {
  const action = event.action;
  const value = JSON.parse(action.value || '{}');
  const openId = event.operator?.open_id;
  
  logger.debug('卡片按钮点击: ' + JSON.stringify(value));
  
  if (value.action === 'chart') {
    const tsCode = value.ts_code;
    const freq = value.freq;
    const validFreqs = ['d', 'w', 'm'];
    const normalizedFreq = validFreqs.includes(freq?.toLowerCase()) ? freq.toLowerCase() : 'd';
    
    const chartData = getChartData(tsCode, normalizedFreq);
    
    if (chartData) {
      try {
        const imageBuffer = generateChart(chartData);
        await messageService.sendImage(openId, imageBuffer, 'open_id');
        logger.info('图表图片发送成功');
      } catch (error) {
        logger.error('发送图表失败: ' + error.message);
      }
    } else {
      await messageService.sendCard(openId, buildRichTextCard('❌ 图表查询失败', [
        { type: 'div', content: `未找到 **${tsCode}** 的图表数据` }
      ], 'red').card, 'open_id');
    }
  } else if (value.action === 'refresh') {
    const progress = getAnalysisProgress();
    const progressCard = formatProgressCard(progress);
    
    await messageService.sendCard(openId, progressCard.card, 'open_id');
    logger.info('进度刷新消息发送成功');
  }
}

async function handleMessage(event) {
  const message = event.message;
  const chatId = message.chat_id;
  const messageType = message.message_type;
  
  if (messageType !== 'text') {
    logger.debug('忽略非文本消息');
    return;
  }
  
  const textContent = JSON.parse(message.content).text;
  const senderId = message.sender_id;
  
  logger.info(`收到消息 from ${senderId?.open_id}: ${textContent}`);
  
  const replyData = route(textContent);
  
  if (!replyData) {
    const defaultReply = buildRichTextCard('🤖 收到消息', [
      { type: 'div', content: `收到: ${textContent}` },
      { type: 'hr' },
      { type: 'div', content: '输入"帮助"查看可用命令' }
    ], 'grey');
    await messageService.sendCard(chatId, defaultReply.card);
    return;
  }
  
  if (replyData.msg_type === 'interactive') {
    await messageService.sendCard(chatId, replyData.card);
  } else if (replyData.type === 'image' && replyData.buffer) {
    await messageService.sendImage(chatId, replyData.buffer);
  } else if (Array.isArray(replyData)) {
    for (const msg of replyData) {
      if (msg && msg.type === 'image' && msg.buffer) {
        await messageService.sendImage(chatId, msg.buffer);
      } else {
        const replyText = typeof msg === 'string' ? msg : msg.text || '无法处理';
        await messageService.sendText(chatId, replyText);
      }
    }
  } else {
    const replyText = typeof replyData === 'string' ? replyData : replyData.text || '无法处理';
    await messageService.sendText(chatId, replyText);
  }
}

module.exports = { handleMessage, handleCardAction };