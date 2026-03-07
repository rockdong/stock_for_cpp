# EMA 收敛策略文档

## 概述

`EMAConvergenceStrategy` 是一个检测 EMA25 略高于 EMA17 的策略。当 EMA25 在 EMA17 上方，且差值在 0 到 0.03 之间时，产生买入信号。这个策略用于预测即将发生的金叉。

## 策略逻辑

### 触发条件

1. **计算差值**：EMA25 - EMA17（不是绝对值）
2. **判断条件**：0 < 差值 <= 0.03
3. **产生信号**：买入（预示即将金叉）

### 示意图

```
价格
  ^
  |     EMA25 ━━━━━━━━━━━━━━━━━━━━
  |                              ╲
  |     EMA17 ━━━━━━━━━━━━━━━━━━━╲━
  |                          ↑
  |                    差值 0 < d <= 0.03
  |                    即将金叉！
  |
  +----------> 时间
```

**关键点**：
- EMA25 > EMA17（长期均线在上方）
- 差值很小（0 到 0.03 之间）
- 预示 EMA17 即将向上突破 EMA25（金叉）

## 策略参数

| 参数名 | 类型 | 默认值 | 说明 |
|--------|------|--------|------|
| `ema_short_period` | int | 17 | 短期 EMA 周期 |
| `ema_long_period` | int | 25 | 长期 EMA 周期 |
| `convergence_threshold` | double | 0.03 | 收敛阈值 |
| `use_percentage` | bool | false | 是否使用百分比模式 |

### 参数说明

#### ema_short_period
- 短期 EMA 的计算周期
- 默认为 17
- 必须小于 `ema_long_period`

#### ema_long_period
- 长期 EMA 的计算周期
- 默认为 25
- 必须大于 `ema_short_period`

#### convergence_threshold
- 收敛阈值
- 默认为 0.03（绝对值模式）
- 条件：0 < (EMA25 - EMA17) <= threshold
- 在百分比模式下，表示百分比（如 1.0 表示 1%）

#### use_percentage
- 是否使用百分比模式
- false（0）：使用绝对值差异
- true（1）：使用百分比差异
- 百分比计算公式：`(EMA25 - EMA17) / EMA25 * 100`

## 使用方法

### 1. 在配置文件中启用

编辑 `.env` 文件：

```bash
# 添加 EMA_CONVERGENCE 到策略列表
STRATEGIES=EMA17TO25;MACD;RSI;EMA_CONVERGENCE
```

### 2. 代码中使用（默认参数）

```cpp
#include "Core.h"

// 创建策略实例
core::EMAConvergenceStrategy strategy;

// 分析股票
auto result = strategy.analyze("000001.SZ", stockData);

if (result.has_value()) {
    std::cout << "收敛信号: " << result->label << std::endl;
}
```

### 3. 自定义参数（绝对值模式）

```cpp
// 设置自定义参数
std::map<std::string, double> params;
params["ema_short_period"] = 17;
params["ema_long_period"] = 25;
params["convergence_threshold"] = 0.05;  // 阈值 0.05
params["use_percentage"] = 0.0;          // 使用绝对值

core::EMAConvergenceStrategy strategy(params);
```

### 4. 自定义参数（百分比模式）

```cpp
// 使用百分比模式
std::map<std::string, double> params;
params["ema_short_period"] = 17;
params["ema_long_period"] = 25;
params["convergence_threshold"] = 1.0;   // 阈值 1%
params["use_percentage"] = 1.0;          // 使用百分比

core::EMAConvergenceStrategy strategy(params);
```

## 策略特点

### 优势

1. **精确预警**：只在即将金叉时产生信号
2. **方向明确**：只检测 EMA25 > EMA17 的情况
3. **灵活配置**：支持绝对值和百分比两种模式
4. **可调参数**：可根据不同股票调整阈值

### 适用场景

- ✅ 预测金叉
- ✅ 寻找入场时机
- ✅ 趋势转折点识别
- ✅ 配合其他策略使用
- ❌ 单独使用（建议结合其他指标）
- ❌ 快速波动市场

### 注意事项

- 只检测 EMA25 > EMA17 的情况（即将金叉）
- 不检测 EMA17 > EMA25 的情况（即将死叉）
- 差值必须大于 0 且小于等于阈值

## 实战示例

### 示例 1：绝对值模式

