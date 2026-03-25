-- 创建分析过程记录表
-- 用于存储每次分析的过程数据和结果

CREATE TABLE IF NOT EXISTS analysis_process_records (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    ts_code TEXT NOT NULL,               -- 股票代码 (如 000001.SZ)
    stock_name TEXT,                     -- 股票名称
    strategy_name TEXT NOT NULL,         -- 策略名称 (如 EMA17TO25)
    trade_date TEXT NOT NULL,            -- 交易日期 (YYYY-MM-DD)
    freq TEXT DEFAULT 'd',               -- 周期 (d=日线, w=周线, m=月线)
    signal TEXT DEFAULT 'NONE',          -- 信号类型 (BUY/SELL/HOLD/NONE)
    
    -- 10 个数据字段
    time TEXT NOT NULL,                  -- 时间点
    open REAL,                           -- 开盘价
    high REAL,                           -- 最高价
    low REAL,                            -- 最低价
    close REAL,                          -- 收盘价
    volume INTEGER,                      -- 成交量
    ema17 REAL,                          -- EMA17 值
    ema25 REAL,                          -- EMA25 值
    macd REAL,                           -- MACD 值
    rsi REAL,                            -- RSI 值
    
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    expires_at DATETIME                  -- 过期时间 (创建后7天)
);

-- 创建索引以加速查询
CREATE INDEX IF NOT EXISTS idx_process_ts_code ON analysis_process_records(ts_code);
CREATE INDEX IF NOT EXISTS idx_process_strategy ON analysis_process_records(strategy_name);
CREATE INDEX IF NOT EXISTS idx_process_date ON analysis_process_records(trade_date);
CREATE INDEX IF NOT EXISTS idx_process_signal ON analysis_process_records(signal);
CREATE INDEX IF NOT EXISTS idx_process_expires ON analysis_process_records(expires_at);

-- 创建触发器：自动设置过期时间
CREATE TRIGGER IF NOT EXISTS trg_set_expires_at
AFTER INSERT ON analysis_process_records
BEGIN
    UPDATE analysis_process_records 
    SET expires_at = datetime('now', '+7 days')
    WHERE id = NEW.id AND expires_at IS NULL;
END;