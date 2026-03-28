## Context

**后端新数据结构**：
```json
{
  "id": 73,
  "ts_code": "000421.SZ",
  "stock_name": "南京公用",
  "trade_date": "20260327",
  "data": {
    "strategies": [
      {
        "name": "EMA17TO25",
        "freqs": [
          {"freq": "d", "signal": "NONE", "candles": [...]},
          {"freq": "w", "signal": "NONE", "candles": [...]},
          {"freq": "m", "signal": "NONE", "candles": [...]}
        ]
      },
      {"name": "EMA25_CROSSOVER", "freqs": [...]}
    ]
  }
}
```

**前端期望的旧结构**：
```json
{
  "ts_code": "000421.SZ",
  "strategy_name": "EMA17TO25",
  "freq": "d",
  "signal": "BUY",
  "data": [10条K线]
}
```

## Goals / Non-Goals

**Goals:**
- 前端正确显示图表数据
- 支持选择不同策略和频率查看
- 保持用户体验流畅

**Non-Goals:**
- 不修改后端 API（已正确）
- 不修改图表组件本身

## Decisions

### 1. 类型定义更新

```typescript
// 新类型定义
export interface StrategyFreqData {
  freq: FreqType
  signal: SignalType
  candles: DataPoint[]
}

export interface StrategyData {
  name: string
  freqs: StrategyFreqData[]
}

export interface AnalysisProcessRecord {
  id: number
  ts_code: string
  stock_name: string
  trade_date: string
  data: {
    strategies: StrategyData[]
  }
  created_at: string
  expires_at: string
}
```

### 2. 图表数据处理逻辑

```typescript
// 默认显示第一个策略的日线数据
const handleSelectRecord = (record: AnalysisProcessRecord) => {
  const firstStrategy = record.data?.strategies?.[0]
  const dailyFreq = firstStrategy?.freqs?.find(f => f.freq === 'd')
  setChartData(dailyFreq?.candles || [])
}
```

### 3. 策略/频率选择器

添加下拉选择器让用户选择查看不同策略和频率的图表数据。

## Risks / Trade-offs

| 风险 | 缓解措施 |
|------|----------|
| 数据为空 | 显示"暂无数据"提示 |
| 策略列表很长 | 使用下拉选择器 |