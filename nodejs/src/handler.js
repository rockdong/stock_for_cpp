const { getReply, getChartForCard } = require('./reply');
const config = require('./config');

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
    console.error('上传图片失败:', error);
    throw error;
  }
}

async function handleCardAction(event) {
  const action = event.action;
  const value = JSON.parse(action.value || '{}');
  const openId = event.operator?.open_id;
  const openMessageId = event.open_message_id;
  
  console.log('卡片按钮点击:', value);
  
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
        console.log('图表图片发送成功');
      } catch (error) {
        console.error('发送图表失败:', error);
      }
    }
  } else if (value.action === 'refresh') {
    try {
      const { getProgressCard } = require('./reply');
      const progressCard = getProgressCard();
      
      await config.client.im.message.patch({
        path: {
          message_id: openMessageId,
        },
        params: {
          receive_id_type: 'open_id',
        },
        data: {
          content: JSON.stringify(progressCard.card),
        },
      });
      console.log('进度刷新成功');
    } catch (error) {
      console.error('刷新进度失败:', error);
    }
  }
}

async function handleMessage(event) {
  const message = event.message;
  const messageId = message.message_id;
  const chatId = message.chat_id;
  const messageType = message.message_type;
  
  if (messageType !== 'text') {
    console.log('忽略非文本消息');
    return;
  }
  
  const textContent = JSON.parse(message.content).text;
  const senderId = message.sender_id;
  
  console.log(`收到消息 from ${senderId?.open_id}: ${textContent}`);
  
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
      console.log('卡片消息发送成功');
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
      console.log(`发送 ${replyData.length} 条消息成功`);
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
      console.log('图片回复成功');
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
      console.log('回复成功');
    }
  } catch (error) {
    console.error('回复失败:', error);
  }
}

module.exports = { handleMessage, handleCardAction };
