## Why

分析结果的 `trade_date` 应该反映数据的实际交易日期，但当前代码在 `main.cpp:356` 错误地用 `analysisDate`（分析执行日期）覆盖了策略返回的正确 `trade_date`。

这导致：
- 周线数据（实际交易日期 20260313）被记录为 20260318
- 月线数据（实际交易日期 20260227）被记录为 20260318
- 数据筛选和查询基于错误的日期

## What Changes

- 删除 `main.cpp:356` 的 `result.trade_date = analysisDate;`
- 保留策略返回的 `trade_date`（来自 `data.back().trade_date`）
- 更新相关注释

## Capabilities

### New Capabilities

无

### Modified Capabilities

无（这是 bug 修复，不改变规格行为）

## Impact

**代码变更**：
- `cpp/main.cpp`: 删除第 356 行，更新第 355 行注释

**数据影响**：
- 修复后，分析结果的 `trade_date` 将正确反映数据的实际交易日期
- 日线：最后一条数据的交易日期（通常为昨天或当天）
- 周线：上周五的交易日期
- 月线：上月底的交易日期

**不影响**：
- `chartData.analysis_date` 继续使用 `analysisDate`（分析执行时间），这是正确的
- 策略逻辑无需修改（已正确设置 `trade_date`）