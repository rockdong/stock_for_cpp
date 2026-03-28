## Why

当前 `analysis_process_records` 表设计存在以下问题：

1. **数据冗余**：每条记录只存储一个时间点的数据，导致同一股票、同一策略、同一天可能产生大量重复记录
2. **查询效率低**：获取 10 天数据需要查询 10 条记录并聚合

现在重构可以优化存储结构、提升查询性能。

## What Changes

### 数据库表结构重构 **BREAKING**
- 将分散的价格数据和技术指标合并为 `data` JSON 字段
- 存储当前周期 + 前 9 个周期的数据（共 10 条），根据 `freq` 决定是日/周/月
- 添加唯一约束：`(ts_code, strategy_name, trade_date, freq)`

### 数据格式变更
- `data` 字段为 JSON 数组，每条包含：`time`, `open`, `high`, `low`, `close`, `volume`, `ema17`, `ema25`, `macd`, `rsi`

### API 变更
- 更新 Node.js REST API 适配新表结构

### 前端展示
- 展示 10 个周期的 K 线 + EMA 曲线图

## Capabilities

### Modified Capabilities
- `analysis-process-storage`: 分析过程数据存储，从单条记录改为 JSON 数组存储 10 个周期数据

## Impact

### 数据库
- **BREAKING**: `analysis_process_records` 表结构变更，需迁移或重建
- 现有数据需要迁移脚本

### C++ 代码
- 需要实现 JSON 序列化逻辑
- 需要修改写入逻辑，聚合 10 个周期数据

### Node.js API
- `GET /api/analysis/process` 返回格式变更
- `POST /api/analysis/process` 请求格式变更

### Web Frontend
- `web-frontend/src/types/analysis.ts` 类型定义更新
- 图表组件适配 JSON 数据格式