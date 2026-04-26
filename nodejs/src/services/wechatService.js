const axios = require('axios');
const logger = require('../logger');

const WECHAT_APP_ID = process.env.WECHAT_APP_ID;
const WECHAT_APP_SECRET = process.env.WECHAT_APP_SECRET;

let accessTokenCache = {
  token: null,
  expiresAt: 0
};

async function getAccessToken() {
  if (accessTokenCache.token && accessTokenCache.expiresAt > Date.now()) {
    return accessTokenCache.token;
  }

  try {
    const response = await axios.get(
      `https://api.weixin.qq.com/cgi-bin/token?grant_type=client_credential&appid=${WECHAT_APP_ID}&secret=${WECHAT_APP_SECRET}`
    );

    if (response.data.access_token) {
      accessTokenCache.token = response.data.access_token;
      accessTokenCache.expiresAt = Date.now() + (response.data.expires_in - 300) * 1000;
      logger.info('获取微信 access_token 成功');
      return response.data.access_token;
    } else {
      logger.error('获取 access_token 失败: ' + JSON.stringify(response.data));
      throw new Error('获取 access_token 失败');
    }
  } catch (error) {
    logger.error('获取 access_token 异常: ' + error.message);
    throw error;
  }
}

async function createQRCode(sceneId, expiresIn = 300) {
  const accessToken = await getAccessToken();

  try {
    const response = await axios.post(
      `https://api.weixin.qq.com/cgi-bin/qrcode/create?access_token=${accessToken}`,
      {
        expire_seconds: expiresIn,
        action_name: 'QR_STR_SCENE',
        action_info: {
          scene: {
            scene_str: sceneId
          }
        }
      }
    );

    if (response.data.ticket) {
      const qrUrl = `https://mp.weixin.qq.com/cgi-bin/showqrcode?ticket=${encodeURIComponent(response.data.ticket)}`;
      logger.info('生成二维码成功: ' + sceneId);
      return {
        ticket: response.data.ticket,
        qrUrl: qrUrl,
        expiresIn: response.data.expire_seconds
      };
    } else {
      logger.error('生成二维码失败: ' + JSON.stringify(response.data));
      throw new Error('生成二维码失败');
    }
  } catch (error) {
    logger.error('生成二维码异常: ' + error.message);
    throw error;
  }
}

function verifySignature(signature, timestamp, nonce, token) {
  const crypto = require('crypto');
  const arr = [token, timestamp, nonce].sort();
  const sha1 = crypto.createHash('sha1').update(arr.join('')).digest('hex');
  return sha1 === signature;
}

async function getFollowerList(nextOpenid = null) {
  const accessToken = await getAccessToken();

  try {
    let url = `https://api.weixin.qq.com/cgi-bin/user/get?access_token=${accessToken}`;
    if (nextOpenid) {
      url += `&next_openid=${nextOpenid}`;
    }

    const response = await axios.get(url);

    if (response.data.data && response.data.data.openid) {
      logger.info(`获取粉丝列表成功: ${response.data.data.openid.length} 个粉丝`);
      return {
        total: response.data.total,
        count: response.data.count,
        openids: response.data.data.openid,
        nextOpenid: response.data.next_openid
      };
    } else if (response.data.total === 0) {
      logger.info('粉丝列表为空');
      return { total: 0, count: 0, openids: [], nextOpenid: null };
    } else {
      logger.error('获取粉丝列表失败: ' + JSON.stringify(response.data));
      throw new Error('获取粉丝列表失败');
    }
  } catch (error) {
    logger.error('获取粉丝列表异常: ' + error.message);
    throw error;
  }
}

async function getUserInfo(openid) {
  const accessToken = await getAccessToken();

  try {
    const response = await axios.get(
      `https://api.weixin.qq.com/cgi-bin/user/info?access_token=${accessToken}&openid=${openid}`
    );

    if (response.data.openid) {
      return {
        openid: response.data.openid,
        subscribe: response.data.subscribe,
        nickname: response.data.nickname,
        headimgurl: response.data.headimgurl,
        subscribeTime: response.data.subscribe_time
      };
    } else {
      logger.error('获取用户信息失败: ' + JSON.stringify(response.data));
      throw new Error('获取用户信息失败');
    }
  } catch (error) {
    logger.error('获取用户信息异常: ' + error.message);
    throw error;
  }
}

async function getPublicAccountQRCode() {
  const accessToken = await getAccessToken();

  try {
    const response = await axios.post(
      `https://api.weixin.qq.com/cgi-bin/qrcode/create?access_token=${accessToken}`,
      {
        action_name: 'QR_LIMIT_STR_SCENE',
        action_info: {
          scene: {
            scene_str: 'subscribe'
          }
        }
      }
    );

    if (response.data.ticket) {
      const qrUrl = `https://mp.weixin.qq.com/cgi-bin/showqrcode?ticket=${encodeURIComponent(response.data.ticket)}`;
      logger.info('生成公众号二维码成功');
      return {
        ticket: response.data.ticket,
        qrUrl: qrUrl,
        type: 'qrcode'
      };
    }

    logger.warn('生成公众号二维码失败: ' + JSON.stringify(response.data));
    const publicUrl = `https://mp.weixin.qq.com/mp/profile_ext?action=home&__biz=${Buffer.from(WECHAT_APP_ID).toString('base64')}#wechat_redirect`;
    return {
      qrUrl: publicUrl,
      type: 'public_url'
    };
  } catch (error) {
    logger.warn('生成公众号二维码异常: ' + error.message);
    const publicUrl = `https://mp.weixin.qq.com/mp/profile_ext?action=home&__biz=${Buffer.from(WECHAT_APP_ID).toString('base64')}#wechat_redirect`;
    return {
      qrUrl: publicUrl,
      type: 'public_url'
    };
  }
}

module.exports = {
  getAccessToken,
  createQRCode,
  verifySignature,
  getFollowerList,
  getUserInfo,
  getPublicAccountQRCode
};