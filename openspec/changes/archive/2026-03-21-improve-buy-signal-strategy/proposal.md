## Why

当前 EMA17/EMA25 黄金交叉策略的 `isUpTrend()` 方法要求整个 EMA 序列单调递增，这在实际交易中几乎不可能满足，导致策略几乎不会触发买入信号。此外，现有策略缺乏信号强度分级和成交量确认机制，无法区分高质量信号和低质量信号，容易产生假阳性。

**为什么现在做**: 买点预测师已完成深入分析，明确了问题根因和改进方向，是实施整改的最佳时机。

## What Changes

- **改进趋势判断逻辑**: 将"全序列单调递增"改为"最近 N 天趋势向上"，N 值可配置
- **新增信号强度评估**: 为每个买入信号增加强度分级（STRONG/MEDIUM/WEAK）和置信度（0-100）
- **新增成交量确认**: 可选的成交量放大确认机制，过滤假突破
- **增强分析结果结构**: `AnalysisResult` 新增 `strength`、`confidence`、`risk_warning` 字段

## Capabilities

### New Capabilities

- `signal-strength-evaluation`: 信号强度评估能力，为交易信号提供质量评分和风险提示

### Modified Capabilities

- `core-system`: 核心策略引擎的需求变更，趋势判断逻辑从"全序列检查"改为"窗口期检查"

## Impact

**代码变更**:
- `cpp/core/strategies/EMA17TO25Strategy.cpp` - 改进 `isUpTrend()` 方法
- `cpp/core/strategies/EMA17TO25Strategy.h` - 新增参数 `trend_days`
- `cpp/core/AnalysisResult.h` - 新增字段 `strength`, `confidence`, `risk_warning`
- 其他策略文件可能受影响（EMA17BreakoutStrategy、EMAConvergenceStrategy 等）

**兼容性**:
- ✅ 向后兼容：现有字段保持不变，新字段有默认值
- ✅ 配置兼容：新参数使用默认值，无需修改现有配置

**风险**:
- 🟡 策略行为变更：修复后触发频率会增加，需要观察实际效果
- 🟡 数据库兼容：如果 `AnalysisResult` 有持久化，需要处理新字段