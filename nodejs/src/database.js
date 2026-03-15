const Database = require('better-sqlite3');
const path = require('path');
const config = require('./config');
const logger = require('./logger');

let db = null;

function initDatabase() {
  if (db) {
    return db;
  }
  
  try {
    const dbPath = path.resolve(__dirname, '..', config.dbPath);
    logger.info('连接数据库: ' + dbPath);
    
    db = new Database(dbPath, { readonly: true });
    logger.info('数据库连接成功');
    return db;
  } catch (error) {
    logger.error('数据库连接失败: ' + error.message);
    return null;
  }
}

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
    logger.error('查询股票失败: ' + error.message);
    return null;
  }
}

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
    logger.error('查询股票列表失败: ' + error.message);
    return [];
  }
}

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
    logger.error('按行业查询股票失败: ' + error.message);
    return [];
  }
}

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
    logger.error('按市场查询股票失败: ' + error.message);
    return [];
  }
}

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
    logger.error('搜索股票失败: ' + error.message);
    return [];
  }
}

function closeDatabase() {
  if (db) {
    db.close();
    db = null;
    logger.info('数据库连接已关闭');
  }
}

function findAnalysisResults(tsCode, strategyName = null, limit = 20) {
  if (!db) initDatabase();
  if (!db) return [];
  
  try {
    let sql = `
      SELECT ar.ts_code, s.name, ar.strategy_name, ar.trade_date, ar.label, ar.opt, ar.freq, ar.created_at
      FROM analysis_results ar
      LEFT JOIN stocks s ON ar.ts_code = s.ts_code
      WHERE ar.ts_code = ?
    `;
    
    const params = [tsCode];
    
    if (strategyName) {
      sql += ` AND ar.strategy_name = ?`;
      params.push(strategyName);
    }
    
    sql += ` ORDER BY ar.trade_date DESC LIMIT ?`;
    params.push(limit);
    
    const stmt = db.prepare(sql);
    return stmt.all(...params);
  } catch (error) {
    logger.error('查询分析结果失败: ' + error.message);
    return [];
  }
}

function findAllAnalysisResults(limit = 20) {
  if (!db) initDatabase();
  if (!db) return [];
  
  try {
    const stmt = db.prepare(`
      SELECT ar.ts_code, s.name, ar.strategy_name, ar.trade_date, ar.label, ar.opt, ar.freq, ar.created_at
      FROM analysis_results ar
      LEFT JOIN stocks s ON ar.ts_code = s.ts_code
      ORDER BY ar.trade_date DESC
      LIMIT ?
    `);
    
    return stmt.all(limit);
  } catch (error) {
    logger.error('查询分析结果列表失败: ' + error.message);
    return [];
  }
}

function findLatestAnalysisResults() {
  if (!db) initDatabase();
  if (!db) return [];
  
  try {
    const stmt = db.prepare(`
      SELECT ar.ts_code, s.name, ar.strategy_name, ar.trade_date, ar.label, ar.opt, ar.freq, ar.created_at
      FROM analysis_results ar
      LEFT JOIN stocks s ON ar.ts_code = s.ts_code
      WHERE ar.trade_date = (SELECT MAX(trade_date) FROM analysis_results)
      ORDER BY ar.ts_code
    `);
    
    return stmt.all();
  } catch (error) {
    logger.error('查询最新分析结果失败: ' + error.message);
    return [];
  }
}

function getAnalysisProgress() {
  if (!db) initDatabase();
  if (!db) return null;
  
  try {
    const stmt = db.prepare(`
      SELECT total, completed, failed, status, started_at, updated_at
      FROM analysis_progress
      WHERE id = 1
    `);
    
    return stmt.get() || null;
  } catch (error) {
    logger.error('查询分析进度失败: ' + error.message);
    return null;
  }
}

function getChartData(tsCode, freq = 'd') {
  if (!db) initDatabase();
  if (!db) return null;
  
  try {
    const stmt = db.prepare(`
      SELECT ts_code, freq, analysis_date, data, updated_at
      FROM chart_data
      WHERE ts_code = ? AND freq = ?
      ORDER BY analysis_date DESC
      LIMIT 1
    `);
    
    const result = stmt.get(tsCode, freq);
    if (!result) return null;
    
    let candles = [];
    try {
      candles = JSON.parse(result.data);
    } catch (e) {
      logger.error('解析图表数据失败: ' + e.message);
      return null;
    }
    
    return {
      ts_code: result.ts_code,
      freq: result.freq,
      analysis_date: result.analysis_date,
      candles: candles,
      updated_at: result.updated_at
    };
  } catch (error) {
    logger.error('查询图表数据失败: ' + error.message);
    return null;
  }
}

module.exports = {
  initDatabase,
  findStockByTsCode,
  findAllStocks,
  findStocksByIndustry,
  findStocksByMarket,
  searchStocks,
  findAnalysisResults,
  findAllAnalysisResults,
  findLatestAnalysisResults,
  getAnalysisProgress,
  getChartData,
  closeDatabase,
};