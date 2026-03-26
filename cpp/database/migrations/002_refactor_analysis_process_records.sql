-- 重构 analysis_process_records 表
-- 将分散的价格数据和技术指标合并为 JSON 数组存储

-- 1. 备份旧表
ALTER TABLE analysis_process_records RENAME TO analysis_process_records_backup;

-- 2. 创建新表结构
CREATE TABLE IF NOT EXISTS analysis_process_records (
    -- 主键
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    
    -- 股票信息
    ts_code TEXT NOT NULL,
    stock_name TEXT,
    
    -- 分析信息
    strategy_name TEXT NOT NULL,
    trade_date TEXT NOT NULL,
    freq TEXT DEFAULT 'd',
    signal TEXT DEFAULT 'NONE',
    
    -- 10 个周期的数据 (JSON 数组)
    data TEXT NOT NULL,
    
    -- 时间戳
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    expires_at DATETIME,
    
    -- 唯一约束
    UNIQUE(ts_code, strategy_name, trade_date, freq)
);

-- 3. 创建索引
CREATE INDEX IF NOT EXISTS idx_process_ts_code ON analysis_process_records(ts_code);
CREATE INDEX IF NOT EXISTS idx_process_strategy ON analysis_process_records(strategy_name);
CREATE INDEX IF NOT EXISTS idx_process_date ON analysis_process_records(trade_date);
CREATE INDEX IF NOT EXISTS idx_process_signal ON analysis_process_records(signal);
CREATE INDEX IF NOT EXISTS idx_process_expires ON analysis_process_records(expires_at);

-- 4. 创建触发器：自动设置过期时间
CREATE TRIGGER IF NOT EXISTS trg_set_expires_at
AFTER INSERT ON analysis_process_records
BEGIN
    UPDATE analysis_process_records 
    SET expires_at = datetime('now', '+7 days')
    WHERE id = NEW.id AND expires_at IS NULL;
END;