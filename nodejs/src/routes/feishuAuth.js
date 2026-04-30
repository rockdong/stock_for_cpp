const express = require('express');
const router = express.Router();
const feishuAuthService = require('../services/feishuAuthService');
const authService = require('../services/authService');
const logger = require('../logger');

router.get('/qrcode', async (req, res) => {
  try {
    const session = await feishuAuthService.createFeishuLoginSession();
    
    const qrImageUrl = `https://api.qrserver.com/v1/create-qr-code/?size=300x300&data=${encodeURIComponent(session.authUrl)}`;

    res.json({
      success: true,
      data: {
        session_id: session.sessionId,
        auth_url: session.authUrl,
        qr_url: qrImageUrl,
        expires_at: session.expiresAt.toISOString(),
        expires_in: session.expiresIn
      }
    });
  } catch (error) {
    logger.error('生成飞书登录二维码失败: ' + error.message);
    res.status(500).json({
      success: false,
      error: error.message
    });
  }
});

router.get('/callback', async (req, res) => {
  try {
    const { code, state } = req.query;

    if (!code || !state) {
      return res.status(400).json({
        success: false,
        error: '缺少必要参数'
      });
    }

    logger.info('飞书登录回调: code=' + code + ', state=' + state);

    const userInfo = await feishuAuthService.handleFeishuCallback(code, state);

    const token = authService.generateToken(userInfo.userId);

    const userAgent = req.headers['user-agent'] || '';
    const isFeishuApp = userAgent.includes('Feishu') || userAgent.includes('Lark');

    if (isFeishuApp) {
      res.send(`
        <!DOCTYPE html>
        <html>
        <head>
          <meta charset="utf-8">
          <title>登录成功</title>
          <style>
            body { font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif; display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; background: #f5f5f5; }
            .container { text-align: center; padding: 40px; background: white; border-radius: 12px; box-shadow: 0 2px 8px rgba(0,0,0,0.1); }
            .success-icon { font-size: 48px; color: #52c41a; }
            h1 { color: #333; margin: 20px 0 10px; }
            p { color: #666; }
          </style>
        </head>
        <body>
          <div class="container">
            <div class="success-icon">✓</div>
            <h1>登录成功</h1>
            <p>欢迎，${userInfo.name}</p>
            <p style="color:#999;font-size:14px;margin-top:20px;">请在原浏览器继续操作</p>
          </div>
        </body>
        </html>
      `);
    } else {
      const redirectUrl = `${process.env.FRONTEND_URL || 'http://119.3.155.216:8880'}/login?token=${token}&name=${encodeURIComponent(userInfo.name)}`;
      res.redirect(redirectUrl);
    }
  } catch (error) {
    logger.error('飞书登录回调处理失败: ' + error.message);
    res.redirect(`${process.env.FRONTEND_URL || 'http://119.3.155.216:8880'}/login?error=login_failed`);
  }
});

router.get('/status', async (req, res) => {
  try {
    const { session_id } = req.query;

    if (!session_id) {
      return res.status(400).json({
        success: false,
        error: 'session_id 参数缺失'
      });
    }

    const session = await authService.getSessionStatus(session_id);

    if (!session) {
      return res.status(404).json({
        success: false,
        error: '会话不存在'
      });
    }

    const now = new Date();
    const expiresAt = new Date(session.expires_at);
    const isExpired = now > expiresAt;

    // 如果登录已完成（数据库中 status='success'），即使过期也返回 token
    // 因为用户已在飞书 App 完成登录，不应因 session 过期拒绝返回 token
    let token = null;
    if (session.status === 'success' && session.user_id) {
      token = authService.generateToken(session.user_id);
    }

    // 返回给前端的 status：成功则为 success，否则才判断过期
    const responseStatus = session.status === 'success' ? 'success' : (isExpired ? 'expired' : session.status);

    res.json({
      success: true,
      data: {
        session_id: session.session_id,
        status: responseStatus,
        user_id: session.user_id || null,
        token: token,
        is_expired: isExpired
      }
    });
  } catch (error) {
    logger.error('查询飞书登录状态失败: ' + error.message);
    res.status(500).json({
      success: false,
      error: error.message
    });
  }
});

module.exports = router;