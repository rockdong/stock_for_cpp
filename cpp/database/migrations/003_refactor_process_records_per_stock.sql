-- Migration: Refactor analysis_process_records to one record per stock
-- Date: 2026-03-27

-- Drop the old table
DROP TABLE IF EXISTS analysis_process_records;

-- Create the new table with simplified schema
CREATE TABLE analysis_process_records (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    ts_code TEXT NOT NULL,
    stock_name TEXT,
    trade_date TEXT NOT NULL,
    data TEXT NOT NULL DEFAULT '{"strategies":[]}',
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    expires_at DATETIME,
    UNIQUE(ts_code, trade_date)
);

-- Create indexes
CREATE INDEX IF NOT EXISTS idx_process_ts_code ON analysis_process_records(ts_code);
CREATE INDEX IF NOT EXISTS idx_process_date ON analysis_process_records(trade_date);
CREATE INDEX IF NOT EXISTS idx_process_expires ON analysis_process_records(expires_at);

-- Create trigger for auto-expiration
CREATE TRIGGER IF NOT EXISTS trg_process_set_expires_at
AFTER INSERT ON analysis_process_records
BEGIN
    UPDATE analysis_process_records 
    SET expires_at = datetime('now', '+7 days')
    WHERE id = NEW.id AND expires_at IS NULL;
END;