const jwt = require('jsonwebtoken');
const { v4: uuidv4 } = require('uuid');
const config = require('../config');
const logger = require('../logger');
const wechatService = require('./wechatService');

const JWT_SECRET = process.env.JWT_SECRET || 'default_secret_change_this';
const JWT_EXPIRES_IN = process.env.JWT_EXPIRES_IN || '7d';
const SESSION_EXPIRES_SECONDS = 300;
const POLL_INTERVAL_MS = 5000;
const POLL_DURATION_MS = 60000;

const pollingTasks = new Map();

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

async function createSessionWithSnapshot() {
  const session = await createSession();

  try {
    const followers = await wechatService.getFollowerList();
    const openids = followers.openids || [];

    if (dbType === 'mysql') {
      await mysqlPool.execute(
        'UPDATE login_sessions SET snapshot_openids = ? WHERE session_id = ?',
        [JSON.stringify(openids), session.sessionId]
      );
    } else {
      sqliteDb.prepare(
        'UPDATE login_sessions SET snapshot_openids = ? WHERE session_id = ?'
      ).run(JSON.stringify(openids), session.sessionId);
    }

    logger.info('创建会话并记录粉丝快照: ' + session.sessionId + ', 快照数量: ' + openids.length);
    startPollingTask(session.sessionId);
  } catch (error) {
    logger.error('获取粉丝快照失败: ' + error.message);
  }

  return session;
}

async function getSnapshotOpenids(sessionId) {
  if (!mysqlPool && !sqliteDb) await initDb();

  if (dbType === 'mysql') {
    const [rows] = await mysqlPool.execute(
      'SELECT snapshot_openids FROM login_sessions WHERE session_id = ?',
      [sessionId]
    );
    const snapshot = rows[0]?.snapshot_openids;
    return snapshot ? JSON.parse(snapshot) : [];
  } else {
    const row = sqliteDb.prepare(
      'SELECT snapshot_openids FROM login_sessions WHERE session_id = ?'
    ).get(sessionId);
    const snapshot = row?.snapshot_openids;
    return snapshot ? JSON.parse(snapshot) : [];
  }
}

async function pollNewFollowers(sessionId) {
  try {
    const session = await getSessionStatus(sessionId);
    if (!session || session.status !== 'pending') {
      stopPollingTask(sessionId);
      return;
    }

    const currentFollowers = await wechatService.getFollowerList();
    const currentOpenids = currentFollowers.openids || [];
    const snapshotOpenids = await getSnapshotOpenids(sessionId);

    const newOpenids = currentOpenids.filter(id => !snapshotOpenids.includes(id));

    if (newOpenids.length > 0) {
      const newOpenid = newOpenids[newOpenids.length - 1];
      const userId = await createUserWithWechatBinding(newOpenid);
      await completeLogin(sessionId, userId);
      stopPollingTask(sessionId);
      logger.info('检测到新粉丝并完成登录: openid=' + newOpenid + ', userId=' + userId);
    }
  } catch (error) {
    logger.error('轮询粉丝失败: ' + error.message);
  }
}

function startPollingTask(sessionId) {
  if (pollingTasks.has(sessionId)) {
    return;
  }

  const startTime = Date.now();

  const task = setInterval(async () => {
    if (Date.now() - startTime > POLL_DURATION_MS) {
      stopPollingTask(sessionId);
      logger.info('轮询任务超时停止: ' + sessionId);
      return;
    }

    await pollNewFollowers(sessionId);
  }, POLL_INTERVAL_MS);

  pollingTasks.set(sessionId, task);
  logger.info('启动轮询任务: ' + sessionId);
}

function stopPollingTask(sessionId) {
  const task = pollingTasks.get(sessionId);
  if (task) {
    clearInterval(task);
    pollingTasks.delete(sessionId);
    logger.info('停止轮询任务: ' + sessionId);
  }
}

module.exports = {
  createSession,
  createSessionWithSnapshot,
  getSessionStatus,
  completeLogin,
  generateToken,
  verifyToken,
  createUserWithWechatBinding,
  getUserIdByOpenid,
  startPollingTask,
  stopPollingTask
};