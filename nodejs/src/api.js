// api.js - REST API 路由（供微信小程序调用）

const express = require('express');
const router = express.Router();
const Database = require('better-sqlite3');
const path = require('path');
const logger = require('./logger');

const dbPath = process.env.DB_PATH || '/app/stock.db';
let db;

function getDb() {
  if (!db) {
    db = new Database(dbPath);
  }
  return db;
}

router.get('/health', (req, res) => {
  res.json({ status: 'ok', timestamp: new Date().toISOString() });
});

router.get('/stocks', (req, res) => {
  try {
    const db = getDb();
    const stocks = db.prepare(`
      SELECT ts_code, name, industry, market 
      FROM stocks 
      LIMIT 100
    `).all();
    res.json({ success: true, data: stocks });
  } catch (err) {
    logger.error('获取股票列表失败:', err);
    res.status(500).json({ success: false, error: err.message });
  }
});

router.get('/stocks/search', (req, res) => {
  try {
    const { keyword } = req.query;
    if (!keyword) {
      return res.json({ success: true, data: [] });
    }
    
    const db = getDb();
    const stocks = db.prepare(`
      SELECT ts_code, name, industry, market 
      FROM stocks 
      WHERE name LIKE ? OR ts_code LIKE ?
      LIMIT 20
    `).all(`%${keyword}%`, `%${keyword}%`);
    
    res.json({ success: true, data: stocks });
  } catch (err) {
    logger.error('搜索股票失败:', err);
    res.status(500).json({ success: false, error: err.message });
  }
});

router.get('/stocks/:code', (req, res) => {
  try {
    const { code } = req.params;
    const db = getDb();
    
    const stock = db.prepare(`
      SELECT ts_code, name, industry, market 
      FROM stocks 
      WHERE ts_code = ?
    `).get(code);
    
    if (!stock) {
      return res.status(404).json({ success: false, error: '股票不存在' });
    }
    
    const latestPrice = db.prepare(`
      SELECT close, high, low, open, volume, time
      FROM chart_data
      WHERE ts_code = ?
      ORDER BY time DESC
      LIMIT 1
    `).get(code);
    
    const result = {
      ...stock,
      price: latestPrice?.close || null,
      high: latestPrice?.high || null,
      low: latestPrice?.low || null,
      volume: latestPrice?.volume || null
    };
    
    res.json({ success: true, data: result });
  } catch (err) {
    logger.error('获取股票详情失败:', err);
    res.status(500).json({ success: false, error: err.message });
  }
});

// ============================================
// analysis/* 路由 - 注意：具体路径必须在 :code 之前
// ============================================

router.get('/analysis/signals', (req, res) => {
  try {
    const db = getDb();
    const { label, limit = 50 } = req.query;
    
    let sql = `
      SELECT a.ts_code, s.name, a.strategy_name, a.label, a.freq, a.trade_date
      FROM analysis_results a
      LEFT JOIN stocks s ON a.ts_code = s.ts_code
    `;
    
    const params = [];
    if (label) {
      sql += ' WHERE a.label = ?';
      params.push(label.toUpperCase());
    }
    
    sql += ' ORDER BY a.trade_date DESC LIMIT ?';
    params.push(parseInt(limit));
    
    const results = db.prepare(sql).all(...params);
    res.json({ success: true, data: results });
  } catch (err) {
    logger.error('获取分析信号失败:', err);
    res.status(500).json({ success: false, error: err.message });
  }
});

router.get('/analysis/progress', (req, res) => {
  try {
    const db = getDb();
    const progress = db.prepare(`
      SELECT * FROM analysis_progress ORDER BY id DESC LIMIT 1
    `).get();
    
    const defaultProgress = {
      id: 1,
      total: 0,
      completed: 0,
      failed: 0,
      status: 'idle',
      started_at: null,
      updated_at: new Date().toISOString()
    };
    
    res.json({ 
      success: true, 
      data: progress || defaultProgress
    });
  } catch (err) {
    logger.error('获取分析进度失败:', err);
    res.status(500).json({ success: false, error: err.message });
  }
});

