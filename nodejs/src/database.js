const Database = require('better-sqlite3');
const path = require('path');
const config = require('./config');

let db = null;

/**
 * 初始化数据库连接
 */
function initDatabase() {
  if (db) {
    return db;
  }
  
  try {
    // 解析数据库路径（相对于 nodejs 目录）
    const dbPath = path.resolve(__dirname, config.dbPath);
    console.log('连接数据库:', dbPath);
    
    db = new Database(dbPath, { readonly: true });
    console.log('数据库连接成功');
    return db;
  } catch (error) {
    console.error('数据库连接失败:', error.message);
    return null;
  }
}

/**
 * 根据股票代码查询股票信息
 * @param {string} tsCode - 股票代码 (如 000001.SZ)
 * @returns {object|null} 股票信息或 null
 */
function findStockByTsCode(tsCode) {
  if (!db) initDatabase();
  if (!db) return null;
  
  try {
    const stmt = db.prepare(`
      SELECT ts_code, symbol, name, area, industry, market, exchange, list_status, list_date
      FROM stocks 
      WHERE ts_code = ? OR symbol = ?
      LIMIT 1
    `);
    
    const result = stmt.get(tsCode, tsCode.replace('.SZ', '').replace('.SH', ''));
    return result || null;
  } catch (error) {
    console.error('查询股票失败:', error.message);
    return null;
  }
}

/**
 * 查询所有股票列表
 * @param {number} limit - 返回数量限制
 * @returns {Array} 股票列表
 */
function findAllStocks(limit = 20) {
  if (!db) initDatabase();
  if (!db) return [];
  
  try {
    const stmt = db.prepare(`
      SELECT ts_code, symbol, name, area, industry, market, exchange, list_status
      FROM stocks 
      ORDER BY ts_code
      LIMIT ?
    `);
    
    return stmt.all(limit);
  } catch (error) {
    console.error('查询股票列表失败:', error.message);
    return [];
  }
}

/**
 * 根据行业查询股票
 * @param {string} industry - 行业名称
 * @param {number} limit - 返回数量限制
 * @returns {Array} 股票列表
 */
function findStocksByIndustry(industry, limit = 20) {
  if (!db) initDatabase();
  if (!db) return [];
  
  try {
    const stmt = db.prepare(`
      SELECT ts_code, symbol, name, area, industry, market, exchange, list_status
      FROM stocks 
      WHERE industry LIKE ?
      ORDER BY ts_code
      LIMIT ?
    `);
    
    return stmt.all(`%${industry}%`, limit);
  } catch (error) {
    console.error('按行业查询股票失败:', error.message);
    return [];
  }
}

/**
 * 根据市场查询股票
 * @param {string} market - 市场类型
 * @param {number} limit - 返回数量限制
 * @returns {Array} 股票列表
 */
function findStocksByMarket(market, limit = 20) {
  if (!db) initDatabase();
  if (!db) return [];
  
  try {
    const stmt = db.prepare(`
      SELECT ts_code, symbol, name, area, industry, market, exchange, list_status
      FROM stocks 
      WHERE market = ?
      ORDER BY ts_code
      LIMIT ?
    `);
    
    return stmt.all(market, limit);
  } catch (error) {
    console.error('按市场查询股票失败:', error.message);
    return [];
  }
}

/**
 * 搜索股票（按名称或代码模糊查询）
 * @param {string} keyword - 关键词
 * @param {number} limit - 返回数量限制
 * @returns {Array} 股票列表
 */
function searchStocks(keyword, limit = 10) {
  if (!db) initDatabase();
  if (!db) return [];
  
  try {
    const stmt = db.prepare(`
      SELECT ts_code, symbol, name, area, industry, market, exchange, list_status
      FROM stocks 
      WHERE name LIKE ? OR symbol LIKE ? OR ts_code LIKE ?
      ORDER BY ts_code
      LIMIT ?
    `);
    
    const searchPattern = `%${keyword}%`;
    return stmt.all(searchPattern, searchPattern, searchPattern, limit);
  } catch (error) {
    console.error('搜索股票失败:', error.message);
    return [];
  }
}

/**
 * 关闭数据库连接
 */
function closeDatabase() {
  if (db) {
    db.close();
    db = null;
    console.log('数据库连接已关闭');
  }
}

module.exports = {
  initDatabase,
  findStockByTsCode,
  findAllStocks,
  findStocksByIndustry,
  findStocksByMarket,
  searchStocks,
  closeDatabase,
};