-- 股票基础信息表
CREATE TABLE stocks (
    ts_code TEXT PRIMARY KEY,
    symbol TEXT NOT NULL,
    name TEXT NOT NULL,
    industry TEXT,
    market TEXT,
    list_date TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- 分析结果表
CREATE TABLE analysis_results (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    ts_code TEXT NOT NULL,
    analysis_date TEXT NOT NULL,
    strategy_name TEXT NOT NULL,
    result TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- 系统配置表
CREATE TABLE system_config (
    key TEXT PRIMARY KEY,
    value TEXT,
    description TEXT
);

-- 插入默认配置
INSERT INTO system_config VALUES ('version', '1.0.0', '数据库版本');

-- 创建索引
CREATE INDEX idx_stocks_symbol ON stocks(symbol);
CREATE INDEX idx_analysis_ts_code ON analysis_results(ts_code);