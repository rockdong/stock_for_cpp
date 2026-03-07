# 技术分析模块 (Analysis)

## 概述

技术分析模块提供了常用的技术指标计算功能，基于 TA-Lib 库实现，支持 7 种常用技术指标。

## 支持的指标

### 趋势指标
- **MA** - 移动平均线 (Moving Average)
- **EMA** - 指数移动平均线 (Exponential Moving Average)
- **MACD** - 平滑异同移动平均线 (Moving Average Convergence Divergence)

### 震荡指标
- **RSI** - 相对强弱指标 (Relative Strength Index)
- **KDJ** - 随机指标 (Stochastic Oscillator)

### 波动率指标
- **BOLL** - 布林带 (Bollinger Bands)
- **ATR** - 平均真实波幅 (Average True Range)

## 快速开始

### 基本用法

```cpp
#include "Analysis.h"

int main() {
    // 准备价格数据
    std::vector<double> prices = {10.0, 11.0, 12.0, 11.5, 13.0, 12.8, 14.0};
    
    // 方法1：使用静态方法（最简单）
    auto ma = analysis::MA::compute(prices, 5);
    auto rsi = analysis::RSI::compute(prices, 14);
    
    // 方法2：使用对象方式
    analysis::MA maIndicator(20);
    auto result = maIndicator.calculate(prices);
    
    // 方法3：使用工厂模式
    auto indicator = analysis::IndicatorFactory::create("MA", {{"period", 20}});
    auto factoryResult = indicator->calculate(prices);
    
    return 0;
}
```

## 详细使用指南

### 1. MA（移动平均线）

```cpp
#include "Analysis.h"

// 静态方法
std::vector<double> prices = {10.0, 11.0, 12.0, 11.5, 13.0};
auto ma = analysis::MA::compute(prices, 5, TA_MAType_SMA);

// 对象方式
analysis::MA maIndicator(20, TA_MAType_SMA);
auto result = maIndicator.calculate(prices);

// 访问结果
for (size_t i = 0; i < result->values.size(); ++i) {
    if (result->values[i] != 0.0) {
        std::cout << "MA[" << i << "] = " << result->values[i] << std::endl;
    }
}
```

**参数说明：**
- `period`: 周期（默认 20）
- `maType`: MA 类型（默认 SMA）
  - `TA_MAType_SMA` - 简单移动平均
  - `TA_MAType_EMA` - 指数移动平均
  - `TA_MAType_WMA` - 加权移动平均
  - `TA_MAType_DEMA` - 双指数移动平均
  - `TA_MAType_TEMA` - 三指数移动平均

### 2. EMA（指数移动平均线）

```cpp
// 静态方法
auto ema = analysis::EMA::compute(prices, 12);

// 对象方式
analysis::EMA emaIndicator(12);
auto result = emaIndicator.calculate(prices);
```

**参数说明：**
- `period`: 周期（默认 12）

### 3. MACD（平滑异同移动平均线）

```cpp
// 静态方法
auto macd = analysis::MACD::compute(prices, 12, 26, 9);

// 对象方式
analysis::MACD macdIndicator(12, 26, 9);
auto result = macdIndicator.calculate(prices);

// 访问 MACD 结果
auto macdResult = std::static_pointer_cast<analysis::MACDResult>(result);
std::cout << "MACD: " << macdResult->macd[i] << std::endl;
std::cout << "Signal: " << macdResult->signal[i] << std::endl;
std::cout << "Histogram: " << macdResult->histogram[i] << std::endl;
```

**参数说明：**
- `fastPeriod`: 快线周期（默认 12）
- `slowPeriod`: 慢线周期（默认 26）
- `signalPeriod`: 信号线周期（默认 9）

**返回值：**
- `macd`: MACD 线
- `signal`: 信号线
- `histogram`: 柱状图（MACD - Signal）

### 4. RSI（相对强弱指标）

```cpp
// 静态方法
auto rsi = analysis::RSI::compute(prices, 14);

// 对象方式
analysis::RSI rsiIndicator(14);
auto result = rsiIndicator.calculate(prices);

// RSI 值在 0-100 之间
// > 70: 超买
// < 30: 超卖
```

