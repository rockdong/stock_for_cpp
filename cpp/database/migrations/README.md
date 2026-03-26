# 数据库迁移指南

## 迁移版本

| 版本 | 文件 | 说明 |
|------|------|------|
| 001 | `001_create_analysis_process_records.sql` | 初始表结构 |
| 002 | `002_refactor_analysis_process_records.sql` | 重构为 JSON 数组存储 |

## 版本 002 迁移说明

### 变更内容

1. **新增字段**
   - `data` - JSON 数组，包含 10 个周期的 K 线数据

2. **移除字段**
   - `time`, `open`, `high`, `low`, `close`, `volume` (合并到 `data`)
   - `ema17`, `ema25`, `macd`, `rsi` (合并到 `data`)

3. **新增约束**
   - `UNIQUE(ts_code, strategy_name, trade_date, freq)`

### 迁移步骤

```bash
# 1. 备份数据库
cp stock.db stock.db.backup

# 2. 执行迁移脚本
sqlite3 stock.db < cpp/database/migrations/002_refactor_analysis_process_records.sql

# 3. 验证迁移结果
sqlite3 stock.db "SELECT COUNT(*) FROM analysis_process_records;"
```

### 数据格式

`data` 字段为 JSON 数组，每条记录包含：

```json
[
  {
    "time": "2026-03-17",
    "open": 10.00,
    "high": 10.50,
    "low": 9.80,
    "close": 10.20,
    "volume": 1000000,
    "ema17": 10.10,
    "ema25": 9.90,
    "macd": 0.15,
    "rsi": 55.20
  }
]
```

### 回滚

```sql
-- 恢复旧表
DROP TABLE IF EXISTS analysis_process_records;
ALTER TABLE analysis_process_records_backup RENAME TO analysis_process_records;
```