// analysis/process/* 路由组 - 必须在 /analysis/:code 之前

router.get('/analysis/process', (req, res) => {
  try {
    const db = getDb();
    const { ts_code, start_date, end_date, signal, strategy, limit = 100 } = req.query;
    
    let sql = `
      SELECT id, ts_code, stock_name, trade_date, data, created_at, expires_at
      FROM analysis_process_records
      WHERE 1=1
    `;
    const params = [];
    
    if (ts_code) {
      sql += ' AND ts_code = ?';
      params.push(ts_code);
    }
    if (start_date) {
      sql += ' AND trade_date >= ?';
      params.push(start_date);
    }
    if (end_date) {
      sql += ' AND trade_date <= ?';
      params.push(end_date);
    }
    
    sql += ' ORDER BY created_at DESC LIMIT ?';
    params.push(parseInt(limit));
    
    const records = db.prepare(sql).all(...params);
    
    let parsedRecords = records.map(record => ({
      ...record,
      data: record.data ? JSON.parse(record.data) : { strategies: [] }
    }));
    
    if (strategy) {
      parsedRecords = parsedRecords.filter(record => {
        const strategies = record.data?.strategies || [];
        return strategies.some(s => s.name === strategy);
      });
    }
    
    if (signal) {
      parsedRecords = parsedRecords.filter(record => {
        const strategies = record.data?.strategies || [];
        return strategies.some(s => s.freqs.some(f => f.signal === signal));
      });
    }
    
    res.json({ success: true, data: parsedRecords });
  } catch (err) {
    logger.error('获取分析过程记录失败:', err);
    res.status(500).json({ success: false, error: err.message });
  }
});

router.get('/analysis/process/strategies', (req, res) => {
    const strategies = ['EMA17TO25', 'EMA25_CROSSOVER', 'EMA17_BREAKOUT', 'EMA_CONVERGENCE', 
                        'EMA25_GREATER_17_PRICE_MATCH', 'MACD', 'RSI', 'BOLL', 'MA_CROSS', 'GRID'];
    res.json({ success: true, data: strategies });
});

router.get('/analysis/process/chart/:ts_code', (req, res) => {
  try {
    const { ts_code } = req.params;
    const { strategy, freq = 'd' } = req.query;
    const db = getDb();
    
    const record = db.prepare(`
      SELECT id, ts_code, stock_name, trade_date, data, created_at
      FROM analysis_process_records
      WHERE ts_code = ?
      ORDER BY trade_date DESC LIMIT 1
    `).get(ts_code);
    
    if (!record) {
      return res.json({ success: true, data: [] });
    }
    
    const parsedData = record.data ? JSON.parse(record.data) : { strategies: [] };
    
    // Find the requested strategy and freq
    let candles = [];
    const strategyLower = strategy ? strategy.toLowerCase() : null;
    
    if (parsedData.strategies) {
      for (const s of parsedData.strategies) {
        if (!strategyLower || s.name.toLowerCase().includes(strategyLower)) {
          for (const f of (s.freqs || [])) {
            if (f.freq === freq) {
              candles = f.candles || [];
              break;
            }
          }
        }
      }
    }
    
    res.json({ 
      success: true, 
      data: candles,
      record: {
        id: record.id,
        ts_code: record.ts_code,
        stock_name: record.stock_name,
        trade_date: record.trade_date
      }
    });
  } catch (err) {
    logger.error('获取分析图表数据失败:', err);
    res.status(500).json({ success: false, error: err.message });
  }
});

