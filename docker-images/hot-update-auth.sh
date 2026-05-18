#!/bin/bash

echo "============================================"
echo "  热更新 Node.js 认证服务代码"
echo "============================================"
echo ""

echo "停止 stock-feishu-bot 容器..."
docker stop stock-feishu-bot

echo "更新 authService.js..."
docker exec stock-feishu-bot sh -c 'cat > /app/src/services/authService.js << '\''EOF'\''
const jwt = require('\''jsonwebtoken'\'');
const { v4: uuidv4 } = require('\''uuid'\'');
const config = require('\''../config'\'');
const logger = require('\''../logger'\'');
const wechatService = require('\''./wechatService'\'');

const JWT_SECRET = process.env.JWT_SECRET || '\''default_secret_change_this'\'';
const JWT_EXPIRES_IN = process.env.JWT_EXPIRES_IN || '\''7d'\'';
const SESSION_EXPIRES_SECONDS = 300;
const POLL_INTERVAL_MS = 5000;
const POLL_DURATION_MS = 60000;

const pollingTasks = new Map();

let dbType = config.dbType;
let mysqlPool = null;
let sqliteDb = null;

async function initDb() {
  if (dbType === '\''mysql'\'' && config.databaseUrl) {
    const mysql = require('\''mysql2/promise'\'');
    mysqlPool = mysql.createPool(config.databaseUrl);
    await ensureTablesExist();
  } else {
    const Database = require('\''better-sqlite3'\'');
    const dbPath = config.dbPath || '\''./stock.db'\'';
    sqliteDb = new Database(dbPath);
    await ensureTablesExist();
  }
}

async function ensureTablesExist() {
  if (dbType === '\''mysql'\'') {
    await mysqlPool.execute('\''CREATE TABLE IF NOT EXISTS users (id INT AUTO_INCREMENT PRIMARY KEY, created_at DATETIME DEFAULT CURRENT_TIMESTAMP)'\'');
    await mysqlPool.execute('\''CREATE TABLE IF NOT EXISTS wechat_bindings (id INT AUTO_INCREMENT PRIMARY KEY, user_id INT NOT NULL, openid VARCHAR(50) UNIQUE NOT NULL, subscribed_at DATETIME DEFAULT CURRENT_TIMESTAMP, unsubscribed_at DATETIME NULL, FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE)'\'');
    await mysqlPool.execute('\''CREATE TABLE IF NOT EXISTS login_sessions (session_id VARCHAR(36) PRIMARY KEY, status VARCHAR(20) DEFAULT '\''pending'\'', user_id INT NULL, created_at DATETIME DEFAULT CURRENT_TIMESTAMP, expires_at DATETIME NOT NULL, snapshot_openids TEXT NULL, FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE SET NULL, INDEX idx_status (status), INDEX idx_expires_at (expires_at))'\'');
    logger.info('\''认证相关表已检查/创建'\'');
  } else {
    sqliteDb.exec('\''CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY AUTOINCREMENT, created_at DATETIME DEFAULT CURRENT_TIMESTAMP)'\'');
    sqliteDb.exec('\''CREATE TABLE IF NOT EXISTS wechat_bindings (id INTEGER PRIMARY KEY AUTOINCREMENT, user_id INTEGER NOT NULL, openid TEXT UNIQUE NOT NULL, subscribed_at DATETIME DEFAULT CURRENT_TIMESTAMP, unsubscribed_at DATETIME NULL, FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE)'\'');
    sqliteDb.exec('\''CREATE TABLE IF NOT EXISTS login_sessions (session_id TEXT PRIMARY KEY, status TEXT DEFAULT '\''pending'\'', user_id INTEGER NULL, created_at DATETIME DEFAULT CURRENT_TIMESTAMP, expires_at DATETIME NOT NULL, snapshot_openids TEXT NULL, FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE SET NULL)'\'');
    logger.info('\''认证相关表已检查/创建'\'');
  }
}
EOF'

