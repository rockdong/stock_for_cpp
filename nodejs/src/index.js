const lark = require('@larksuiteoapi/node-sdk');
const config = require('./config');
const { handleMessage, handleCardAction } = require('./handler');

async function main() {
  console.log('启动飞书机器人（WebSocket 长连接模式）...');
  
  const baseConfig = {
    appId: config.appId,
    appSecret: config.appSecret,
  };
  
  const client = new lark.Client(baseConfig);
  
  const eventDispatcher = new lark.EventDispatcher({
    async handler(ctx, event) {
      if (event.header?.event_type === 'card.action.trigger') {
        console.log('卡片按钮点击事件:', JSON.stringify(event));
        await handleCardAction(event.event);
        return;
      }
      if (event.header?.event_type === 'im.message.receive_v1') {
        console.log('收到消息:', JSON.stringify(event));
        const message = event.event?.message;
        if (message && message.message_type === 'text') {
          await handleMessage(event.event);
        }
      }
    }
  });
  
  const wsClient = new lark.WSClient({
    ...baseConfig,
    loggerLevel: lark.LoggerLevel.info,
  });
  
  wsClient.start({
    eventDispatcher: eventDispatcher,
  });
  
  console.log('飞书机器人服务已启动，通过 WebSocket 长连接监听消息...');
}

main().catch(console.error);
