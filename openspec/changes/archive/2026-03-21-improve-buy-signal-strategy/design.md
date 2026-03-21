## Context

当前 EMA17TO25 策略的 `isUpTrend()` 方法存在严重缺陷：要求整个 EMA 历史序列单调递增。这在真实市场环境中几乎不可能满足，导致策略几乎不会触发买入信号。

**当前代码**:
```cpp
bool isUpTrend(const std::vector<double>& ema) const {
    for (size_t i = 1; i < ema.size(); i++) {
        if (ema[i] <= ema[i - 1]) return false;  // 任意一天下跌就失败
    }
    return true;
}
```

**问题分析**:
- 假设 100 天数据，要求 99 天全部上涨
- EMA 作为移动平均，本身就会滞后，不可能完全单调
- 这是逻辑 bug，不是设计意图

**约束条件**:
- 必须向后兼容：现有 API 和数据结构不能破坏
- 必须可配置：不同策略可能需要不同的趋势判断窗口
- 必须高效：不能影响策略计算性能

## Goals / Non-Goals

**Goals:**
- 修复 `isUpTrend()` 的逻辑缺陷，改为检查最近 N 天的趋势
- 为 `AnalysisResult` 添加信号强度和置信度字段
- 提供可配置的趋势判断窗口参数
- 保持与现有代码的兼容性

**Non-Goals:**
- 不实现自动学习/自适应参数（用户明确要求）
- 不修改其他策略的实现（仅作为参考）
- 不添加新的外部依赖
- 不修改数据库 schema（本次仅改内存结构）

## Decisions

### 1. 趋势判断窗口化

**决定**: 将"全序列检查"改为"窗口期检查"

**实现**:
```cpp
bool isUpTrend(const std::vector<double>& ema) const {
    int trendDays = static_cast<int>(getParameter("trend_days", 3));
    if (ema.size() < static_cast<size_t>(trendDays + 1)) {
        return false;
    }
    
    // 只检查最近 trend_days 天
    for (size_t i = ema.size() - trendDays; i < ema.size(); i++) {
        if (ema[i] <= ema[i - 1]) return false;
    }
    return true;
}
```

**理由**: 
- 3 天窗口足以确认短期趋势
- 默认值保守，用户可根据策略风格调整
- 时间复杂度从 O(n) 降到 O(k)，k 为窗口大小

**替代方案**:
- 方案 A: 检查趋势斜率（需要引入数学计算）
- 方案 B: 检查 EMA 差值变化（与现有逻辑耦合更高）
- **选择当前方案**: 简单直观，易于理解和维护

### 2. 信号强度评估

**决定**: 在 `AnalysisResult` 中添加 `strength` 和 `confidence` 字段

**数据结构**:
```cpp
enum class SignalStrength { WEAK, MEDIUM, STRONG };

struct AnalysisResult {
    // 现有字段
    std::string ts_code;
    std::string strategy_name;
    std::string trade_date;
    std::string label;
    std::string opt;
    std::string freq;
    
    // 新增字段
    SignalStrength strength = SignalStrength::MEDIUM;
    double confidence = 50.0;  // 0-100
    std::string risk_warning;
};
```

**理由**:
- 使用 enum 保证类型安全
- 默认值 MEDIUM/50.0 保持向后兼容
- `risk_warning` 提供风险提示文本

### 3. 参数配置方式

**决定**: 使用现有 `setParameter()` 机制，不引入新的配置文件

**新增参数**:
| 参数名 | 默认值 | 说明 |
|--------|--------|------|
| `trend_days` | 3 | 趋势判断窗口天数 |
| `min_confidence` | 0 | 最小置信度阈值 |

**理由**: 
- 复用现有参数机制，减少改动
- 与其他策略参数保持一致

## Risks / Trade-offs

**风险 1**: 修复后触发频率大幅增加
- **缓解**: 默认窗口 3 天仍然保守
- **监控**: 建议上线后观察信号质量

**风险 2**: 新字段影响现有序列化/反序列化
- **缓解**: 新字段有默认值，JSON 输出时可选
- **测试**: 添加单元测试验证兼容性

**权衡**: 简单性 vs 精确性
- **选择**: 优先简单性，后续可根据实际效果迭代

## Migration Plan

### 阶段 1: 代码修改 (1-2 天)
1. 修改 `EMA17TO25Strategy.h/cpp`
2. 修改 `AnalysisResult.h`
3. 更新 `StrategyBase::createResult()` 方法

### 阶段 2: 测试验证 (1 天)
1. 编写单元测试
2. 回测历史数据验证
3. 对比修改前后的信号差异

### 阶段 3: 部署上线 (1 天)
1. 合并代码
2. 更新 Docker 镜像
3. 观察线上效果

### 回滚策略
- 代码回滚：`git revert`
- 参数回滚：设置 `trend_days` 为极大值（如 999）

## Open Questions

1. ~~是否需要添加成交量确认？~~ → 作为后续迭代，本次不做
2. 其他策略（EMA17Breakout、EMAConvergence）是否需要同步修改？ → 建议单独评估
3. 是否需要记录修改前后的信号差异？ → 建议添加日志