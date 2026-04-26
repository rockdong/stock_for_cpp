const lark = require('@larksuiteoapi/node-sdk');
const express = require('express');
const cors = require('cors');
const config = require('./config');
const { handleMessage, handleCardAction } = require('./handler');
const apiRoutes = require('./api');
const authRoutes = require('./routes/auth');
const logger = require('./logger');
const requestLogger = require('./middleware/requestLogger');
const wechatEventHandler = require('./webhook/wechatEventHandler');
const wechatService = require('./services/wechatService');
const authService = require('./services/authService');

const HTTP_PORT = process.env.HTTP_PORT || 3000;

async function main() {
  logger.info('启动飞书机器人（WebSocket 长连接模式）...');
  logger.info(`App ID: ${config.appId ? config.appId.substring(0, 10) + '...' : '未配置'}`);
  
  await authService.initDb();
  
  if (!config.appId || !config.appSecret) {
    logger.error('飞书配置缺失！请检查 FEISHU_APP_ID 和 FEISHU_APP_SECRET 环境变量');
    process.exit(1);
  }
  
  const baseConfig = {
    appId: config.appId,
    appSecret: config.appSecret,
  };
  
  const client = new lark.Client(baseConfig);
  
  // 启动 HTTP API 服务（供微信小程序调用）
  const httpApp = express();
  httpApp.use(cors());
  httpApp.use(express.json());
  httpApp.use(requestLogger);
  httpApp.use('/api', apiRoutes);
  httpApp.use('/api/auth', authRoutes);
  
  httpApp.post('/api/wechat/event', express.text({ type: 'text/xml' }), async (req, res) => {
    try {
      const { signature, timestamp, nonce } = req.query;
      const response = await wechatEventHandler.handleWechatEvent(
        req.body,
        signature,
        timestamp,
        nonce
      );
      res.set('Content-Type', 'text/xml');
      res.send(response);
    } catch (error) {
      logger.error('处理微信事件失败: ' + error.message);
      res.status(500).send('error');
    }
  });
  
  httpApp.get('/api/wechat/event', (req, res) => {
    const { signature, timestamp, nonce, echostr } = req.query;
    if (wechatService.verifySignature(signature, timestamp, nonce, process.env.WECHAT_TOKEN)) {
      res.send(echostr);
    } else {
      res.status(403).send('invalid signature');
    }
  });
  
  httpApp.listen(HTTP_PORT, () => {
    logger.info(`HTTP API 服务已启动，端口: ${HTTP_PORT}`);
    logger.info(`API 文档: http://localhost:${HTTP_PORT}/api/stocks`);
  });
  
  // 飞书 WebSocket 连接
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