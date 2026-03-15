const client = require('../client');
const logger = require('../logger');

class MessageService {
  constructor(clientInstance) {
    this.client = clientInstance || client;
  }

  async sendText(receiveId, text, idType = 'chat_id') {
    try {
      await this.client.im.message.create({
        params: { receive_id_type: idType },
        data: {
          receive_id: receiveId,
          msg_type: 'text',
          content: JSON.stringify({ text }),
        },
      });
      logger.info('文本消息发送成功');
      return { success: true };
    } catch (error) {
      logger.error('发送文本失败: ' + error.message);
      return { success: false, error };
    }
  }

  async sendCard(receiveId, card, idType = 'chat_id') {
    try {
      await this.client.im.message.create({
        params: { receive_id_type: idType },
        data: {
          receive_id: receiveId,
          msg_type: 'interactive',
          content: JSON.stringify(card),
        },
      });
      logger.info('卡片消息发送成功');
      return { success: true };
    } catch (error) {
      logger.error('发送卡片失败: ' + error.message);
      return { success: false, error };
    }
  }

  async uploadImage(imageBuffer) {
    try {
      const response = await this.client.im.image.create({
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

  async sendImage(receiveId, imageBuffer, idType = 'chat_id') {
    try {
      const imageKey = await this.uploadImage(imageBuffer);
      await this.client.im.message.create({
        params: { receive_id_type: idType },
        data: {
          receive_id: receiveId,
          msg_type: 'image',
          content: JSON.stringify({ image_key: imageKey }),
        },
      });
      logger.info('图片消息发送成功');
      return { success: true };
    } catch (error) {
      logger.error('发送图片失败: ' + error.message);
      return { success: false, error };
    }
  }
}

const messageService = new MessageService();

module.exports = messageService;
module.exports.MessageService = MessageService;