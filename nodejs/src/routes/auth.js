const express = require('express');
const router = express.Router();
const authService = require('../services/authService');
const wechatService = require('../services/wechatService');
const logger = require('../logger');

router.post('/admin/login', async (req, res) => {
  try {
    const { username, password } = req.body;

    if (!username || !password) {
      return res.status(400).json({
        success: false,
        error: '用户名和密码必填'
      });
    }

    if (username !== 'admin' || password !== 'admin123') {
      return res.status(401).json({
        success: false,
        error: '用户名或密码错误'
      });
    }

    const token = authService.generateToken(1);

    res.json({
      success: true,
      data: {
        token,
        user_id: 1,
        username: 'admin'
      }
    });
  } catch (error) {
    logger.error('管理员登录失败: ' + error.message);
    res.status(500).json({
      success: false,
      error: error.message
    });
  }
});

router.get('/qrcode', async (req, res) => {
  try {
    const session = await authService.createSessionWithSnapshot();
    // 创建带 session_id 的临时二维码，用户扫码后微信会推送事件到 webhook
    const qrCode = await wechatService.createQRCode(session.sessionId, 300);

    res.json({
      success: true,
      data: {
        session_id: session.sessionId,
        qr_url: qrCode.qrUrl,
        qr_type: 'qrcode',
        expires_at: session.expiresAt,
        expires_in: qrCode.expiresIn || 60
      }
    });
  } catch (error) {
    logger.error('生成二维码失败: ' + error.message);
    res.status(500).json({
      success: false,
      error: error.message
    });
  }
});

router.get('/status', async (req, res) => {
  try {
    const { sessionId } = req.query;

    if (!sessionId) {
      return res.status(400).json({
        success: false,
        error: 'sessionId 参数缺失'
      });
    }

    const session = await authService.getSessionStatus(sessionId);

    if (!session) {
      return res.status(404).json({
        success: false,
        error: '会话不存在'
      });
    }

    const now = new Date();
    const expiresAt = new Date(session.expires_at);
    const isExpired = now > expiresAt;

    res.json({
      success: true,
      data: {
        session_id: session.session_id,
        status: isExpired ? 'expired' : session.status,
        user_id: session.user_id || null,
        is_expired: isExpired
      }
    });
  } catch (error) {
    logger.error('查询登录状态失败: ' + error.message);
    res.status(500).json({
      success: false,
      error: error.message
    });
  }
});

router.post('/token', async (req, res) => {
  try {
    const { sessionId } = req.body;

    if (!sessionId) {
      return res.status(400).json({
        success: false,
        error: 'sessionId 参数缺失'
      });
    }

    const session = await authService.getSessionStatus(sessionId);

    if (!session) {
      return res.status(404).json({
        success: false,
        error: '会话不存在'
      });
    }

    if (session.status !== 'success') {
      return res.status(400).json({
        success: false,
        error: '登录未完成',
        status: session.status
      });
    }

    const now = new Date();
    const expiresAt = new Date(session.expires_at);
    if (now > expiresAt) {
      return res.status(400).json({
        success: false,
        error: '会话已过期'
      });
    }

    const token = authService.generateToken(session.user_id);

    res.json({
      success: true,
      data: {
        token,
        user_id: session.user_id
      }
    });
  } catch (error) {
    logger.error('生成 token 失败: ' + error.message);
    res.status(500).json({
      success: false,
      error: error.message
    });
  }
});

module.exports = router;