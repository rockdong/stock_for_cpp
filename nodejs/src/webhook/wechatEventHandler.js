const xml2js = require('xml2js');
const authService = require('../services/authService');
const wechatService = require('../services/wechatService');
const logger = require('../logger');

const WECHAT_TOKEN = process.env.WECHAT_TOKEN || '';

const xmlParser = new xml2js.Parser({ explicitArray: false });
const xmlBuilder = new xml2js.Builder({ rootName: 'xml', headless: true });

async function handleWechatEvent(body, signature, timestamp, nonce) {
  if (!wechatService.verifySignature(signature, timestamp, nonce, WECHAT_TOKEN)) {
    logger.error('微信签名验证失败');
    throw new Error('签名验证失败');
  }

  const parsed = await xmlParser.parseStringPromise(body);
  const message = parsed.xml;

  logger.info('收到微信事件: ' + message.MsgType + '/' + message.Event);

  if (message.MsgType === 'event' && message.Event === 'subscribe') {
    return await handleSubscribeEvent(message);
  }

  if (message.MsgType === 'event' && message.Event === 'SCAN') {
    return await handleScanEvent(message);
  }

  return buildTextResponse(message.ToUserName, message.FromUserName, 'success');
}

async function handleSubscribeEvent(message) {
  const openid = message.FromUserName;
  const eventKey = message.EventKey || '';
  const sessionId = eventKey.replace('qrscene_', '');

  logger.info('用户关注公众号: openid=' + openid + ', sessionId=' + sessionId);

  const userId = await authService.createUserWithWechatBinding(openid);

  if (sessionId) {
    await authService.completeLogin(sessionId, userId);
  }

  return buildTextResponse(
    message.ToUserName,
    message.FromUserName,
    '欢迎关注股票分析公众号！您已成功登录系统。'
  );
}

async function handleScanEvent(message) {
  const openid = message.FromUserName;
  const sessionId = message.EventKey || '';

  logger.info('已关注用户扫码: openid=' + openid + ', sessionId=' + sessionId);

  const userId = await authService.getUserIdByOpenid(openid);

  if (userId && sessionId) {
    await authService.completeLogin(sessionId, userId);
  }

  return buildTextResponse(
    message.ToUserName,
    message.FromUserName,
    '扫码登录成功！'
  );
}

function buildTextResponse(toUser, fromUser, content) {
  const message = {
    ToUserName: fromUser,
    FromUserName: toUser,
    CreateTime: Math.floor(Date.now() / 1000),
    MsgType: 'text',
    Content: content
  };
  return xmlBuilder.buildObject(message);
}

module.exports = {
  handleWechatEvent
};