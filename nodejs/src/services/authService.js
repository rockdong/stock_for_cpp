const jwt = require('jsonwebtoken');
const { v4: uuidv4 } = require('uuid');
const config = require('../config');
const logger = require('../logger');

const JWT_SECRET = process.env.JWT_SECRET || 'default_secret_change_this';
const JWT_EXPIRES_IN = process.env.JWT_EXPIRES_IN || '7d';
const SESSION_EXPIRES_SECONDS = 300;

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

function generateToken(userId) {
  return jwt.sign(
    { userId, iat: Date.now() },
    JWT_SECRET,
    { expiresIn: JWT_EXPIRES_IN }
  );
}

function verifyToken(token) {
  try {
    return jwt.verify(token, JWT_SECRET);
  } catch (error) {
    return null;
  }
}

async function createUserWithWechatBinding(openid) {
  if (!mysqlPool && !sqliteDb) await initDb();

  if (dbType === 'mysql') {
    const [existing] = await mysqlPool.execute(
      'SELECT user_id FROM wechat_bindings WHERE openid = ?',
      [openid]
    );
    if (existing[0]) {
      return existing[0].user_id;
    }

    const [userResult] = await mysqlPool.execute(
      'INSERT INTO users (created_at) VALUES (NOW())'
    );
    const userId = userResult.insertId;

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

module.exports = {
  createSession,
  getSessionStatus,
  completeLogin,
  generateToken,
  verifyToken,
  createUserWithWechatBinding,
  getUserIdByOpenid
};