router.get('/analysis/process/:id', (req, res) => {
  try {
    const { id } = req.params;
    const db = getDb();
    
    const record = db.prepare(`
      SELECT id, ts_code, stock_name, trade_date, data, created_at, expires_at
      FROM analysis_process_records WHERE id = ?
    `).get(id);
    
    if (!record) {
      return res.status(404).json({ success: false, error: '记录不存在' });
    }
    
    res.json({ 
      success: true, 
      data: {
        ...record,
        data: record.data ? JSON.parse(record.data) : { strategies: [] }
      }
    });
  } catch (err) {
    logger.error('获取分析过程记录详情失败:', err);
    res.status(500).json({ success: false, error: err.message });
  }
});

router.post('/analysis/process', (req, res) => {
  try {
    const db = getDb();
    const record = req.body;
    
    const dataJson = typeof record.data === 'object' 
      ? JSON.stringify(record.data) 
      : (typeof record.data === 'string' ? record.data : '{"strategies":[]}');
    
    const stmt = db.prepare(`
      INSERT OR REPLACE INTO analysis_process_records 
      (ts_code, stock_name, trade_date, data)
      VALUES (?, ?, ?, ?)
    `);
    
    const result = stmt.run(
      record.ts_code,
      record.stock_name || '',
      record.trade_date,
      dataJson
    );
    
    res.json({ success: true, id: result.lastInsertRowid });
  } catch (err) {
    logger.error('写入分析过程记录失败:', err);
    res.status(500).json({ success: false, error: err.message });
  }
});

router.delete('/analysis/process/expired', (req, res) => {
  try {
    const db = getDb();
    
    const result = db.prepare(`
      DELETE FROM analysis_process_records 
      WHERE expires_at < datetime('now')
    `).run();
    
    res.json({ 
      success: true, 
      deleted: result.changes,
      message: `清理了 ${result.changes} 条过期记录` 
    });
  } catch (err) {
    logger.error('清理过期记录失败:', err);
    res.status(500).json({ success: false, error: err.message });
  }
});

// analysis/:code 必须放在所有 /analysis/xxx 路由之后

router.get('/analysis/:code', (req, res) => {
  try {
    const { code } = req.params;
    const db = getDb();
    
    const result = db.prepare(`
      SELECT a.ts_code, s.name, a.strategy_name, a.label, a.freq, a.trade_date
      FROM analysis_results a
      LEFT JOIN stocks s ON a.ts_code = s.ts_code
      WHERE a.ts_code = ?
      ORDER BY a.trade_date DESC
      LIMIT 1
    `).get(code);
    
    if (!result) {
      return res.status(404).json({ success: false, error: '无分析结果' });
    }
    
    res.json({ success: true, data: result });
  } catch (err) {
    logger.error('获取单股分析失败:', err);
    res.status(500).json({ success: false, error: err.message });
  }
});

// charts 路由

router.get('/charts/:code', (req, res) => {
  try {
    const { code } = req.params;
    const { freq = 'd', limit = 100 } = req.query;
    const db = getDb();
    
    const chartData = db.prepare(`
      SELECT time, open, high, low, close, volume
      FROM chart_data
      WHERE ts_code = ? AND freq = ?
      ORDER BY time DESC
      LIMIT ?
    `).all(code, freq, parseInt(limit));
    
    res.json({ success: true, data: chartData.reverse() });
  } catch (err) {
    logger.error('获取K线数据失败:', err);
    res.status(500).json({ success: false, error: err.message });
  }
});

// 用户相关 API

router.get('/user/watchlist', (req, res) => {
  res.json({ success: true, data: [] });
});

router.post('/user/watchlist', (req, res) => {
  res.json({ success: true, message: '添加成功' });
});

router.delete('/user/watchlist/:code', (req, res) => {
  res.json({ success: true, message: '删除成功' });
});

router.post('/user/subscribe', (req, res) => {
  res.json({ success: true, message: '订阅成功' });
});

router.delete('/user/subscribe/:code', (req, res) => {
  res.json({ success: true, message: '取消成功' });
});

module.exports = router;