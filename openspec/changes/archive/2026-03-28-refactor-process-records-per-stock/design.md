## Context

**当前数据结构**：
```
analysis_process_records 表
├── ts_code (股票代码)
├── strategy_name (策略名称)  ← 导致多条记录
├── freq (频率: d/w/m)       ← 导致多条记录
├── signal (信号类型)
├── data (JSON: 10条K线)
└── UNIQUE(ts_code, strategy_name, trade_date, freq)
```

**问题**：一只股票 × N个策略 × 3个频率 = 多条记录

**目标**：一只股票 = 一条记录，包含所有策略和频率的数据

## Goals / Non-Goals

**Goals:**
- 每只股票每天只保存一条记录
- JSON 包含所有策略、所有频率的完整数据
- 支持不同策略计算不同技术指标
- 保持 API 向后兼容

**Non-Goals:**
- 不修改策略分析逻辑
- 不修改前端展示（API 兼容）

## Decisions

### 1. 新 JSON 结构

```json
{
  "ts_code": "000001.SZ",
  "stock_name": "西王食品",
  "trade_date": "20260327",
  "strategies": [
    {
      "name": "EMA17TO25",
      "freqs": [
        {
          "freq": "d",
          "signal": "BUY",
          "candles": [
            {"time": "20260316", "open": 2.9, "high": 3.14, "low": 2.89, "close": 3.14, "volume": 1114353, "ema17": 2.89, "ema25": 2.92},
            // ... 10 条
          ]
        },
        {"freq": "w", "signal": "NONE", "candles": []},
        {"freq": "m", "signal": "BUY", "candles": [...]}
      ]
    },
    {
      "name": "MACD",
      "freqs": [
        {
          "freq": "d",
          "signal": "SELL",
          "candles": [
            {"time": "20260316", "open": 2.9, "high": 3.14, "low": 2.89, "close": 3.14, "volume": 1114353, "macd": 0.05, "signal_line": 0.03},
            // ... MACD 策略特有的指标
          ]
        }
      ]
    }
  ]
}
```

### 2. 新数据模型

```cpp
struct StrategyFreqData {
    std::string freq;                           // d/w/m
    std::string signal;                         // BUY/SELL/HOLD/NONE
    std::vector<ProcessDataPoint> candles;      // 10条K线（可能为空）
};

struct StrategyData {
    std::string name;                           // 策略名称
    std::vector<StrategyFreqData> freqs;        // 3个频率的数据
};

struct StockProcessRecord {
    std::string ts_code;
    std::string stock_name;
    std::string trade_date;
    std::vector<StrategyData> strategies;       // 所有策略的数据
    std::string created_at;
    std::string expires_at;
};
```

### 3. 表结构变更

**方案：删除重建表**（简单直接，数据可重新生成）

```sql
DROP TABLE IF EXISTS analysis_process_records;

CREATE TABLE analysis_process_records (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    ts_code TEXT NOT NULL,
    stock_name TEXT,
    trade_date TEXT NOT NULL,
    data TEXT NOT NULL DEFAULT '{}',
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    expires_at DATETIME,
    UNIQUE(ts_code, trade_date)
);

CREATE INDEX idx_process_ts_code ON analysis_process_records(ts_code);
CREATE INDEX idx_process_date ON analysis_process_records(trade_date);
CREATE INDEX idx_process_expires ON analysis_process_records(expires_at);
```

### 4. API 兼容策略

**现有 API** `/api/analysis/process`：
- 保持返回数组格式
- 每条记录展开为多条返回（向后兼容）
- 新增字段 `strategies` 包含完整数据

```json
// 兼容返回格式
[
  {
    "ts_code": "000001.SZ",
    "stock_name": "西王食品",
    "trade_date": "20260327",
    "strategies": [...]  // 完整数据
  }
]
```

### 5. main.cpp 重构

**修改 `analyzeStock()` 函数**：
1. 收集所有策略的所有频率数据
2. 构建 `StockProcessRecord` 对象
3. 一次性 upsert 到数据库

```cpp
StockProcessRecord record;
record.ts_code = stock.ts_code;
record.stock_name = stock.name;
record.trade_date = analysisDate;

for (const auto& strategy : strategyManager.getStrategies()) {
    StrategyData strategyData;
    strategyData.name = strategy->getName();
    
    for (const auto& freq : {"d", "w", "m"}) {
        StrategyFreqData freqData;
        freqData.freq = freq;
        freqData.signal = "NONE";
        
        // 获取数据、计算指标、填充 candles
        auto data = dataSource->getQuoteData(stock.ts_code, "", "", freq);
        auto result = strategy->analyze(stock.ts_code, data);
        
        if (result.has_value()) {
            freqData.signal = result->label == "买入" ? "BUY" : 
                              result->label == "卖出" ? "SELL" : "HOLD";
        }
        
        // 填充 10 条 K 线数据
        fillCandles(freqData.candles, data, strategy);
        
        strategyData.freqs.push_back(freqData);
    }
    
    record.strategies.push_back(strategyData);
}

processRecordDao.upsert(record);
```

## Risks / Trade-offs

| 风险 | 影响 | 缓解措施 |
|------|------|----------|
| 单条记录变大 | 从 ~500B 增至 ~50KB | JSON 压缩；前端分页加载 |
| JSON 查询复杂 | SQLite 无法直接查询 JSON 内字段 | API 层处理过滤逻辑 |
| 迁移数据丢失 | 现有数据被删除 | 数据可重新生成，无需保留 |

## Migration Plan

1. **部署前**：无需迁移，新表结构即可
2. **部署后**：C++ 分析会自动填充新数据
3. **回滚**：恢复旧代码即可，表会自动重建