```
日期       收盘价   EMA17   EMA25   差值    信号
2024-01-01  10.00   10.20   10.30   0.10    -
2024-01-02  10.10   10.22   10.28   0.06    -
2024-01-03  10.20   10.28   10.30   0.02    买入 ✓
```

**说明**：
- 差值 = EMA25 - EMA17 = 10.30 - 10.28 = 0.02
- 0 < 0.02 <= 0.03 ✓ 满足条件
- EMA25 在 EMA17 上方，但差值很小，预示即将金叉
- 产生买入信号

### 示例 2：百分比模式

```cpp
params["convergence_threshold"] = 1.0;  // 1%
params["use_percentage"] = 1.0;

日期       收盘价   EMA17   EMA25   差值%   信号
2024-01-01  10.00   10.20   10.50   2.86%   -
2024-01-02  10.10   10.25   10.40   1.44%   -
2024-01-03  10.20   10.35   10.40   0.48%   买入 ✓
```

**说明**：
- 差值% = (EMA25 - EMA17) / EMA25 * 100 = (10.40 - 10.35) / 10.40 * 100 = 0.48%
- 0 < 0.48% <= 1.0% ✓ 满足条件
- 产生买入信号

### 示例 3：不同场景

```
场景 A：满足条件（即将金叉）
日期       EMA17   EMA25   差值    信号
2024-01-03  10.28   10.30   0.02    买入 ✓
说明：EMA25 略高于 EMA17，差值在范围内

场景 B：差值过大（不满足条件）
日期       EMA17   EMA25   差值    信号
2024-01-03  10.20   10.30   0.10    - 
说明：差值 0.10 > 0.03，不满足条件

场景 C：EMA17 已经在上方（不满足条件）
日期       EMA17   EMA25   差值    信号
2024-01-03  10.32   10.30   -0.02   -
说明：差值为负，不满足 0 < 差值 <= 0.03
```

## 与其他策略配合

### 1. 与 EMA17TO25 交叉策略配合

```cpp
// 收敛策略作为预警，交叉策略作为确认
auto convergenceResult = emaConvergenceStrategy.analyze(tsCode, data);
auto crossResult = ema17to25Strategy.analyze(tsCode, data);

if (convergenceResult.has_value()) {
    std::cout << "预警：均线即将交叉" << std::endl;
}

if (crossResult.has_value() && crossResult->label == "买入") {
    std::cout << "确认：金叉发生，买入！" << std::endl;
}
```

### 2. 与 EMA17 突破策略配合

```cpp
// 收敛 + 突破 = 强信号
auto convergenceResult = emaConvergenceStrategy.analyze(tsCode, data);
auto breakoutResult = ema17BreakoutStrategy.analyze(tsCode, data);

if (convergenceResult && breakoutResult &&
    convergenceResult->label == "买入" && breakoutResult->label == "买入") {
    std::cout << "强烈买入信号：收敛 + 突破" << std::endl;
}
```

### 3. 与成交量配合

```cpp
// 收敛时成交量放大，信号更可靠
if (convergenceResult && convergenceResult->label == "买入") {
    double currentVolume = data.back().volume;
    double avgVolume = calculateAverageVolume(data, 20);
    
    if (currentVolume > avgVolume * 1.5) {
        std::cout << "放量收敛，即将金叉！" << std::endl;
    }
}
```

## 参数优化建议

### 1. 阈值优化

测试不同的收敛阈值：

```cpp
// 绝对值模式
std::vector<double> thresholds = {0.01, 0.03, 0.05, 0.10};

// 百分比模式
std::vector<double> thresholds = {0.5, 1.0, 2.0, 3.0};

for (double threshold : thresholds) {
    params["convergence_threshold"] = threshold;
    // 进行回测...
}
```

### 2. 周期优化

测试不同的 EMA 周期组合：

```cpp
std::vector<std::pair<int, int>> periods = {
    {10, 20},
    {12, 26},
    {17, 25},
    {20, 30}
};

for (const auto& [short_period, long_period] : periods) {
    params["ema_short_period"] = short_period;
    params["ema_long_period"] = long_period;
    // 进行回测...
}
```

### 3. 模式选择

比较绝对值模式和百分比模式：

```cpp
// 测试绝对值模式
params["use_percentage"] = 0.0;
params["convergence_threshold"] = 0.03;
// 回测...

// 测试百分比模式
params["use_percentage"] = 1.0;
params["convergence_threshold"] = 1.0;
// 回测...
```

