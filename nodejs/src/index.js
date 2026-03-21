const lark = require('@larksuiteoapi/node-sdk');
const config = require('./config');
const { handleMessage, handleCardAction } = require('./handler');
const logger = require('./logger');

async function main() {
  logger.info('启动飞书机器人（WebSocket 长连接模式）...');
  logger.info(`App ID: ${config.appId ? config.appId.substring(0, 10) + '...' : '未配置'}`);
  
  if (!config.appId || !config.appSecret) {
    logger.error('飞书配置缺失！请检查 FEISHU_APP_ID 和 FEISHU_APP_SECRET 环境变量');
    process.exit(1);
  }
  
  const baseConfig = {
    appId: config.appId,
    appSecret: config.appSecret,
  };
  
  const client = new lark.Client(baseConfig);
  
  const eventDispatcher = new lark.EventDispatcher({}).register({
    'im.message.receive_v1': async (data) => {
      logger.info('收到消息事件');
      logger.debug('消息详情: ' + JSON.stringify(data));
      const message = data.message;
      if (message && message.message_type === 'text') {
        await handleMessage(data);
      }
    },
    'card.action.trigger': async (data) => {
      logger.info('收到卡片回调');
      logger.debug('卡片详情: ' + JSON.stringify(data));
      await handleCardAction(data);
    }
  });
  
  const wsClient = new lark.WSClient({
    ...baseConfig,
    loggerLevel: lark.LoggerLevel.debug,
  });
  
  logger.info('正在建立 WebSocket 连接...');
  
  wsClient.start({
    eventDispatcher: eventDispatcher,
  });
  
  logger.info('飞书机器人服务已启动，通过 WebSocket 长连接监听消息...');
}

main().catch(err => {
  logger.error('程序异常: ' + err.message);
  logger.error(err.stack);
});