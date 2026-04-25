# 微信公众号扫码登录实现计划

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** 实现 Web 前端微信公众号扫码登录功能，用户扫码关注后自动绑定账号并获取 JWT token。

**Architecture:** 后端 Node.js 负责生成带参数二维码、处理微信事件推送、用户绑定；前端 React 实现登录页面、轮询状态、token 管理。使用 MySQL 存储用户和会话数据。

**Tech Stack:** Node.js Express, MySQL, JWT, 微信公众号 API, React, Zustand

---

## Task 1: 数据库表创建

**Files:**
- Modify: `nodejs/src/database.js`（添加新表的初始化逻辑）
- Modify: `backend/prisma/schema.prisma`（添加 Prisma 模型定义）

**Step 1: 在 Prisma schema 中添加用户和认证相关模型**

修改 `backend/prisma/schema.prisma`，在文件末尾添加：

```prisma
model User {
  id        Int       @id @default(autoincrement())
  createdAt DateTime  @default(now()) @map("created_at")

  wechatBindings WechatBinding[]
  loginSessions  LoginSession[]

  @@map("users")
}

model WechatBinding {
  id            Int       @id @default(autoincrement())
  userId        Int       @map("user_id")
  openid        String    @unique @db.VarChar(50)
  subscribedAt  DateTime  @default(now()) @map("subscribed_at")
  unsubscribedAt DateTime? @map("unsubscribed_at")

  user User @relation(fields: [userId], references: [id], onDelete: Cascade)

  @@map("wechat_bindings")
}

model LoginSession {
  sessionId String   @id @map("session_id") @db.VarChar(36)
  status    String   @default("pending") @db.VarChar(20)
  userId    Int?     @map("user_id")
  createdAt DateTime @default(now()) @map("created_at")
  expiresAt DateTime @map("expires_at")

  user User? @relation(fields: [userId], references: [id], onDelete: SetNull)

  @@index([status])
  @@index([expiresAt])
  @@map("login_sessions")
}
```

**Step 2: 运行 Prisma migrate 创建表**

```bash
cd backend && npx prisma migrate dev --name add_auth_tables
```

Expected: 成功创建 users、wechat_bindings、login_sessions 表

**Step 3: 验证表结构**

```bash
docker exec stock-mysql mysql -uroot -pstock_password -e "DESCRIBE stock_db.users; DESCRIBE stock_db.wechat_bindings; DESCRIBE stock_db.login_sessions;"
```

Expected: 显示三个表的结构

**Step 4: Commit**

```bash
git add backend/prisma/schema.prisma backend/prisma/migrations/
git commit -m "feat(db): add users, wechat_bindings, login_sessions tables"
```

---

## Task 2: 微信服务封装

**Files:**
- Create: `nodejs/src/services/wechatService.js`

**Step 1: 创建 wechatService.js**

