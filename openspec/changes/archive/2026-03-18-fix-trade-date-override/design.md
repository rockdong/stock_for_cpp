## Context

当前 `main.cpp:analyzeStock()` 在收集策略结果时错误地覆盖了 `trade_date`：

```cpp
// main.cpp:354-362 (当前代码)
for (const auto& pair : strategyResults) {
    if (pair.second.has_value()) {
        auto result = *pair.second;
        result.trade_date = analysisDate;  // ← 错误覆盖
        result.freq = freq;
        result.opt = freq;
        results.push_back(result);
    }
}
```

策略内部已正确计算 `trade_date`（来自 `data.back().trade_date`），无需覆盖。

## Goals / Non-Goals

**Goals:**
- 修复 `trade_date` 被错误覆盖的 bug
- 确保周线/月线数据的 `trade_date` 正确反映实际交易日期

**Non-Goals:**
- 不修改策略逻辑（已正确）
- 不修改 `analysisDate` 在其他地方的使用（如 `chartData.analysis_date`）

## Decisions

### D1: 删除覆盖代码

**决定**: 删除 `main.cpp:356` 的 `result.trade_date = analysisDate;`

**理由**: 策略返回的 `trade_date` 已经是正确的，无需修改。

**替代方案**: 无（这是唯一正确的修复方式）

### D2: 保留 `analysisDate` 计算

**决定**: 保留 `calculateAnalysisDate()` 函数和 `analysisDate` 变量

**理由**: `analysisDate` 仍用于 `chartData.analysis_date`，这是分析执行时间的元数据，与 `trade_date` 是不同概念。

## Risks / Trade-offs

**风险**: 历史数据的 `trade_date` 已存储错误值

**缓解**: 
- 本次修复只影响新数据
- 历史数据需要重新分析才能修正（用户可自行选择是否重新分析）