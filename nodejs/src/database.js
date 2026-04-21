const path = require('path');
const config = require('./config');
const logger = require('./logger');

let dbType = config.dbType;
let sqliteDb = null;
let mysqlPool = null;

// 根据数据库类型初始化
async function initDatabase() {
  if (dbType === 'mysql' && config.databaseUrl) {
    return await initMySQL();
  } else {
    return initSQLite();
  }
}

// SQLite 初始化
function initSQLite() {
  if (sqliteDb) {
    return sqliteDb;
  }
  
  try {
    const Database = require('better-sqlite3');
    const dbPath = path.resolve(__dirname, '..', config.dbPath);
    logger.info('连接 SQLite 数据库: ' + dbPath);
    
    sqliteDb = new Database(dbPath, { readonly: true });
    logger.info('SQLite 数据库连接成功');
    return sqliteDb;
  } catch (error) {
    logger.error('SQLite 数据库连接失败: ' + error.message);
    return null;
  }
}

// MySQL 初始化
async function initMySQL() {
  if (mysqlPool) {
    return mysqlPool;
  }
  
  try {
    const mysql = require('mysql2/promise');
    logger.info('连接 MySQL 数据库: ' + config.databaseUrl);
    
    mysqlPool = mysql.createPool(config.databaseUrl);
    
    // 测试连接
    const conn = await mysqlPool.getConnection();
    await conn.ping();
    conn.release();
    
    logger.info('MySQL 数据库连接成功');
    return mysqlPool;
  } catch (error) {
    logger.error('MySQL 数据库连接失败: ' + error.message);
    return null;
  }
}

