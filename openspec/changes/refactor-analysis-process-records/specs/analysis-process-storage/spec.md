# Analysis Process Storage

分析过程数据存储能力，用于存储股票分析的 K 线数据和技术指标。

## ADDED Requirements

### Requirement: Store 10 periods of data as JSON array
系统 SHALL 将 10 个周期的 K 线数据和技术指标以 JSON 数组格式存储在 `data` 字段中。

**周期规则**：
- `freq='d'`: 当天 + 前 9 个交易日
- `freq='w'`: 当周 + 前 9 周
- `freq='m'`: 当月 + 前 9 月

#### Scenario: Store daily data
- **WHEN** 分析日线数据 (`freq='d'`)
- **THEN** `data` 字段 SHALL 包含 10 条记录
- **AND** 每条记录 SHALL 包含 `time`, `open`, `high`, `low`, `close`, `volume`, `ema17`, `ema25`, `macd`, `rsi` 字段
- **AND** `time` 格式 SHALL 为 `YYYY-MM-DD`

#### Scenario: Store weekly data
- **WHEN** 分析周线数据 (`freq='w'`)
- **THEN** `data` 字段 SHALL 包含 10 条周记录
- **AND** `time` 格式 SHALL 为 `YYYY-WXX`

#### Scenario: Store monthly data
- **WHEN** 分析月线数据 (`freq='m'`)
- **THEN** `data` 字段 SHALL 包含 10 条月记录
- **AND** `time` 格式 SHALL 为 `YYYY-MM`

### Requirement: JSON data structure format
`data` 字段 SHALL 遵循以下 JSON 结构：

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
  },
  ...
]
```

#### Scenario: Data field validation
- **WHEN** 写入 `data` 字段
- **THEN** 系统 SHALL 验证 JSON 格式有效性
- **AND** 系统 SHALL 验证数组长度为 10
- **AND** 系统 SHALL 验证每条记录包含所有必需字段

### Requirement: Unique constraint on records
系统 SHALL 对 `(ts_code, strategy_name, trade_date, freq)` 组合实施唯一约束。

#### Scenario: Insert new record
- **WHEN** 插入新的分析记录（股票、策略、日期、周期组合不存在）
- **THEN** 系统 SHALL 成功创建新记录

#### Scenario: Upsert existing record
- **WHEN** 插入已存在的分析记录（股票、策略、日期、周期组合已存在）
- **THEN** 系统 SHALL 更新现有记录而非创建重复记录

### Requirement: API returns parsed JSON data
Node.js REST API SHALL 返回解析后的 JSON 数据对象，而非字符串。

#### Scenario: GET process records API
- **WHEN** 用户请求 `GET /api/analysis/process`
- **THEN** 响应中的 `data` 字段 SHALL 为 JSON 对象数组
- **AND** 不应为 JSON 字符串

#### Scenario: GET chart data API
- **WHEN** 用户请求 `GET /api/analysis/process/chart/:ts_code`
- **THEN** 响应 SHALL 包含解析后的图表数据
- **AND** 数据 SHALL 按时间升序排列

### Requirement: Data expiration after 7 days
系统 SHALL 在记录创建 7 天后自动标记为过期。

#### Scenario: Auto-set expiration
- **WHEN** 新记录被创建
- **THEN** `expires_at` 字段 SHALL 自动设置为创建时间 + 7 天

#### Scenario: Expired records cleanup
- **WHEN** 调用 `DELETE /api/analysis/process/expired`
- **THEN** 系统 SHALL 删除所有 `expires_at < datetime('now')` 的记录

## REMOVED Requirements

### Requirement: Individual time point records
**Reason**: 重构为 JSON 数组存储，不再需要分散的单条记录
**Migration**: 使用迁移脚本将现有数据聚合为 JSON 数组格式

原有字段（已废弃）：
- `time` (独立字段)
- `open`, `high`, `low`, `close`, `volume` (独立字段)
- `ema17`, `ema25`, `macd`, `rsi` (独立字段)

**迁移方案**：
1. 按 `(ts_code, strategy_name, trade_date, freq)` 分组
2. 取每组最近 10 条记录
3. 聚合为 JSON 数组存入新 `data` 字段