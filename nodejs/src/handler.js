const { getReply, getChartForCard } = require('./reply');
const config = require('./config');
const logger = require('./logger');

async function uploadImage(imageBuffer) {
  try {
    const response = await config.client.im.image.create({
      data: {
        image_type: 'message',
        image: imageBuffer,
      },
    });
    return response.data.image_key;
  } catch (error) {
    logger.error('上传图片失败: ' + error.message);
    throw error;
  }
}

async function handleCardAction(event) {
  const action = event.action;
  const value = JSON.parse(action.value || '{}');
  const openId = event.operator?.open_id;
  const openMessageId = event.open_message_id;
  
  logger.debug('卡片按钮点击: ' + JSON.stringify(value));
  
  if (value.action === 'chart') {
    const replyData = getChartForCard(value.ts_code, value.freq);
    
    if (replyData && replyData.type === 'image' && replyData.buffer) {
      try {
        const imageKey = await uploadImage(replyData.buffer);
        await config.client.im.message.create({
          params: { receive_id_type: 'open_id' },
          data: {
            receive_id: openId,
            msg_type: 'image',
            content: JSON.stringify({ image_key: imageKey }),
          },
        });
        logger.info('图表图片发送成功');
      } catch (error) {
        logger.error('发送图表失败: ' + error.message);
      }
    }
  } else if (value.action === 'refresh') {
    try {
      const { getProgressCard } = require('./reply');
      const progressCard = getProgressCard();
      
      await config.client.im.message.create({
        params: { receive_id_type: 'open_id' },
        data: {
          receive_id: openId,
          msg_type: 'interactive',
          content: JSON.stringify(progressCard.card),
        },
      });
      logger.info('进度刷新消息发送成功');
    } catch (error) {
      logger.error('发送进度刷新消息失败: ' + error.message);
    }
  }
}

async function handleMessage(event) {
  const message = event.message;
  const messageId = message.message_id;
  const chatId = message.chat_id;
  const messageType = message.message_type;
  
  if (messageType !== 'text') {
    logger.debug('忽略非文本消息');
    return;
  }
  
  const textContent = JSON.parse(message.content).text;
  const senderId = message.sender_id;
  
  logger.info(`收到消息 from ${senderId?.open_id}: ${textContent}`);
  
  const replyData = getReply(textContent);
  
  try {
    if (replyData && replyData.msg_type === 'interactive') {
      await config.client.im.message.create({
        params: { receive_id_type: 'chat_id' },
        data: {
          receive_id: chatId,
          msg_type: 'interactive',
          content: JSON.stringify(replyData.card),
        },
      });
      logger.info('卡片消息发送成功');
      return;
    }

    if (Array.isArray(replyData)) {
      for (const msg of replyData) {
        if (msg && msg.type === 'image' && msg.buffer) {
          const imageKey = await uploadImage(msg.buffer);
          await config.client.im.message.create({
            params: { receive_id_type: 'chat_id' },
            data: {
              receive_id: chatId,
              msg_type: 'image',
              content: JSON.stringify({ image_key: imageKey }),
            },
          });
        } else {
          const replyText = typeof msg === 'string' ? msg : msg.text || '无法处理';
          await config.client.im.message.create({
            params: { receive_id_type: 'chat_id' },
            data: {
              receive_id: chatId,
              msg_type: 'text',
              content: JSON.stringify({ text: replyText }),
            },
          });
        }
      }
      logger.info(`发送 ${replyData.length} 条消息成功`);
    } else if (replyData && replyData.type === 'image' && replyData.buffer) {
      const imageKey = await uploadImage(replyData.buffer);
      
      await config.client.im.message.create({
        params: {
          receive_id_type: 'chat_id',
        },
        data: {
          receive_id: chatId,
          msg_type: 'image',
          content: JSON.stringify({ image_key: imageKey }),
        },
      });
      logger.info('图片回复成功');
    } else {
      const replyText = typeof replyData === 'string' ? replyData : replyData.text || '无法处理';
      
      await config.client.im.message.create({
        params: {
          receive_id_type: 'chat_id',
        },
        data: {
          receive_id: chatId,
          msg_type: 'text',
          content: JSON.stringify({ text: replyText }),
        },
      });
      logger.info('回复成功');
    }
  } catch (error) {
    logger.error('回复失败: ' + error.message);
  }
}

module.exports = { handleMessage, handleCardAction };