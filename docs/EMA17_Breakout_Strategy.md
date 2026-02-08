# EMA17 突破策略文档

## 概述

`EMA17BreakoutStrategy` 是一个检测股票收盘价首次站上 EMA17 均线的策略。当股票价格从 EMA17 下方突破到上方时，产生买入信号。

## 策略逻辑

### 触发条件

1. **前一天**：收盘价 < EMA17
2. **当天**：收盘价 >= EMA17
3. **结果**：产生买入信号

### 示意图

```
价格
  ^
  |     ●  ← 当天收盘价（突破）
  |    /
  |   /
  |  ● ← 前一天收盘价（在下方）
  | /
  |/__________ EMA17
  |
  +----------> 时间
```

## 策略参数

| 参数名 | 类型 | 默认值 | 说明 |
|--------|------|--------|------|
| `ema_period` | int | 17 | EMA 周期 |
| `min_breakout_pct` | double | 0.0 | 最小突破幅度百分比 |

### 参数说明

#### ema_period
- EMA 均线的计算周期
- 默认为 17，可以根据需要调整
- 常用值：10, 17, 20, 30

#### min_breakout_pct
- 最小突破幅度百分比
- 用于过滤微小突破，避免假信号
- 例如：设置为 1.0 表示价格必须至少高于 EMA17 的 1%
- 默认为 0.0，表示不限制

## 使用方法

### 1. 在配置文件中启用

编辑 `.env` 文件：

```bash
# 添加 EMA17_BREAKOUT 到策略列表
STRATEGIES=EMA17TO25;MACD;RSI;EMA17_BREAKOUT
```

### 2. 代码中使用

```cpp
#include "Core.h"

// 创建策略实例
core::EMA17BreakoutStrategy strategy;

// 分析股票
auto result = strategy.analyze("000001.SZ", stockData);

if (result.has_value()) {
    std::cout << "突破信号: " << result->label << std::endl;
}
```

### 3. 自定义参数

```cpp
// 设置自定义参数
std::map<std::string, double> params;
params["ema_period"] = 20;           // 使用 EMA20
params["min_breakout_pct"] = 1.0;    // 要求至少突破 1%

core::EMA17BreakoutStrategy strategy(params);
```

## 策略特点

### 优势

1. **简单明确**：逻辑清晰，易于理解
2. **趋势跟随**：捕捉上升趋势的起点
3. **及时入场**：在突破发生时立即产生信号
4. **可配置**：支持自定义 EMA 周期和突破幅度

### 适用场景

- ✅ 趋势性行情
- ✅ 突破后持续上涨
- ✅ 配合其他指标使用
- ❌ 震荡市场（容易产生假突破）
- ❌ 单独使用（建议结合其他策略）

## 实战示例

### 示例 1：基本突破

```
日期       收盘价   EMA17   信号
2024-01-01  10.00   10.50   -
2024-01-02  10.20   10.48   -
2024-01-03  10.60   10.52   买入 ✓  (突破)
```

**说明**：1月3日收盘价从 EMA17 下方突破到上方，产生买入信号。

### 示例 2：带最小突破幅度

```cpp
// 设置最小突破幅度为 1%
params["min_breakout_pct"] = 1.0;

日期       收盘价   EMA17   突破幅度   信号
2024-01-01  10.00   10.50   -         -
2024-01-02  10.20   10.48   -         -
2024-01-03  10.55   10.52   0.29%     - (不满足 1%)
2024-01-04  10.70   10.55   1.42%     买入 ✓
```

**说明**：1月3日虽然突破，但幅度只有 0.29%，不满足 1% 的要求。1月4日突破幅度达到 1.42%，产生买入信号。

### 示例 3：假突破

```
日期       收盘价   EMA17   信号
2024-01-01  10.00   10.50   -
2024-01-02  10.55   10.52   买入 ✓  (突破)
2024-01-03  10.40   10.51   -       (回落)
2024-01-04  10.30   10.48   -
```

**说明**：1月2日产生突破信号，但随后价格回落，形成假突破。建议结合其他指标确认。

## 与其他策略配合

### 1. 与 EMA17TO25 配合

