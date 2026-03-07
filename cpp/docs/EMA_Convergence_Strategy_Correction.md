# EMA 收敛策略 - 逻辑修正说明

## 📋 修正内容

### 原始需求
判断 EMA25 和 EMA17 最后一个元素的差值小于或等于 0.03

### 修正后的逻辑

**关键点**：
1. **不是绝对值**：计算 `EMA25 - EMA17`（有方向性）
2. **差值必须为正**：`0 < 差值 <= 0.03`
3. **只检测即将金叉**：EMA25 略高于 EMA17 的情况

---

## 🎯 核心逻辑

### 判断条件

```cpp
差值 = EMA25 - EMA17  // 不是 |EMA25 - EMA17|

if (差值 > 0 && 差值 <= 0.03) {
    产生买入信号  // 预示即将金叉
}
```

### 三种场景

#### 场景 A：满足条件 ✓
```
EMA17 = 10.28, EMA25 = 10.30
差值 = 10.30 - 10.28 = 0.02
0 < 0.02 <= 0.03 ✓ 产生买入信号
```
**含义**：EMA25 略高于 EMA17，即将金叉

#### 场景 B：差值过大 ✗
```
EMA17 = 10.20, EMA25 = 10.35
差值 = 10.35 - 10.20 = 0.15
0.15 > 0.03 ✗ 不满足条件
```
**含义**：EMA25 远高于 EMA17，距离金叉还较远

#### 场景 C：已经金叉 ✗
```
EMA17 = 10.32, EMA25 = 10.30
差值 = 10.30 - 10.32 = -0.02
-0.02 < 0 ✗ 不满足条件
```
**含义**：EMA17 已经在 EMA25 上方，金叉已经发生

---

## 💻 代码实现

### 核心判断逻辑

```cpp
bool EMAConvergenceStrategy::isConverging(
    const std::vector<double>& emaShort,
    const std::vector<double>& emaLong,
    double threshold,
    bool usePercentage
) const {
    double shortValue = emaShort.back();  // EMA17
    double longValue = emaLong.back();    // EMA25
    
    // 计算 EMA25 - EMA17 的差值（不是绝对值）
    double diff = longValue - shortValue;
    
    if (usePercentage && longValue != 0) {
        // 百分比模式：(EMA25 - EMA17) / EMA25 * 100
        diff = (diff / longValue) * 100.0;
    }
    
    // 判断：0 < diff <= threshold
    return diff > 0 && diff <= threshold;
}
```

### 信号生成

```cpp
std::string EMAConvergenceStrategy::determineTrend(
    double emaShort, 
    double emaLong
) const {
    // 只有当 0 < (EMA25 - EMA17) <= threshold 时才会调用此函数
    // 此时 EMA25 > EMA17，说明短期均线在长期均线下方
    // 但差值很小，即将金叉，产生买入信号
    return "买入";
}
```

---

## 📊 使用示例

### 示例 1：基本使用

```cpp
#include "Core.h"

// 创建策略（默认阈值 0.03）
core::EMAConvergenceStrategy strategy;

// 分析股票
auto result = strategy.analyze("000001.SZ", stockData);

if (result.has_value()) {
    std::cout << "信号: " << result->label << std::endl;
    std::cout << "含义: EMA25 略高于 EMA17，即将金叉" << std::endl;
}
```

### 示例 2：自定义阈值

```cpp
// 设置阈值为 0.05
std::map<std::string, double> params;
params["convergence_threshold"] = 0.05;

core::EMAConvergenceStrategy strategy(params);

// 现在会检测：0 < (EMA25 - EMA17) <= 0.05
```

### 示例 3：百分比模式

```cpp
// 使用百分比模式，阈值 1%
std::map<std::string, double> params;
params["convergence_threshold"] = 1.0;   // 1%
params["use_percentage"] = 1.0;          // 启用百分比

core::EMAConvergenceStrategy strategy(params);

// 现在会检测：0 < (EMA25 - EMA17) / EMA25 * 100 <= 1.0
```

---

## 🔍 与其他策略的区别

### EMA_CONVERGENCE vs EMA17TO25

| 策略 | 检测内容 | 触发时机 | 信号类型 |
|------|---------|---------|---------|
| **EMA_CONVERGENCE** | EMA25 略高于 EMA17 | 金叉前 | 预警信号 |
| **EMA17TO25** | EMA17 穿越 EMA25 | 金叉时 | 确认信号 |

**推荐组合使用**：
1. `EMA_CONVERGENCE` 提前预警（T0）
2. `EMA17TO25` 确认金叉（T+1）

---

## 📈 实战时间线

```
完整的交易时间线：

T-3: EMA17 = 10.15, EMA25 = 10.30, 差值 = 0.15
     → 无信号（差值过大）

T-2: EMA17 = 10.20, EMA25 = 10.28, 差值 = 0.08
     → 无信号（差值过大）

T-1: EMA17 = 10.25, EMA25 = 10.30, 差值 = 0.05
     → 无信号（差值过大）

T0:  EMA17 = 10.28, EMA25 = 10.30, 差值 = 0.02
     → ✓ EMA_CONVERGENCE 产生买入信号（预警）
     → 准备入场

T+1: EMA17 = 10.31, EMA25 = 10.30, 差值 = -0.01
     → ✓ EMA17TO25 产生买入信号（金叉确认）
     → 执行买入

T+2: EMA17 = 10.35, EMA25 = 10.32, 差值 = -0.03
     → 持仓中
```

---

## ✅ 修正总结

### 修改前（错误）
- 使用绝对值：`|EMA25 - EMA17|`
- 只要差值 <= 0.03 就产生信号
- 无论 EMA17 在上还是在下都会触发

### 修改后（正确）
- 不使用绝对值：`EMA25 - EMA17`
- 必须满足：`0 < 差值 <= 0.03`
- 只在 EMA25 > EMA17 时触发（即将金叉）

### 策略价值
- ✅ 精确预警即将发生的金叉
- ✅ 提前 1-2 个交易日发出信号
- ✅ 为入场提供准备时间
- ✅ 结合交叉策略确认，提高成功率

---

## 🎯 使用建议

1. **单独使用**：作为预警信号，提前关注
2. **组合使用**：与 EMA17TO25 交叉策略配合
3. **参数调整**：根据股票波动性调整阈值
4. **回测验证**：在历史数据上验证效果

---

## 📚 相关文档

- 完整文档：`docs/EMA_Convergence_Strategy.md`
- 策略总结：`docs/EMA_Convergence_Strategy_Summary.md`
- 使用示例：`examples/ema_convergence_example.cpp`