## 使用场景分析

### 场景 1：预测金叉

```
时间线：
  T-2: EMA17 < EMA25，差值 0.10（未收敛）
  T-1: EMA17 < EMA25，差值 0.05（未收敛）
  T0:  EMA17 < EMA25，差值 0.02（收敛）✓ 产生买入信号
  T+1: EMA17 > EMA25（金叉发生）
```

**策略价值**：在 T0 时刻提前预警，为入场做准备。

### 场景 2：趋势确认

```
当前状态：
  - EMA17 > EMA25（多头排列）
  - 差值 0.02（收敛）
  
信号：买入
含义：多头趋势中的短暂调整，可能是加仓机会
```

### 场景 3：避免假信号

```
震荡市场：
  - 均线频繁收敛和发散
  - 建议提高阈值或结合其他指标
```

## 回测建议

### 1. 评估指标

- **预警准确率**：收敛后多久发生交叉
- **信号频率**：每月产生多少次信号
- **收益率**：按信号操作的收益
- **最大回撤**：最大亏损幅度

### 2. 回测代码框架

```cpp
// 回测框架
struct BacktestResult {
    int totalSignals;
    int successfulPredictions;
    double accuracy;
    double avgReturn;
};

BacktestResult backtest(
    const std::vector<StockData>& historicalData,
    const std::map<std::string, double>& params
) {
    EMAConvergenceStrategy strategy(params);
    BacktestResult result = {0, 0, 0.0, 0.0};
    
    // 滑动窗口回测
    for (size_t i = 30; i < historicalData.size() - 5; ++i) {
        std::vector<StockData> window(
            historicalData.begin(),
            historicalData.begin() + i
        );
        
        auto signal = strategy.analyze("TEST", window);
        
        if (signal.has_value()) {
            result.totalSignals++;
            
            // 检查未来 5 天是否发生交叉
            bool crossOccurred = checkCrossInNextDays(
                historicalData, i, 5
            );
            
            if (crossOccurred) {
                result.successfulPredictions++;
            }
        }
    }
    
    result.accuracy = (double)result.successfulPredictions / 
                      result.totalSignals * 100.0;
    
    return result;
}
```

## 注意事项

### 1. 阈值设置

- **太小**：信号过于频繁，可能产生噪音
- **太大**：错过最佳入场时机
- **建议**：根据股票价格和波动性调整

### 2. 模式选择

- **绝对值模式**：适合价格稳定的股票
- **百分比模式**：适合不同价格区间的股票
- **建议**：高价股使用百分比模式

### 3. 市场环境

- **趋势市场**：效果较好
- **震荡市场**：容易产生假信号
- **建议**：结合趋势判断指标

### 4. 数据要求

- 至少需要 `ema_long_period + 1` 天的数据
- 数据不足时返回 `std::nullopt`
- 建议准备充足的历史数据

## 性能优化

### 1. 批量分析

```cpp
// 使用 StrategyManager 批量分析
auto& manager = core::StrategyManager::getInstance();
manager.initialize("EMA_CONVERGENCE");

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

### 查询所有收敛信号

```cpp
data::AnalysisResultDAO dao;

// 查询 EMA_CONVERGENCE 策略的所有信号
auto results = dao.findByStrategy("EMA_CONVERGENCE");

for (const auto& result : results) {
    std::cout << result.ts_code << " - " 
              << result.trade_date << " - " 
              << result.label << std::endl;
}
```

### 查询特定股票的收敛记录

```cpp
// 查询某只股票的所有收敛记录
auto results = dao.findByTsCodeAndStrategy("000001.SZ", "EMA_CONVERGENCE");
```

## 总结

`EMAConvergenceStrategy` 是一个实用的预警策略：

- ✅ **预警功能**：提前发现均线交叉
- ✅ **灵活配置**：支持绝对值和百分比模式
- ✅ **趋势判断**：同时判断当前趋势
- ✅ **易于使用**：通过配置文件启用
- ⚠️ **需要确认**：建议结合交叉策略确认
- ⚠️ **参数调优**：需要根据市场调整阈值

建议在实际使用中：
1. 作为预警信号，不作为唯一依据
2. 结合 EMA17TO25 交叉策略确认
3. 根据股票特性调整阈值
4. 进行充分的回测验证
5. 注意市场环境的影响