```cpp
// 同时使用两个策略
auto breakoutResult = ema17BreakoutStrategy.analyze(tsCode, data);
auto crossResult = ema17to25Strategy.analyze(tsCode, data);

// 两个策略都产生买入信号时，信号更强
if (breakoutResult.has_value() && crossResult.has_value()) {
    if (breakoutResult->label == "买入" && crossResult->label == "买入") {
        std::cout << "强烈买入信号！" << std::endl;
    }
}
```

### 2. 与 RSI 配合

```cpp
// EMA17 突破 + RSI 未超买
auto breakoutResult = ema17BreakoutStrategy.analyze(tsCode, data);
auto rsiResult = rsiStrategy.analyze(tsCode, data);

if (breakoutResult.has_value() && rsiResult.has_value()) {
    if (breakoutResult->label == "买入" && rsiResult->label != "卖出") {
        std::cout << "突破且未超买，可以买入" << std::endl;
    }
}
```

### 3. 与成交量配合

```cpp
// 突破时成交量放大，信号更可靠
if (breakoutResult.has_value() && breakoutResult->label == "买入") {
    double currentVolume = data.back().volume;
    double avgVolume = calculateAverageVolume(data, 20);
    
    if (currentVolume > avgVolume * 1.5) {
        std::cout << "放量突破，信号可靠！" << std::endl;
    }
}
```

## 回测建议

### 1. 参数优化

测试不同的 EMA 周期：

```cpp
std::vector<int> periods = {10, 17, 20, 30};
for (int period : periods) {
    params["ema_period"] = period;
    // 进行回测...
}
```

### 2. 突破幅度优化

测试不同的最小突破幅度：

```cpp
std::vector<double> breakoutPcts = {0.0, 0.5, 1.0, 2.0};
for (double pct : breakoutPcts) {
    params["min_breakout_pct"] = pct;
    // 进行回测...
}
```

### 3. 持仓时间

建议测试不同的持仓时间：
- 短期：3-5 天
- 中期：10-20 天
- 长期：30+ 天

## 注意事项

### 1. 假突破风险

- 突破后可能快速回落
- 建议设置止损位（如 EMA17 下方 2%）
- 结合其他指标确认

### 2. 震荡市场

- 在震荡市场中容易产生频繁信号
- 建议添加趋势过滤器
- 或提高 `min_breakout_pct` 参数

### 3. 数据要求

- 至少需要 EMA 周期 + 2 天的数据
- 数据不足时返回 `std::nullopt`
- 建议准备充足的历史数据

## 性能优化

### 1. 批量分析

```cpp
// 使用 StrategyManager 批量分析
auto& manager = core::StrategyManager::getInstance();
manager.initialize("EMA17_BREAKOUT");

for (const auto& stock : stockList) {
    auto data = getStockData(stock.ts_code);
    auto results = manager.analyzeAll(stock.ts_code, data);
    // 处理结果...
}
```

### 2. 多线程

```cpp
// 利用多线程加速分析
THREAD_COUNT=8  // 在 .env 中配置
```

## 数据库查询

### 查询所有突破信号

```cpp
data::AnalysisResultDAO dao;

// 查询 EMA17_BREAKOUT 策略的所有买入信号
auto results = dao.findByStrategy("EMA17_BREAKOUT");

for (const auto& result : results) {
    if (result.label == "买入") {
        std::cout << result.ts_code << " - " << result.trade_date << std::endl;
    }
}
```

### 查询特定股票的突破记录

```cpp
// 查询某只股票的所有突破记录
auto results = dao.findByTsCodeAndStrategy("000001.SZ", "EMA17_BREAKOUT");
```

## 总结

`EMA17BreakoutStrategy` 是一个简单有效的趋势跟随策略：

- ✅ **逻辑清晰**：检测首次突破 EMA17
- ✅ **及时入场**：在突破发生时产生信号
- ✅ **可配置**：支持自定义参数
- ✅ **易于使用**：通过配置文件启用
- ⚠️ **需要确认**：建议结合其他指标
- ⚠️ **风险控制**：注意假突破和止损

建议在实际使用中：
1. 结合其他策略确认信号
2. 设置合理的止损位
3. 根据市场环境调整参数
4. 进行充分的回测验证

