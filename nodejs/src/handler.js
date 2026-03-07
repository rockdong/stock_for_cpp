const { getReply } = require('./reply');
const config = require('./config');

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
  
  const replyText = getReply(textContent);
  
  try {
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
  } catch (error) {
    console.error('回复失败:', error);
  }
}

module.exports = { handleMessage };