echo "更新 index.js..."
docker exec stock-feishu-bot sh -c 'cat > /app/src/index.js << '\''EOF'\''
const lark = require('\''@larksuiteoapi/node-sdk'\'');
const express = require('\''express'\'');
const cors = require('\''cors'\'');
const config = require('\''./config'\'');
const { handleMessage, handleCardAction } = require('\''./handler'\'');
const apiRoutes = require('\''./api'\'');
const authRoutes = require('\''./routes/auth'\'');
const logger = require('\''./logger'\'');
const requestLogger = require('\''./middleware/requestLogger'\'');
const wechatEventHandler = require('\''./webhook/wechatEventHandler'\'');
const wechatService = require('\''./services/wechatService'\'');
const authService = require('\''./services/authService'\'');

const HTTP_PORT = process.env.HTTP_PORT || 3000;

async function main() {
  logger.info('\''启动飞书机器人（WebSocket 长连接模式）...'\'');
  logger.info('\''App ID: '\'' + (config.appId ? config.appId.substring(0, 10) + '\''...'\'': '\''未配置'\''));
  
  await authService.initDb();
  
  if (!config.appId || !config.appSecret) {
    logger.error('\''飞书配置缺失！请检查 FEISHU_APP_ID 和 FEISHU_APP_SECRET 环境变量'\'');
    process.exit(1);
  }
  
  const baseConfig = {
    appId: config.appId,
    appSecret: config.appSecret,
  };
  
  const client = new lark.Client(baseConfig);
  
  const httpApp = express();
  httpApp.use(cors());
  httpApp.use(express.json());
  httpApp.use(requestLogger);
  httpApp.use('\''/api'\'', apiRoutes);
  httpApp.use('\''/api/auth'\'', authRoutes);
  
  httpApp.post('\''/api/wechat/event'\'', express.text({ type: '\''text/xml'\'' }), async (req, res) => {
    try {
      const { signature, timestamp, nonce } = req.query;
      const response = await wechatEventHandler.handleWechatEvent(req.body, signature, timestamp, nonce);
      res.set('\''Content-Type'\'', '\''text/xml'\'');
      res.send(response);
    } catch (error) {
      logger.error('\''处理微信事件失败: '\'' + error.message);
      res.status(500).send('\''error'\'');
    }
  });
  
  httpApp.get('\''/api/wechat/event'\'', (req, res) => {
    const { signature, timestamp, nonce, echostr } = req.query;
    if (wechatService.verifySignature(signature, timestamp, nonce, process.env.WECHAT_TOKEN)) {
      res.send(echostr);
    } else {
      res.status(403).send('\''invalid signature'\'');
    }
  });
  
  httpApp.listen(HTTP_PORT, () => {
    logger.info('\''HTTP API 服务已启动，端口: '\'' + HTTP_PORT);
    logger.info('\''API 文档: http://localhost:'\' + HTTP_PORT + '\''/api/stocks'\'');
  });
  
  const eventDispatcher = new lark.EventDispatcher({}).register({
    '\''im.message.receive_v1'\'': async (data) => {
      logger.info('\''收到消息事件'\'');
      logger.debug('\''消息详情: '\'' + JSON.stringify(data));
      const message = data.message;
      if (message && message.message_type === '\''text'\'') {
        await handleMessage(data);
      }
    },
    '\''card.action.trigger'\'': async (data) => {
      logger.info('\''收到卡片回调'\'');
      logger.debug('\''卡片详情: '\'' + JSON.stringify(data));
      await handleCardAction(data);
    }
  });
  
  const wsClient = new lark.WSClient({
    ...baseConfig,
    loggerLevel: lark.LoggerLevel.debug,
  });
  
  logger.info('\''正在建立 WebSocket 连接...'\'');
  
  wsClient.start({
    eventDispatcher: eventDispatcher,
  });
  
  logger.info('\''飞书机器人服务已启动，通过 WebSocket 长连接监听消息...'\'');
}

main().catch(err => {
  logger.error('\''程序异常: '\'' + err.message);
  logger.error(err.stack);
});
EOF'

echo "重启容器..."
docker start stock-feishu-bot

sleep 3

echo "测试 API..."
curl -s http://localhost:8880/api/auth/qrcode

echo ""
echo "============================================"
echo "  热更新完成！"
echo "============================================"