**参数说明：**
- `period`: 周期（默认 14）

### 5. KDJ（随机指标）

```cpp
std::vector<double> high = {12.0, 13.0, 14.0, 13.5, 15.0};
std::vector<double> low = {10.0, 11.0, 12.0, 11.5, 13.0};
std::vector<double> close = {11.0, 12.0, 13.0, 12.5, 14.0};

// 静态方法
auto k = analysis::KDJ::compute(high, low, close, 9, 3, 3);

// 对象方式
analysis::KDJ kdjIndicator(9, 3, 3);
auto result = kdjIndicator.calculate(high, low, close);

// 访问 KDJ 结果
auto kdjResult = std::static_pointer_cast<analysis::KDJResult>(result);
std::cout << "K: " << kdjResult->k[i] << std::endl;
std::cout << "D: " << kdjResult->d[i] << std::endl;
std::cout << "J: " << kdjResult->j[i] << std::endl;
```

**参数说明：**
- `fastK_Period`: K 线周期（默认 9）
- `slowK_Period`: K 线平滑周期（默认 3）
- `slowD_Period`: D 线平滑周期（默认 3）

**返回值：**
- `k`: K 线
- `d`: D 线
- `j`: J 线（3K - 2D）

### 6. BOLL（布林带）

```cpp
// 静态方法
auto middle = analysis::BOLL::compute(prices, 20, 2.0, 2.0);

// 对象方式
analysis::BOLL bollIndicator(20, 2.0, 2.0);
auto result = bollIndicator.calculate(prices);

// 访问 BOLL 结果
auto bollResult = std::static_pointer_cast<analysis::BOLLResult>(result);
std::cout << "Upper: " << bollResult->upper[i] << std::endl;
std::cout << "Middle: " << bollResult->middle[i] << std::endl;
std::cout << "Lower: " << bollResult->lower[i] << std::endl;
```

**参数说明：**
- `period`: 周期（默认 20）
- `nbDevUp`: 上轨标准差倍数（默认 2.0）
- `nbDevDn`: 下轨标准差倍数（默认 2.0）

**返回值：**
- `upper`: 上轨
- `middle`: 中轨（移动平均线）
- `lower`: 下轨

### 7. ATR（平均真实波幅）

```cpp
std::vector<double> high = {12.0, 13.0, 14.0, 13.5, 15.0};
std::vector<double> low = {10.0, 11.0, 12.0, 11.5, 13.0};
std::vector<double> close = {11.0, 12.0, 13.0, 12.5, 14.0};

// 静态方法
auto atr = analysis::ATR::compute(high, low, close, 14);

// 对象方式
analysis::ATR atrIndicator(14);
auto result = atrIndicator.calculate(high, low, close);
```

**参数说明：**
- `period`: 周期（默认 14）

## 工厂模式

使用工厂模式可以动态创建指标：

```cpp
#include "Analysis.h"

// 通过名称创建
auto ma = analysis::IndicatorFactory::create("MA", {{"period", 20}});
auto rsi = analysis::IndicatorFactory::create("RSI", {{"period", 14}});

// 通过枚举创建
auto macd = analysis::IndicatorFactory::create(
    analysis::IndicatorFactory::IndicatorType::MACD,
    {{"fastPeriod", 12}, {"slowPeriod", 26}, {"signalPeriod", 9}}
);

// 计算指标
auto result = ma->calculate(prices);

// 获取支持的指标列表
auto indicators = analysis::IndicatorFactory::getSupportedIndicators();
for (const auto& name : indicators) {
    std::cout << name << std::endl;
}
```

## 完整示例

