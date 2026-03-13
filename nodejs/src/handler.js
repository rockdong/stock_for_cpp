const { getReply } = require('./reply');
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

module.exports = { handleMessage };