```javascript
const axios = require('axios');
const logger = require('../logger');

const WECHAT_APP_ID = process.env.WECHAT_APP_ID;
const WECHAT_APP_SECRET = process.env.WECHAT_APP_SECRET;

// access_token 缓存
let accessTokenCache = {
  token: null,
  expiresAt: 0
};

/**
 * 获取 access_token（带缓存）
 */
async function getAccessToken() {
  // 如果缓存有效，直接返回
  if (accessTokenCache.token && accessTokenCache.expiresAt > Date.now()) {
    return accessTokenCache.token;
  }

  try {
    const response = await axios.get(
      `https://api.weixin.qq.com/cgi-bin/token?grant_type=client_credential&appid=${WECHAT_APP_ID}&secret=${WECHAT_APP_SECRET}`
    );

    if (response.data.access_token) {
      accessTokenCache.token = response.data.access_token;
      // 提前 5 分钟过期，避免临界情况
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

/**
 * 生成带参数临时二维码
 * @param {string} sceneId - 场景值（session_id）
 * @param {number} expiresIn - 过期时间（秒），默认 300
 */
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

/**
 * 验证微信消息签名
 */
function verifySignature(signature, timestamp, nonce, token) {
  const crypto = require('crypto');
  const arr = [token, timestamp, nonce].sort();
  const sha1 = crypto.createHash('sha1').update(arr.join('')).digest('hex');
  return sha1 === signature;
}

module.exports = {
  getAccessToken,
  createQRCode,
  verifySignature
};
```

**Step 2: 添加环境变量配置示例**

修改 `env/.env.example`，添加：

```env
# 微信公众号配置
WECHAT_APP_ID=wxxxxxxxxxxx
WECHAT_APP_SECRET=xxxxxxxxxxxxxxxx
WECHAT_TOKEN=your_token

# JWT 配置
JWT_SECRET=your_jwt_secret_key_here
JWT_EXPIRES_IN=7d
```

**Step 3: Commit**

```bash
git add nodejs/src/services/wechatService.js env/.env.example
git commit -m "feat(wechat): add wechatService for QR code generation"
```

---

## Task 3: 认证服务封装

**Files:**
- Create: `nodejs/src/services/authService.js`

**Step 1: 安装 JWT 依赖**

```bash
cd nodejs && npm install jsonwebtoken uuid
```

Expected: package.json 中添加 jsonwebtoken 和 uuid 依赖

**Step 2: 创建 authService.js**

```javascript
const jwt = require('jsonwebtoken');
const { v4: uuidv4 } = require('uuid');
const config = require('../config');
const logger = require('../logger');

const JWT_SECRET = process.env.JWT_SECRET || 'default_secret_change_this';
const JWT_EXPIRES_IN = process.env.JWT_EXPIRES_IN || '7d';
const SESSION_EXPIRES_SECONDS = 300; // 5 分钟

// 数据库连接（复用现有逻辑）
let dbType = config.dbType;
let mysqlPool = null;
let sqliteDb = null;

async function initDb() {
  if (dbType === 'mysql' && config.databaseUrl) {
    const mysql = require('mysql2/promise');
    mysqlPool = mysql.createPool(config.databaseUrl);
  } else {
    const Database = require('better-sqlite3');
    const dbPath = config.dbPath || './stock.db';
    sqliteDb = new Database(dbPath);
  }
}

/**
 * 创建登录会话
 */
async function createSession() {
  if (!mysqlPool && !sqliteDb) await initDb();

  const sessionId = uuidv4();
  const expiresAt = new Date(Date.now() + SESSION_EXPIRES_SECONDS * 1000);

  if (dbType === 'mysql') {
    await mysqlPool.execute(
      'INSERT INTO login_sessions (session_id, status, expires_at) VALUES (?, ?, ?)',
      [sessionId, 'pending', expiresAt]
    );
  } else {
    sqliteDb.prepare(
      'INSERT INTO login_sessions (session_id, status, expires_at) VALUES (?, ?, ?)'
    ).run(sessionId, 'pending', expiresAt.toISOString());
  }

  logger.info('创建登录会话: ' + sessionId);
  return {
    sessionId,
    expiresAt
  };
}

/**
 * 查询会话状态
 */
async function getSessionStatus(sessionId) {
  if (!mysqlPool && !sqliteDb) await initDb();

  if (dbType === 'mysql') {
    const [rows] = await mysqlPool.execute(
      'SELECT session_id, status, user_id, expires_at FROM login_sessions WHERE session_id = ?',
      [sessionId]
    );
    return rows[0] || null;
  } else {
    return sqliteDb.prepare(
      'SELECT session_id, status, user_id, expires_at FROM login_sessions WHERE session_id = ?'
    ).get(sessionId);
  }
}

/**
 * 完成登录（绑定用户）
 */
async function completeLogin(sessionId, userId) {
  if (!mysqlPool && !sqliteDb) await initDb();

  if (dbType === 'mysql') {
    await mysqlPool.execute(
      'UPDATE login_sessions SET status = ?, user_id = ? WHERE session_id = ?',
      ['success', userId, sessionId]
    );
  } else {
    sqliteDb.prepare(
      'UPDATE login_sessions SET status = ?, user_id = ? WHERE session_id = ?'
    ).run('success', userId, sessionId);
  }

  logger.info('登录完成: sessionId=' + sessionId + ', userId=' + userId);
}

/**
 * 生成 JWT token
 */
function generateToken(userId) {
  return jwt.sign(
    { userId, iat: Date.now() },
    JWT_SECRET,
    { expiresIn: JWT_EXPIRES_IN }
  );
}

/**
 * 验证 JWT token
 */
function verifyToken(token) {
  try {
    return jwt.verify(token, JWT_SECRET);
  } catch (error) {
    return null;
  }
}

/**
 * 创建用户并绑定微信 openid
 */
async function createUserWithWechatBinding(openid) {
  if (!mysqlPool && !sqliteDb) await initDb();

  // 先检查是否已绑定
  if (dbType === 'mysql') {
    const [existing] = await mysqlPool.execute(
      'SELECT user_id FROM wechat_bindings WHERE openid = ?',
      [openid]
    );
    if (existing[0]) {
      return existing[0].user_id;
    }

    // 创建新用户
    const [userResult] = await mysqlPool.execute(
      'INSERT INTO users (created_at) VALUES (NOW())'
    );
    const userId = userResult.insertId;

    // 创建绑定
    await mysqlPool.execute(
      'INSERT INTO wechat_bindings (user_id, openid, subscribed_at) VALUES (?, ?, NOW())',
      [userId, openid]
    );

    logger.info('创建用户并绑定微信: userId=' + userId + ', openid=' + openid);
    return userId;
  } else {
    const existing = sqliteDb.prepare(
      'SELECT user_id FROM wechat_bindings WHERE openid = ?'
    ).get(openid);
    if (existing) {
      return existing.user_id;
    }

    const userResult = sqliteDb.prepare(
      'INSERT INTO users (created_at) VALUES (?)'
    ).run(new Date().toISOString());
    const userId = userResult.lastInsertRowid;

    sqliteDb.prepare(
      'INSERT INTO wechat_bindings (user_id, openid, subscribed_at) VALUES (?, ?, ?)'
    ).run(userId, openid, new Date().toISOString());

    logger.info('创建用户并绑定微信: userId=' + userId + ', openid=' + openid);
    return userId;
  }
}

module.exports = {
  createSession,
  getSessionStatus,
  completeLogin,
  generateToken,
  verifyToken,
  createUserWithWechatBinding
};
```

**Step 3: Commit**

```bash
git add nodejs/src/services/authService.js nodejs/package.json nodejs/package-lock.json
git commit -m "feat(auth): add authService for session and JWT management"
```

---

## Task 4: 认证路由

**Files:**
- Create: `nodejs/src/routes/auth.js`
- Modify: `nodejs/src/index.js`（引入路由）

**Step 1: 创建 auth.js 路由**

```javascript
const express = require('express');
const router = express.Router();
const wechatService = require('../services/wechatService');
const authService = require('../services/authService');
const logger = require('../logger');

/**
 * GET /api/auth/qrcode
 * 生成带参数二维码
 */
router.get('/qrcode', async (req, res) => {
  try {
    // 创建登录会话
    const session = await authService.createSession();

    // 生成微信二维码
    const qrCode = await wechatService.createQRCode(session.sessionId, 300);

    res.json({
      success: true,
      data: {
        session_id: session.sessionId,
        qr_url: qrCode.qrUrl,
        expires_in: qrCode.expiresIn
      }
    });
  } catch (error) {
    logger.error('生成二维码失败: ' + error.message);
    res.status(500).json({
      success: false,
      error: '生成二维码失败'
    });
  }
});

/**
 * GET /api/auth/status
 * 轮询登录状态
 */
router.get('/status', async (req, res) => {
  try {
    const { session_id } = req.query;

    if (!session_id) {
      return res.status(400).json({
        success: false,
        error: '缺少 session_id 参数'
      });
    }

    const session = await authService.getSessionStatus(session_id);

    if (!session) {
      return res.status(404).json({
        success: false,
        error: '会话不存在'
      });
    }

    // 检查是否过期
    const expiresAt = new Date(session.expires_at);
    if (expiresAt < new Date()) {
      return res.json({
        success: true,
        data: {
          status: 'expired'
        }
      });
    }

    res.json({
      success: true,
      data: {
        status: session.status,
        user_id: session.user_id || null
      }
    });
  } catch (error) {
    logger.error('查询状态失败: ' + error.message);
    res.status(500).json({
      success: false,
      error: '查询状态失败'
    });
  }
});

/**
 * POST /api/auth/token
 * 登录成功后获取 JWT token
 */
router.post('/token', async (req, res) => {
  try {
    const { session_id } = req.body;

    if (!session_id) {
      return res.status(400).json({
        success: false,
        error: '缺少 session_id 参数'
      });
    }

    const session = await authService.getSessionStatus(session_id);

    if (!session || session.status !== 'success') {
      return res.status(400).json({
        success: false,
        error: '登录未完成或会话不存在'
      });
    }

    // 检查是否过期
    const expiresAt = new Date(session.expires_at);
    if (expiresAt < new Date()) {
      return res.status(400).json({
        success: false,
        error: '会话已过期'
      });
    }

    // 生成 JWT token
    const token = authService.generateToken(session.user_id);

    // 清理会话（可选）
    // await authService.deleteSession(session_id);

    res.json({
      success: true,
      data: {
        token: token,
        expires_in: 7 * 24 * 60 * 60 // 7 天
      }
    });
  } catch (error) {
    logger.error('获取 token 失败: ' + error.message);
    res.status(500).json({
      success: false,
      error: '获取 token 失败'
    });
  }
});

module.exports = router;
```

**Step 2: 在 index.js 中引入路由**

修改 `nodejs/src/index.js`，在现有路由引入部分添加：

```javascript
// 在其他路由引入之后添加
const authRoutes = require('./routes/auth');
app.use('/api/auth', authRoutes);
```

**Step 3: Commit**

```bash
git add nodejs/src/routes/auth.js nodejs/src/index.js
git commit -m "feat(auth): add auth routes for QR code login"
```

---

## Task 5: 微信事件处理

**Files:**
- Create: `nodejs/src/webhook/wechatEventHandler.js`
- Modify: `nodejs/src/index.js`（引入 webhook）

**Step 1: 创建 wechatEventHandler.js**

```javascript
const xml2js = require('xml2js');
const authService = require('../services/authService');
const wechatService = require('../services/wechatService');
const logger = require('../logger');

const WECHAT_TOKEN = process.env.WECHAT_TOKEN || '';

const xmlParser = new xml2js.Parser({ explicitArray: false });
const xmlBuilder = new xml2js.Builder({ rootName: 'xml', headless: true });

/**
 * 处理微信事件推送
 */
async function handleWechatEvent(body, signature, timestamp, nonce) {
  // 验证签名
  if (!wechatService.verifySignature(signature, timestamp, nonce, WECHAT_TOKEN)) {
    logger.error('微信签名验证失败');
    throw new Error('签名验证失败');
  }

  // 解析 XML
  const parsed = await xmlParser.parseStringPromise(body);
  const message = parsed.xml;

  logger.info('收到微信事件: ' + message.MsgType + '/' + message.Event);

  // 处理关注事件
  if (message.MsgType === 'event' && message.Event === 'subscribe') {
    return await handleSubscribeEvent(message);
  }

  // 处理已关注用户的扫码事件
  if (message.MsgType === 'event' && message.Event === 'SCAN') {
    return await handleScanEvent(message);
  }

  // 默认返回 success
  return buildTextResponse(message.ToUserName, message.FromUserName, 'success');
}

/**
 * 处理关注事件（新用户扫码关注）
 */
async function handleSubscribeEvent(message) {
  const openid = message.FromUserName;
  // EventKey 格式: qrscene_<session_id>
  const eventKey = message.EventKey || '';
  const sessionId = eventKey.replace('qrscene_', '');

  logger.info('用户关注公众号: openid=' + openid + ', sessionId=' + sessionId);

  // 创建用户并绑定
  const userId = await authService.createUserWithWechatBinding(openid);

  // 完成登录
  if (sessionId) {
    await authService.completeLogin(sessionId, userId);
  }

  // 返回欢迎消息
  return buildTextResponse(
    message.ToUserName,
    message.FromUserName,
    '欢迎关注股票分析公众号！您已成功登录系统。'
  );
}

/**
 * 处理扫码事件（已关注用户扫码）
 */
async function handleScanEvent(message) {
  const openid = message.FromUserName;
  const sessionId = message.EventKey || '';

  logger.info('已关注用户扫码: openid=' + openid + ', sessionId=' + sessionId);

  // 查找绑定记录获取 userId
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

/**
 * 构建文本消息响应
 */
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
```

**Step 2: 在 authService.js 中添加 getUserIdByOpenid 方法**

在 `authService.js` 末尾的 module.exports 之前添加：

```javascript
/**
 * 根据 openid 获取 userId
 */
async function getUserIdByOpenid(openid) {
  if (!mysqlPool && !sqliteDb) await initDb();

  if (dbType === 'mysql') {
    const [rows] = await mysqlPool.execute(
      'SELECT user_id FROM wechat_bindings WHERE openid = ?',
      [openid]
    );
    return rows[0]?.user_id || null;
  } else {
    const row = sqliteDb.prepare(
      'SELECT user_id FROM wechat_bindings WHERE openid = ?'
    ).get(openid);
    return row?.user_id || null;
  }
}
```

并更新 module.exports：

```javascript
module.exports = {
  createSession,
  getSessionStatus,
  completeLogin,
  generateToken,
  verifyToken,
  createUserWithWechatBinding,
  getUserIdByOpenid
};
```

**Step 3: 在 index.js 中引入 webhook**

修改 `nodejs/src/index.js`，添加：

```javascript
const xml2js = require('xml2js');
const wechatEventHandler = require('./webhook/wechatEventHandler');

// 微信事件推送 webhook
app.post('/api/wechat/event', async (req, res) => {
  try {
    const { signature, timestamp, nonce } = req.query;
    const body = req.body;

    // 微信推送的是 XML，需要解析
    const response = await wechatEventHandler.handleWechatEvent(
      body.toString(),
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

// 微信服务器验证（GET 请求）
app.get('/api/wechat/event', (req, res) => {
  const { signature, timestamp, nonce, echostr } = req.query;
  
  if (wechatService.verifySignature(signature, timestamp, nonce, process.env.WECHAT_TOKEN)) {
    res.send(echostr);
  } else {
    res.status(403).send('invalid signature');
  }
});
```

**Step 4: 安装 xml2js**

```bash
cd nodejs && npm install xml2js
```

**Step 5: Commit**

```bash
git add nodejs/src/webhook/wechatEventHandler.js nodejs/src/services/authService.js nodejs/src/index.js nodejs/package.json nodejs/package-lock.json
git commit -m "feat(wechat): add webhook handler for subscribe/scan events"
```

---

## Task 6: 前端登录页面

**Files:**
- Create: `web-frontend/src/pages/Login/index.tsx`
- Modify: `web-frontend/src/App.tsx`（添加路由）

**Step 1: 安装路由依赖**

```bash
cd web-frontend && npm install react-router-dom
```

**Step 2: 创建 Login 页面**

创建 `web-frontend/src/pages/Login/index.tsx`：

```typescript
import { useState, useEffect, useCallback } from 'react';
import { useNavigate } from 'react-router-dom';
import { authApi } from '../../services/authApi';
import { tokenStorage } from '../../utils/tokenStorage';

export default function LoginPage() {
  const navigate = useNavigate();
  const [qrUrl, setQrUrl] = useState<string>('');
  const [sessionId, setSessionId] = useState<string>('');
  const [status, setStatus] = useState<'loading' | 'pending' | 'success' | 'expired' | 'error'>('loading');
  const [countdown, setCountdown] = useState<number>(300);

  // 生成二维码
  const generateQRCode = useCallback(async () => {
    setStatus('loading');
    try {
      const result = await authApi.getQRCode();
      setQrUrl(result.qr_url);
      setSessionId(result.session_id);
      setCountdown(result.expires_in);
      setStatus('pending');
    } catch (error) {
      console.error('生成二维码失败:', error);
      setStatus('error');
    }
  }, []);

  // 轮询登录状态
  useEffect(() => {
    if (status !== 'pending' || !sessionId) return;

    const interval = setInterval(async () => {
      try {
        const result = await authApi.getStatus(sessionId);
        
        if (result.status === 'success') {
          setStatus('success');
          clearInterval(interval);
          
          // 获取 token
          const tokenResult = await authApi.getToken(sessionId);
          tokenStorage.save(tokenResult.token);
          
          // 跳转到分析页
          navigate('/analysis');
        } else if (result.status === 'expired') {
          setStatus('expired');
          clearInterval(interval);
        }
      } catch (error) {
        console.error('轮询状态失败:', error);
      }
    }, 2000);

    return () => clearInterval(interval);
  }, [status, sessionId, navigate]);

  // 倒计时
  useEffect(() => {
    if (status !== 'pending') return;

    const interval = setInterval(() => {
      setCountdown(prev => {
        if (prev <= 1) {
          setStatus('expired');
          return 0;
        }
        return prev - 1;
      });
    }, 1000);

    return () => clearInterval(interval);
  }, [status]);

  // 初始化生成二维码
  useEffect(() => {
    generateQRCode();
  }, [generateQRCode]);

  return (
    <div className="min-h-screen bg-base flex items-center justify-center">
      <div className="bg-surface rounded-xl border border-border-default p-8 max-w-md w-full text-center">
        <h1 className="text-2xl font-semibold text-text-primary mb-2">
          微信扫码登录
        </h1>
        <p className="text-text-tertiary mb-6">
          扫码关注公众号即可登录
        </p>

        {status === 'loading' && (
          <div className="text-text-tertiary">正在生成二维码...</div>
        )}

        {status === 'pending' && qrUrl && (
          <>
            <img 
              src={qrUrl} 
              alt="微信登录二维码" 
              className="w-64 h-64 mx-auto mb-4 rounded-lg"
            />
            <p className="text-text-tertiary text-sm">
              请使用微信扫描二维码
            </p>
            <p className="text-accent-amber text-sm mt-2">
              二维码有效期: {Math.floor(countdown / 60)}:{(countdown % 60).toString().padStart(2, '0')}
            </p>
          </>
        )}

        {status === 'success' && (
          <div className="text-signal-sell">
            <span className="text-4xl mb-4">✓</span>
            <p>登录成功，正在跳转...</p>
          </div>
        )}

        {status === 'expired' && (
          <>
            <p className="text-signal-buy mb-4">二维码已过期</p>
            <button 
              onClick={generateQRCode}
              className="btn-primary"
            >
              刷新二维码
            </button>
          </>
        )}

        {status === 'error' && (
          <>
            <p className="text-signal-buy mb-4">生成二维码失败</p>
            <button 
              onClick={generateQRCode}
              className="btn-primary"
            >
              重试
            </button>
          </>
        )}
      </div>
    </div>
  );
}
```

**Step 3: 创建 tokenStorage.ts**

创建 `web-frontend/src/utils/tokenStorage.ts`：

```typescript
const TOKEN_KEY = 'auth_token';

export const tokenStorage = {
  save: (token: string) => {
    localStorage.setItem(TOKEN_KEY, token);
  },

  get: (): string | null => {
    return localStorage.getItem(TOKEN_KEY);
  },

  remove: () => {
    localStorage.removeItem(TOKEN_KEY);
  },

  exists: (): boolean => {
    return !!localStorage.getItem(TOKEN_KEY);
  }
};
```

**Step 4: 创建 authApi.ts**

创建 `web-frontend/src/services/authApi.ts`：

```typescript
import axios from 'axios';

const api = axios.create({
  baseURL: '/api',
  timeout: 30000,
});

export const authApi = {
  getQRCode: async (): Promise<{ session_id: string; qr_url: string; expires_in: number }> => {
    const response = await api.get('/auth/qrcode');
    return response.data.data;
  },

  getStatus: async (sessionId: string): Promise<{ status: string; user_id?: number }> => {
    const response = await api.get('/auth/status', { params: { session_id: sessionId } });
    return response.data.data;
  },

  getToken: async (sessionId: string): Promise<{ token: string; expires_in: number }> => {
    const response = await api.post('/auth/token', { session_id: sessionId });
    return response.data.data;
  },
};
```

**Step 5: Commit**

```bash
git add web-frontend/src/pages/Login/index.tsx web-frontend/src/utils/tokenStorage.ts web-frontend/src/services/authApi.ts web-frontend/package.json web-frontend/package-lock.json
git commit -m "feat(frontend): add Login page with QR code display and polling"
```

---

## Task 7: 前端路由配置

**Files:**
- Modify: `web-frontend/src/App.tsx`
- Modify: `web-frontend/src/main.tsx`（如果需要）

**Step 1: 更新 App.tsx 添加路由**

```typescript
import { BrowserRouter, Routes, Route, Navigate } from 'react-router-dom';
import AnalysisPage from './pages/Analysis';
import LoginPage from './pages/Login';
import { tokenStorage } from './utils/tokenStorage';

function App() {
  const isLoggedIn = tokenStorage.exists();

  return (
    <BrowserRouter>
      <Routes>
        <Route 
          path="/login" 
          element={isLoggedIn ? <Navigate to="/analysis" /> : <LoginPage />} 
        />
        <Route 
          path="/analysis" 
          element={isLoggedIn ? <AnalysisPage /> : <Navigate to="/login" />} 
        />
        <Route 
          path="/" 
          element={<Navigate to={isLoggedIn ? "/analysis" : "/login"} />} 
        />
      </Routes>
    </BrowserRouter>
  );
}

export default App;
```

**Step 2: Commit**

```bash
git add web-frontend/src/App.tsx
git commit -m "feat(frontend): add routing with auth guard"
```

---

## Task 8: 整合与测试

**Step 1: 重启 Node.js 服务**

```bash
docker-compose restart stock-api-server stock-feishu-bot
```

**Step 2: 重启前端服务**

```bash
docker-compose restart stock-web-frontend
```

**Step 3: 手动测试流程**

1. 访问 `http://localhost` 应跳转到 `/login`
2. 页面显示二维码
3. 使用微信扫码关注公众号
4. 前端自动跳转到 `/analysis`

**Step 4: 检查日志**

```bash
docker logs stock-api-server --tail 50
```

Expected: 看到 "生成二维码成功"、"用户关注公众号"、"登录完成" 等日志

---

## 完成检查清单

- [ ] 数据库表已创建
- [ ] 后端二维码生成接口可用
- [ ] 后端轮询状态接口可用
- [ ] 后端微信事件处理可用
- [ ] 前端登录页面显示二维码
- [ ] 扫码后能自动跳转
- [ ] token 存储在 localStorage
- [ ] 未登录用户无法访问分析页