```cpp
#include "Analysis.h"
#include <iostream>
#include <iomanip>

int main() {
    // 准备测试数据（模拟股票价格）
    std::vector<double> prices = {
        10.0, 10.5, 11.0, 10.8, 11.2, 11.5, 11.3, 11.8, 12.0, 11.9,
        12.2, 12.5, 12.3, 12.8, 13.0, 12.9, 13.2, 13.5, 13.3, 13.8
    };
    
    std::cout << std::fixed << std::setprecision(2);
    
    // 1. 计算 MA
    std::cout << "=== MA (5) ===" << std::endl;
    auto ma = analysis::MA::compute(prices, 5);
    for (size_t i = 0; i < ma.size(); ++i) {
        if (ma[i] != 0.0) {
            std::cout << "MA[" << i << "] = " << ma[i] << std::endl;
        }
    }
    
    // 2. 计算 RSI
    std::cout << "\n=== RSI (14) ===" << std::endl;
    auto rsi = analysis::RSI::compute(prices, 14);
    for (size_t i = 0; i < rsi.size(); ++i) {
        if (rsi[i] != 0.0) {
            std::cout << "RSI[" << i << "] = " << rsi[i];
            if (rsi[i] > 70) std::cout << " (超买)";
            if (rsi[i] < 30) std::cout << " (超卖)";
            std::cout << std::endl;
        }
    }
    
    // 3. 计算 MACD
    std::cout << "\n=== MACD (12, 26, 9) ===" << std::endl;
    analysis::MACD macdIndicator(12, 26, 9);
    auto macdResult = std::static_pointer_cast<analysis::MACDResult>(
        macdIndicator.calculate(prices)
    );
    
    for (size_t i = 0; i < macdResult->macd.size(); ++i) {
        if (macdResult->macd[i] != 0.0) {
            std::cout << "MACD[" << i << "] = " << macdResult->macd[i]
                      << ", Signal = " << macdResult->signal[i]
                      << ", Hist = " << macdResult->histogram[i] << std::endl;
        }
    }
    
    // 4. 使用工厂模式
    std::cout << "\n=== 使用工厂模式 ===" << std::endl;
    auto indicators = analysis::IndicatorFactory::getSupportedIndicators();
    std::cout << "支持的指标: ";
    for (const auto& name : indicators) {
        std::cout << name << " ";
    }
    std::cout << std::endl;
    
    return 0;
}
```

## 注意事项

1. **数据量要求**：每个指标都有最小数据量要求，通常至少需要 `period` 个数据点
2. **返回值格式**：返回的数组长度与输入相同，前面不足的部分填充 0.0
3. **异常处理**：数据不足或参数错误时会抛出 `std::invalid_argument` 异常
4. **线程安全**：指标计算是线程安全的，可以在多线程环境中使用

## API 参考

### IIndicator 接口

```cpp
class IIndicator {
public:
    virtual std::shared_ptr<IndicatorResult> calculate(const std::vector<double>& prices) = 0;
    virtual std::string getName() const = 0;
    virtual void setParameters(const std::map<std::string, double>& params) = 0;
    virtual std::map<std::string, double> getParameters() const = 0;
};
```

### IndicatorResult 结构

```cpp
struct IndicatorResult {
    std::string name;                                    // 指标名称
    std::vector<double> values;                          // 主要数值序列
    std::map<std::string, std::vector<double>> series;   // 其他数值序列
};
```

## 性能优化建议

1. **批量计算**：一次性计算多个指标比分别计算更高效
2. **缓存结果**：对于相同的数据和参数，可以缓存计算结果
3. **增量更新**：新增数据时，考虑使用增量计算而非全量重算

## 常见问题

**Q: 为什么返回的数组前面有很多 0？**  
A: 技术指标需要一定的历史数据才能计算，前面不足的部分用 0 填充。

**Q: 如何判断指标是否有效？**  
A: 检查返回值是否为 0，非 0 值表示有效的指标值。

**Q: 支持实时更新吗？**  
A: 当前版本需要重新计算整个序列，后续版本会支持增量更新。

**Q: 可以自定义指标吗？**  
A: 可以，继承 `IndicatorBase` 类并实现 `calculate` 方法即可。

## 参考资料

- [TA-Lib 官方文档](https://ta-lib.org/)
- [技术分析指标详解](https://www.investopedia.com/technical-analysis-4689657)