// 查询股票
async function findStockByTsCode(tsCode) {
  if (dbType === 'mysql') {
    if (!mysqlPool) await initMySQL();
    if (!mysqlPool) return null;
    
    try {
      const symbol = tsCode.replace('.SZ', '').replace('.SH', '');
      const [rows] = await mysqlPool.execute(`
        SELECT ts_code, symbol, name, area, industry, market, exchange, list_status, list_date
        FROM stocks 
        WHERE ts_code = ? OR symbol = ?
        LIMIT 1
      `, [tsCode, symbol]);
      return rows[0] || null;
    } catch (error) {
      logger.error('查询股票失败: ' + error.message);
      return null;
    }
  } else {
    if (!sqliteDb) initSQLite();
    if (!sqliteDb) return null;
    
    try {
      const stmt = sqliteDb.prepare(`
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
}

// 查询股票列表
async function findAllStocks(limit = 20) {
  if (dbType === 'mysql') {
    if (!mysqlPool) await initMySQL();
    if (!mysqlPool) return [];
    
    try {
      const [rows] = await mysqlPool.execute(`
        SELECT ts_code, symbol, name, area, industry, market, exchange, list_status
        FROM stocks 
        ORDER BY ts_code
        LIMIT ?
      `, [limit]);
      return rows;
    } catch (error) {
      logger.error('查询股票列表失败: ' + error.message);
      return [];
    }
  } else {
    if (!sqliteDb) initSQLite();
    if (!sqliteDb) return [];
    
    try {
      const stmt = sqliteDb.prepare(`
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
}

// 按行业查询
async function findStocksByIndustry(industry, limit = 20) {
  if (dbType === 'mysql') {
    if (!mysqlPool) await initMySQL();
    if (!mysqlPool) return [];
    
    try {
      const [rows] = await mysqlPool.execute(`
        SELECT ts_code, symbol, name, area, industry, market, exchange, list_status
        FROM stocks 
        WHERE industry LIKE ?
        ORDER BY ts_code
        LIMIT ?
      `, [`%${industry}%`, limit]);
      return rows;
    } catch (error) {
      logger.error('按行业查询股票失败: ' + error.message);
      return [];
    }
  } else {
    if (!sqliteDb) initSQLite();
    if (!sqliteDb) return [];
    
    try {
      const stmt = sqliteDb.prepare(`
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
}

// 按市场查询
async function findStocksByMarket(market, limit = 20) {
  if (dbType === 'mysql') {
    if (!mysqlPool) await initMySQL();
    if (!mysqlPool) return [];
    
    try {
      const [rows] = await mysqlPool.execute(`
        SELECT ts_code, symbol, name, area, industry, market, exchange, list_status
        FROM stocks 
        WHERE market = ?
        ORDER BY ts_code
        LIMIT ?
      `, [market, limit]);
      return rows;
    } catch (error) {
      logger.error('按市场查询股票失败: ' + error.message);
      return [];
    }
  } else {
    if (!sqliteDb) initSQLite();
    if (!sqliteDb) return [];
    
    try {
      const stmt = sqliteDb.prepare(`
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
}

// 搜索股票
async function searchStocks(keyword, limit = 10) {
  const searchPattern = `%${keyword}%`;
  
  if (dbType === 'mysql') {
    if (!mysqlPool) await initMySQL();
    if (!mysqlPool) return [];
    
    try {
      const [rows] = await mysqlPool.execute(`
        SELECT ts_code, symbol, name, area, industry, market, exchange, list_status
        FROM stocks 
        WHERE name LIKE ? OR symbol LIKE ? OR ts_code LIKE ?
        ORDER BY ts_code
        LIMIT ?
      `, [searchPattern, searchPattern, searchPattern, limit]);
      return rows;
    } catch (error) {
      logger.error('搜索股票失败: ' + error.message);
      return [];
    }
  } else {
    if (!sqliteDb) initSQLite();
    if (!sqliteDb) return [];
    
    try {
      const stmt = sqliteDb.prepare(`
        SELECT ts_code, symbol, name, area, industry, market, exchange, list_status
        FROM stocks 
        WHERE name LIKE ? OR symbol LIKE ? OR ts_code LIKE ?
        ORDER BY ts_code
        LIMIT ?
      `);
      return stmt.all(searchPattern, searchPattern, searchPattern, limit);
    } catch (error) {
      logger.error('搜索股票失败: ' + error.message);
      return [];
    }
  }
}

// 查询分析结果
async function findAnalysisResults(tsCode, strategyName = null, limit = 20) {
  if (dbType === 'mysql') {
    if (!mysqlPool) await initMySQL();
    if (!mysqlPool) return [];
    
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
      
      const [rows] = await mysqlPool.execute(sql, params);
      return rows;
    } catch (error) {
      logger.error('查询分析结果失败: ' + error.message);
      return [];
    }
  } else {
    if (!sqliteDb) initSQLite();
    if (!sqliteDb) return [];
    
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
      
      const stmt = sqliteDb.prepare(sql);
      return stmt.all(...params);
    } catch (error) {
      logger.error('查询分析结果失败: ' + error.message);
      return [];
    }
  }
}

// 查询所有分析结果
async function findAllAnalysisResults(limit = 20) {
  if (dbType === 'mysql') {
    if (!mysqlPool) await initMySQL();
    if (!mysqlPool) return [];
    
    try {
      const [rows] = await mysqlPool.execute(`
        SELECT ar.ts_code, s.name, ar.strategy_name, ar.trade_date, ar.label, ar.opt, ar.freq, ar.created_at
        FROM analysis_results ar
        LEFT JOIN stocks s ON ar.ts_code = s.ts_code
        ORDER BY ar.trade_date DESC
        LIMIT ?
      `, [limit]);
      return rows;
    } catch (error) {
      logger.error('查询分析结果列表失败: ' + error.message);
      return [];
    }
  } else {
    if (!sqliteDb) initSQLite();
    if (!sqliteDb) return [];
    
    try {
      const stmt = sqliteDb.prepare(`
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
}

// 查询最新分析结果
async function findLatestAnalysisResults() {
  if (dbType === 'mysql') {
    if (!mysqlPool) await initMySQL();
    if (!mysqlPool) return [];
    
    try {
      const [rows] = await mysqlPool.execute(`
        SELECT ar.ts_code, s.name, ar.strategy_name, ar.trade_date, ar.label, ar.opt, ar.freq, ar.created_at
        FROM analysis_results ar
        LEFT JOIN stocks s ON ar.ts_code = s.ts_code
        WHERE ar.trade_date = (SELECT MAX(trade_date) FROM analysis_results)
        ORDER BY ar.ts_code
      `);
      return rows;
    } catch (error) {
      logger.error('查询最新分析结果失败: ' + error.message);
      return [];
    }
  } else {
    if (!sqliteDb) initSQLite();
    if (!sqliteDb) return [];
    
    try {
      const stmt = sqliteDb.prepare(`
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
}

// 查询分析进度
async function getAnalysisProgress() {
  if (dbType === 'mysql') {
    if (!mysqlPool) await initMySQL();
    if (!mysqlPool) return null;
    
    try {
      const [rows] = await mysqlPool.execute(`
        SELECT total, completed, failed, status, started_at, updated_at
        FROM analysis_progress
        WHERE id = 1
      `);
      return rows[0] || null;
    } catch (error) {
      logger.error('查询分析进度失败: ' + error.message);
      return null;
    }
  } else {
    if (!sqliteDb) initSQLite();
    if (!sqliteDb) return null;
    
    try {
      const stmt = sqliteDb.prepare(`
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
}

// 查询图表数据
async function getChartData(tsCode, freq = 'd') {
  if (dbType === 'mysql') {
    if (!mysqlPool) await initMySQL();
    if (!mysqlPool) return null;
    
    try {
      const [rows] = await mysqlPool.execute(`
        SELECT ts_code, freq, analysis_date, data, updated_at
        FROM chart_data
        WHERE ts_code = ? AND freq = ?
        ORDER BY analysis_date DESC
        LIMIT 1
      `, [tsCode, freq]);
      
      const result = rows[0];
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
  } else {
    if (!sqliteDb) initSQLite();
    if (!sqliteDb) return null;
    
    try {
      const stmt = sqliteDb.prepare(`
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
}

// 关闭数据库
async function closeDatabase() {
  if (sqliteDb) {
    sqliteDb.close();
    sqliteDb = null;
    logger.info('SQLite 数据库连接已关闭');
  }
  if (mysqlPool) {
    await mysqlPool.end();
    mysqlPool = null;
    logger.info('MySQL 数据库连接已关闭');
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