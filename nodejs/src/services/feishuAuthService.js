const axios = require('axios');
const crypto = require('crypto');
const logger = require('../logger');
const authService = require('./authService');

const FEISHU_APP_ID = process.env.FEISHU_APP_ID;
const FEISHU_APP_SECRET = process.env.FEISHU_APP_SECRET;
const BASE_URL = process.env.BASE_URL || 'http://119.3.155.216:8880';

const PASSPORT_BASE_URL = 'https://passport.larksuite.com';
const OPEN_API_BASE_URL = 'https://open.larksuite.com';

let appAccessTokenCache = {
  token: null,
  expiresAt: 0
};

async function getAppAccessToken() {
  if (appAccessTokenCache.token && appAccessTokenCache.expiresAt > Date.now()) {
    return appAccessTokenCache.token;
  }

  try {
    const response = await axios.post(
      `${OPEN_API_BASE_URL}/open-apis/auth/v3/app_access_token/internal`,
      {
        app_id: FEISHU_APP_ID,
        app_secret: FEISHU_APP_SECRET
      }
    );

    if (response.data.code === 0 && response.data.app_access_token) {
      appAccessTokenCache.token = response.data.app_access_token;
      appAccessTokenCache.expiresAt = Date.now() + (response.data.expire - 300) * 1000;
      logger.info('获取飞书 app_access_token 成功');
      return response.data.app_access_token;
    } else {
      logger.error('获取 app_access_token 失败: ' + JSON.stringify(response.data));
      throw new Error('获取 app_access_token 失败: ' + response.data.msg);
    }
  } catch (error) {
    logger.error('获取 app_access_token 异常: ' + error.message);
    throw error;
  }
}

async function createFeishuLoginSession() {
  const session = await authService.createSession();
  
  const redirectUri = `${BASE_URL}/api/auth/feishu/callback`;
  
  const authUrl = `${PASSPORT_BASE_URL}/suite/passport/oauth/authorize?` +
    `client_id=${FEISHU_APP_ID}&` +
    `redirect_uri=${encodeURIComponent(redirectUri)}&` +
    `response_type=code&` +
    `state=${session.sessionId}&` +
    `scope=user:info`;

  logger.info('创建飞书登录会话: ' + session.sessionId);

  return {
    sessionId: session.sessionId,
    authUrl,
    expiresAt: session.expiresAt,
    expiresIn: 300
  };
}

async function getUserAccessToken(code) {
  const appAccessToken = await getAppAccessToken();

  try {
    const response = await axios.post(
      `${OPEN_API_BASE_URL}/open-apis/authen/v1/oidc/access_token`,
      {
        grant_type: 'authorization_code',
        code: code
      },
      {
        headers: {
          'Authorization': `Bearer ${appAccessToken}`,
          'Content-Type': 'application/json'
        }
      }
    );

    if (response.data.code === 0 && response.data.data) {
      logger.info('获取飞书 user_access_token 成功');
      return response.data.data;
    } else {
      logger.error('获取 user_access_token 失败: ' + JSON.stringify(response.data));
      throw new Error('获取 user_access_token 失败: ' + response.data.msg);
    }
  } catch (error) {
    logger.error('获取 user_access_token 异常: ' + error.message);
    throw error;
  }
}

async function getUserInfo(userAccessToken) {
  try {
    const response = await axios.get(
      `${OPEN_API_BASE_URL}/open-apis/authen/v1/user_info`,
      {
        headers: {
          'Authorization': `Bearer ${userAccessToken}`
        }
      }
    );

    if (response.data.code === 0 && response.data.data) {
      logger.info('获取飞书用户信息成功: ' + response.data.data.name);
      return response.data.data;
    } else {
      logger.error('获取用户信息失败: ' + JSON.stringify(response.data));
      throw new Error('获取用户信息失败: ' + response.data.msg);
    }
  } catch (error) {
    logger.error('获取用户信息异常: ' + error.message);
    throw error;
  }
}

async function handleFeishuCallback(code, state) {
  const tokenData = await getUserAccessToken(code);
  const userInfo = await getUserInfo(tokenData.access_token);

  const userId = userInfo.sub || userInfo.open_id;
  
  await authService.completeLogin(state, userId);
  logger.info('飞书登录完成: state=' + state + ', userId=' + userId + ', name=' + userInfo.name);

  return {
    userId,
    name: userInfo.name,
    avatarUrl: userInfo.avatar_url,
    email: userInfo.email
  };
}

module.exports = {
  createFeishuLoginSession,
  handleFeishuCallback,
  getUserAccessToken,
  getUserInfo
};