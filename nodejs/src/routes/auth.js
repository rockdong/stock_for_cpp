const express = require('express');
const router = express.Router();
const authService = require('../services/authService');
const wechatService = require('../services/wechatService');
const logger = require('../logger');

router.get('/qrcode', async (req, res) => {
  try {
    const session = await authService.createSessionWithSnapshot();
    const qrCode = await wechatService.getPublicAccountQRCode();

    let qrImageUrl;
    if (qrCode.type === 'qrcode') {
      qrImageUrl = qrCode.qrUrl;
    } else {
      qrImageUrl = `https://api.qrserver.com/v1/create-qr-code/?size=300x300&data=${encodeURIComponent(qrCode.qrUrl)}`;
    }

    res.json({
      success: true,
      data: {
        sessionId: session.sessionId,
        qrUrl: qrImageUrl,
        qrType: 'image',
        expiresAt: session.expiresAt,
        expiresIn: 60
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
        sessionId: session.session_id,
        status: isExpired ? 'expired' : session.status,
        userId: session.user_id || null,
        isExpired
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
        userId: session.user_id
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