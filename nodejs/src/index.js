const lark = require('@larksuiteoapi/node-sdk');
const config = require('./config');
const { handleMessage, handleCardAction } = require('./handler');
const logger = require('./logger');

async function main() {
  logger.info('启动飞书机器人（WebSocket 长连接模式）...');
  
  const baseConfig = {
    appId: config.appId,
    appSecret: config.appSecret,
  };
  
  const client = new lark.Client(baseConfig);
  
  const eventDispatcher = new lark.EventDispatcher({}).register({
    'im.message.receive_v1': async (data) => {
      logger.debug('收到消息: ' + JSON.stringify(data));
      const message = data.message;
      if (message && message.message_type === 'text') {
        await handleMessage(data);
      }
    }
  }).on('card.action.trigger', async (data) => {
    logger.debug('卡片按钮点击事件: ' + JSON.stringify(data));
    await handleCardAction(data);
  });
  
  const wsClient = new lark.WSClient({
    ...baseConfig,
    loggerLevel: lark.LoggerLevel.info,
  });
  
  wsClient.start({
    eventDispatcher: eventDispatcher,
  });
  
  logger.info('飞书机器人服务已启动，通过 WebSocket 长连接监听消息...');
}

main().catch(err => logger.error('程序异常: ' + err.message));