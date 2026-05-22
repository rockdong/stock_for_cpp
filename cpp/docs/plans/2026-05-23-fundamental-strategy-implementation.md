# 基本面策略化 + Emoji 实施计划

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** 将基本面筛选改造为策略模式，所有策略使用 emoji 标识，移除自适应学习系统

**Architecture:** 新增 FundamentalStrategy 继承 StrategyBase，新增 StrategyEmojiMapper 提供 emoji 映射，前端添加 hover 提示

**Tech Stack:** C++17, React + TypeScript, NestJS

---

## Task 1: 创建 StrategyEmojiMapper 头文件

**Files:**
- Create: `cpp/core/StrategyEmojiMapper.h`

**Step 1: 创建头文件**

```cpp
#ifndef CORE_STRATEGY_EMOJI_MAPPER_H
#define CORE_STRATEGY_EMOJI_MAPPER_H

#include <string>
#include <map>

namespace core {

/**
 * @brief 策略 Emoji 映射器
 * 
 * 提供策略名称与 emoji 的双向映射
 */
class StrategyEmojiMapper {
public:
    /**
     * @brief 初始化映射表
     */
    static void initialize();
    
    /**
     * @brief 策略名称 → Emoji
     * @param strategyName 策略名称
     * @return Emoji 字符串，未注册则返回原名称
     */
    static std::string getEmoji(const std::string& strategyName);
    
    /**
     * @brief 检查策略是否已注册 emoji
     */
    static bool isRegistered(const std::string& strategyName);
    
    /**
     * @brief 注册策略的 emoji（用于扩展）
     */
    static void registerEmoji(const std::string& strategyName, const std::string& emoji);
    
    /**
     * @brief 获取所有映射
     */
    static const std::map<std::string, std::string>& getAllMappings();

private:
    static std::map<std::string, std::string> emojiMap_;
    static bool initialized_;
};

} // namespace core

#endif // CORE_STRATEGY_EMOJI_MAPPER_H
```

**Step 2: 验证文件创建**

Run: `ls cpp/core/StrategyEmojiMapper.h`
Expected: 文件存在

**Step 3: Commit**

```bash
git add cpp/core/StrategyEmojiMapper.h
git commit -m "feat(core): add StrategyEmojiMapper header"
```

---

## Task 2: 创建 StrategyEmojiMapper 实现文件

**Files:**
- Create: `cpp/core/StrategyEmojiMapper.cpp`

**Step 1: 创建实现文件**

```cpp
#include "StrategyEmojiMapper.h"
#include "Logger.h"

namespace core {

std::map<std::string, std::string> StrategyEmojiMapper::emojiMap_;
bool StrategyEmojiMapper::initialized_ = false;

void StrategyEmojiMapper::initialize() {
    if (initialized_) return;
    
    // 默认映射表
    emojiMap_ = {
        {"EMA17TO25Strategy", "🔗"},
        {"EMA25Greater17PriceMatchStrategy", "⬆️"},
        {"EMA17BreakoutStrategy", "🚀"},
        {"EMA25CrossoverStrategy", "🔄"},
        {"EMAConvergenceStrategy", "🎯"},
        {"MACrossStrategy", "📈"},
        {"MACDStrategy", "📊"},
        {"RSIStrategy", "🎢"},
        {"BOLLStrategy", "🎪"},
        {"GridStrategy", "🧱"},
        {"SurgeSignalStrategy", "💥"},
        {"FundamentalStrategy", "💎"}
    };
    
    initialized_ = true;
    
    LOG_DEBUG("StrategyEmojiMapper 初始化完成，已注册 " + 
              std::to_string(emojiMap_.size()) + " 个策略 emoji");
}

std::string StrategyEmojiMapper::getEmoji(const std::string& strategyName) {
    if (!initialized_) initialize();
    
    auto it = emojiMap_.find(strategyName);
    if (it != emojiMap_.end()) {
        return it->second;
    }
    
    // 未注册则返回原名称
    return strategyName;
}

bool StrategyEmojiMapper::isRegistered(const std::string& strategyName) {
    if (!initialized_) initialize();
    return emojiMap_.find(strategyName) != emojiMap_.end();
}

void StrategyEmojiMapper::registerEmoji(const std::string& strategyName, const std::string& emoji) {
    if (!initialized_) initialize();
    emojiMap_[strategyName] = emoji;
}

const std::map<std::string, std::string>& StrategyEmojiMapper::getAllMappings() {
    if (!initialized_) initialize();
    return emojiMap_;
}

} // namespace core
```

**Step 2: 验证文件创建**

Run: `ls cpp/core/StrategyEmojiMapper.cpp`
Expected: 文件存在

**Step 3: Commit**

```bash
git add cpp/core/StrategyEmojiMapper.cpp
git commit -m "feat(core): implement StrategyEmojiMapper"
```

---

## Task 3: 创建 FundamentalStrategy 头文件

**Files:**
- Create: `cpp/core/strategies/FundamentalStrategy.h`

**Step 1: 创建头文件**

```cpp
#ifndef CORE_FUNDAMENTAL_STRATEGY_H
#define CORE_FUNDAMENTAL_STRATEGY_H

#include "../Strategy.h"
#include "../FundamentalFilter.h"
#include "../FundamentalData.h"

namespace core {

/**
 * @brief 基本面策略
 * 
 * 根据估值、盈利、增长、健康评分生成交易信号
 * 
 * 信号阈值：
 * - 总分 ≥80: 强买入 (STRONG)
 * - 总分 ≥60: 中买入 (MEDIUM)
 * - 总分 ≥40: 弱买入 (WEAK)
 * - 总分 <40: 卖出
 * - 数据缺失: 持有
 */
class FundamentalStrategy : public StrategyBase {
public:
    explicit FundamentalStrategy(const std::map<std::string, double>& params = {});
    
    std::optional<AnalysisResult> analyze(
        const std::string& tsCode,
        const std::vector<StockData>& data
    ) override;
    
    bool validateParameters() const override;

private:
    // 评分阈值
    double strongBuyThreshold_ = 80.0;
    double mediumBuyThreshold_ = 60.0;
    double weakBuyThreshold_ = 40.0;
    double sellThreshold_ = 40.0;
    
    FundamentalFilter filter_;
    
    /**
     * @brief 获取财务指标数据
     */
    std::optional<FinancialIndicator> getFinancialIndicator(
        const std::string& tsCode,
        const std::string& tradeDate
    ) const;
    
    /**
     * @brief 根据评分生成信号
     */
    AnalysisResult generateSignal(
        const std::string& tsCode,
        const std::string& tradeDate,
        double totalScore,
        const std::string& grade
    ) const;
};

} // namespace core

#endif // CORE_FUNDAMENTAL_STRATEGY_H
```

**Step 2: 验证文件创建**

Run: `ls cpp/core/strategies/FundamentalStrategy.h`
Expected: 文件存在

**Step 3: Commit**

```bash
git add cpp/core/strategies/FundamentalStrategy.h
git commit -m "feat(core): add FundamentalStrategy header"
```

---

## Task 4: 创建 FundamentalStrategy 实现文件

**Files:**
- Create: `cpp/core/strategies/FundamentalStrategy.cpp`

**Step 1: 创建实现文件**

```cpp
#include "FundamentalStrategy.h"
#include "../../network/DataSourceFactory.h"
#include "../../utils/TimeUtil.h"
#include "../../log/Logger.h"

namespace core {

FundamentalStrategy::FundamentalStrategy(const std::map<std::string, double>& params)
    : StrategyBase("FundamentalStrategy", "基本面策略 - 估值/盈利/增长/健康评分")
{
    setParameter("strong_buy_threshold", 80.0);
    setParameter("medium_buy_threshold", 60.0);
    setParameter("weak_buy_threshold", 40.0);
    setParameter("sell_threshold", 40.0);
    
    setParameters(params);
    
    // 更新阈值
    strongBuyThreshold_ = getParameter("strong_buy_threshold", 80.0);
    mediumBuyThreshold_ = getParameter("medium_buy_threshold", 60.0);
    weakBuyThreshold_ = getParameter("weak_buy_threshold", 40.0);
    sellThreshold_ = getParameter("sell_threshold", 40.0);
}

std::optional<AnalysisResult> FundamentalStrategy::analyze(
    const std::string& tsCode,
    const std::vector<StockData>& data
) {
    if (data.empty()) {
        return std::nullopt;
    }
    
    // 获取分析日期（使用最新交易日期）
    std::string tradeDate = data.back().trade_date;
    
    // 获取财务指标数据
    auto indicatorOpt = getFinancialIndicator(tsCode, tradeDate);
    if (!indicatorOpt) {
        LOG_WARN("FundamentalStrategy: 无法获取 " + tsCode + " 的财务数据");
        return std::nullopt;
    }
    
    const FinancialIndicator& indicator = *indicatorOpt;
    
    // 使用 FundamentalFilter 计算评分
    FundamentalScore score = filter_.calculateScore(indicator);
    
    // 生成信号
    return generateSignal(tsCode, tradeDate, score.totalScore, score.grade);
}

bool FundamentalStrategy::validateParameters() const {
    return strongBuyThreshold_ >= mediumBuyThreshold_ &&
           mediumBuyThreshold_ >= weakBuyThreshold_ &&
           weakBuyThreshold_ >= sellThreshold_;
}

std::optional<FinancialIndicator> FundamentalStrategy::getFinancialIndicator(
    const std::string& tsCode,
    const std::string& tradeDate
) const {
    try {
        auto dataSource = network::DataSourceFactory::createFromConfig();
        
        // 获取每日指标数据
        auto tushareSource = std::dynamic_pointer_cast<network::TushareDataSource>(dataSource);
        if (!tushareSource) {
            return std::nullopt;
        }
        
        auto response = tushareSource->getClient()->getDailyBasic(tsCode, tradeDate, "", "");
        
        if (!response.isSuccess() || !response.data.contains("items")) {
            return std::nullopt;
        }
        
        auto items = response.data["items"];
        if (items.empty()) {
            return std::nullopt;
        }
        
        auto fields = response.data["fields"].get<std::vector<std::string>>();
        std::map<std::string, size_t> fieldIndex;
        for (size_t i = 0; i < fields.size(); ++i) {
            fieldIndex[fields[i]] = i;
        }
        
        auto get_double = [&](const auto& item, const std::string& fieldName) -> double {
            auto it = fieldIndex.find(fieldName);
            if (it != fieldIndex.end() && it->second < item.size() && !item[it->second].is_null()) {
                return item[it->second].get<double>();
            }
            return 0.0;
        };
        
        const auto& item = items[0];
        
        FinancialIndicator indicator;
        indicator.ts_code = tsCode;
        indicator.ann_date = tradeDate;
        indicator.end_date = tradeDate;
        indicator.pe = get_double(item, "pe");
        indicator.pe_ttm = get_double(item, "pe_ttm");
        indicator.pb = get_double(item, "pb");
        indicator.total_mv = get_double(item, "total_mv");
        indicator.circ_mv = get_double(item, "circ_mv");
        indicator.turnover_rate = get_double(item, "turnover_rate");
        
        return indicator;
        
    } catch (const std::exception& e) {
        LOG_ERROR("FundamentalStrategy 获取财务数据失败: " + std::string(e.what()));
        return std::nullopt;
    }
}

AnalysisResult FundamentalStrategy::generateSignal(
    const std::string& tsCode,
    const std::string& tradeDate,
    double totalScore,
    const std::string& grade
) const {
    std::string label;
    std::string opt;
    SignalStrength strength = SignalStrength::WEAK;
    double confidence = totalScore;
    std::string warning;
    
    if (totalScore >= strongBuyThreshold_) {
        label = "买入";
        opt = "buy";
        strength = SignalStrength::STRONG;
        warning = "基本面优秀，评级: " + grade + ", 总分: " + std::to_string(static_cast<int>(totalScore));
    } else if (totalScore >= mediumBuyThreshold_) {
        label = "买入";
        opt = "buy";
        strength = SignalStrength::MEDIUM;
        warning = "基本面良好，评级: " + grade + ", 总分: " + std::to_string(static_cast<int>(totalScore));
    } else if (totalScore >= weakBuyThreshold_) {
        label = "买入";
        opt = "buy";
        strength = SignalStrength::WEAK;
        warning = "基本面一般，评级: " + grade + ", 总分: " + std::to_string(static_cast<int>(totalScore));
    } else {
        label = "卖出";
        opt = "sell";
        warning = "基本面较差，评级: " + grade + ", 总分: " + std::to_string(static_cast<int>(totalScore));
    }
    
    return createResult(tsCode, tradeDate, label, strength, confidence, warning, opt);
}

} // namespace core
```

**Step 2: 验证文件创建**

Run: `ls cpp/core/strategies/FundamentalStrategy.cpp`
Expected: 文件存在

**Step 3: Commit**

```bash
git add cpp/core/strategies/FundamentalStrategy.cpp
git commit -m "feat(core): implement FundamentalStrategy"
```

---

## Task 5: 修改 StrategyFactory 注册新策略

**Files:**
- Modify: `cpp/core/StrategyFactory.h`
- Modify: `cpp/core/StrategyFactory.cpp`

**Step 1: 读取现有 StrategyFactory**

先读取现有文件了解结构。

**Step 2: 在 StrategyFactory.cpp 中注册 FundamentalStrategy**

在现有的策略注册代码后添加：

```cpp
// 在 StrategyFactory.cpp 的注册列表中添加
#include "strategies/FundamentalStrategy.h"

// 在 registerBuiltInStrategies() 或构造函数中添加
registerStrategy("FundamentalStrategy", [](const std::map<std::string, double>& params) {
    return std::make_shared<FundamentalStrategy>(params);
});

// 注册 emoji
StrategyEmojiMapper::registerEmoji("FundamentalStrategy", "💎");
```

**Step 3: Commit**

```bash
git add cpp/core/StrategyFactory.h cpp/core/StrategyFactory.cpp
git commit -m "feat(core): register FundamentalStrategy in StrategyFactory"
```

---

## Task 6: 更新 Core.h 头文件

**Files:**
- Modify: `cpp/core/Core.h`

**Step 1: 添加新头文件引用**

```cpp
// 在 Core.h 中添加
#include "StrategyEmojiMapper.h"
#include "strategies/FundamentalStrategy.h"
```

**Step 2: Commit**

```bash
git add cpp/core/Core.h
git commit -m "feat(core): add new headers to Core.h"
```

---

## Task 7: 更新 CMakeLists.txt

**Files:**
- Modify: `cpp/CMakeLists.txt`

**Step 1: 添加新源文件**

找到 `core/` 模块的源文件列表，添加：

```cmake
# 添加新文件
core/StrategyEmojiMapper.cpp
core/strategies/FundamentalStrategy.cpp

# 移除删除的文件（如果有以下条目）
# core/PredictionHistory.cpp  # 删除
# core/AccuracyAnalyzer.cpp   # 删除
# core/ParameterOptimizer.cpp # 删除
```

**Step 2: Commit**

```bash
git add cpp/CMakeLists.txt
git commit -m "chore(build): update CMakeLists.txt for new files"
```

---

## Task 8: 删除自适应学习系统文件

**Files:**
- Delete: `cpp/core/PredictionHistory.h`
- Delete: `cpp/core/PredictionHistory.cpp`
- Delete: `cpp/core/AccuracyAnalyzer.h`
- Delete: `cpp/core/AccuracyAnalyzer.cpp`
- Delete: `cpp/core/ParameterOptimizer.h`
- Delete: `cpp/core/ParameterOptimizer.cpp`

**Step 1: 删除文件**

```bash
rm cpp/core/PredictionHistory.h cpp/core/PredictionHistory.cpp
rm cpp/core/AccuracyAnalyzer.h cpp/core/AccuracyAnalyzer.cpp
rm cpp/core/ParameterOptimizer.h cpp/core/ParameterOptimizer.cpp
```

**Step 2: 验证删除**

Run: `ls cpp/core/PredictionHistory.h 2>&1`
Expected: "No such file or directory"

**Step 3: Commit**

```bash
git add -A cpp/core/
git commit -m "refactor(core): remove adaptive learning system"
```

---

## Task 9: 修改 main.cpp 使用 emoji

**Files:**
- Modify: `cpp/main.cpp`

**Step 1: 找到 analyzeStock 函数中保存结果的位置**

在 `analyzeStock` 函数中，找到保存 `AnalysisResult` 的代码。

**Step 2: 使用 StrategyEmojiMapper 获取 emoji**

在生成结果时，替换 strategy_name 为 emoji：

```cpp
// 原代码
result.strategy_name = strategy->getName();

// 新代码
#include "StrategyEmojiMapper.h"

// 在 initializeStrategies() 后初始化 emoji 映射
StrategyEmojiMapper::initialize();

// 在 analyzeStock 中使用 emoji
result.strategy_name = StrategyEmojiMapper::getEmoji(strategy->getName());
```

**Step 3: 移除 Phase 1 基本面筛选逻辑**

删除 `performFundamentalScreening()` 函数及相关调用。

**Step 4: Commit**

```bash
git add cpp/main.cpp
git commit -m "refactor(main): use emoji for strategy names, remove Phase 1"
```

---

## Task 10: 编译验证

**Files:**
- None (验证步骤)

**Step 1: 编译项目**

```bash
cd cpp/build
cmake ..
make -j4
```

Expected: 编译成功，无错误

**Step 2: 如有编译错误，修复**

常见问题：
- 缺少头文件引用 → 添加到 Core.h
- 链接错误 → 检查 CMakeLists.txt

---

## Task 11: 创建前端 strategyEmoji.ts

**Files:**
- Create: `web-frontend/src/lib/strategyEmoji.ts`

**Step 1: 创建 emoji 映射文件**

```typescript
// web-frontend/src/lib/strategyEmoji.ts

/**
 * 策略 Emoji 映射
 * 用于 hover 提示显示策略含义
 */

export interface StrategyInfo {
  emoji: string;
  name: string;
  description: string;
}

export const STRATEGY_EMOJI_MAP: Record<string, StrategyInfo> = {
  '🔗': { emoji: '🔗', name: 'EMA17TO25Strategy', description: 'EMA17/EMA25 黄金交叉' },
  '⬆️': { emoji: '⬆️', name: 'EMA25Greater17PriceMatchStrategy', description: 'EMA25 > EMA17 价格匹配上涨' },
  '🚀': { emoji: '🚀', name: 'EMA17BreakoutStrategy', description: 'EMA17 突破策略' },
  '🔄': { emoji: '🔄', name: 'EMA25CrossoverStrategy', description: 'EMA25 交叉策略' },
  '🎯': { emoji: '🎯', name: 'EMAConvergenceStrategy', description: 'EMA 收敛策略' },
  '📈': { emoji: '📈', name: 'MACrossStrategy', description: '均线交叉策略' },
  '📊': { emoji: '📊', name: 'MACDStrategy', description: 'MACD 指标策略' },
  '🎢': { emoji: '🎢', name: 'RSIStrategy', description: 'RSI 超买超卖策略' },
  '🎪': { emoji: '🎪', name: 'BOLLStrategy', description: '布林带策略' },
  '🧱': { emoji: '🧱', name: 'GridStrategy', description: '网格交易策略' },
  '💥': { emoji: '💥', name: 'SurgeSignalStrategy', description: '暴涨预警策略' },
  '💎': { emoji: '💎', name: 'FundamentalStrategy', description: '基本面策略' },
};

/**
 * 获取策略描述（用于 hover 提示）
 */
export function getStrategyDescription(emoji: string): string {
  return STRATEGY_EMOJI_MAP[emoji]?.description || emoji;
}

/**
 * 获取策略名称
 */
export function getStrategyName(emoji: string): string {
  return STRATEGY_EMOJI_MAP[emoji]?.name || emoji;
}
```

**Step 2: Commit**

```bash
git add web-frontend/src/lib/strategyEmoji.ts
git commit -m "feat(frontend): add strategy emoji mapping"
```

---

## Task 12: 修改 AnalysisFilter.tsx 添加 hover 提示

**Files:**
- Modify: `web-frontend/src/components/Filter/AnalysisFilter.tsx`

**Step 1: 添加 import**

```typescript
import { getStrategyDescription } from '../../lib/strategyEmoji';
```

**Step 2: 修改策略按钮添加 title**

找到策略按钮渲染代码（约第 176-189 行），添加 title 属性：

```tsx
{strategies.map(strategy => (
  <button
    key={strategy}
    type="button"
    onClick={() => toggleStrategy(strategy)}
    title={getStrategyDescription(strategy)}  // 新增 hover 提示
    className={`px-4 py-2 rounded-lg text-sm font-medium ...`}
  >
    {strategy}
  </button>
))}
```

**Step 3: Commit**

```bash
git add web-frontend/src/components/Filter/AnalysisFilter.tsx
git commit -m "feat(frontend): add hover tooltip for strategy buttons"
```

---

## Task 13: 修改 ChartModal.tsx 添加 hover 提示

**Files:**
- Modify: `web-frontend/src/components/Modal/ChartModal.tsx`

**Step 1: 添加 import**

```typescript
import { getStrategyDescription } from '../../lib/strategyEmoji';
```

**Step 2: 修改策略选择按钮添加 title**

找到策略选择按钮代码（约第 182-196 行），添加 title 属性：

```tsx
{strategies.map(s => (
  <button
    key={s}
    onClick={() => {
      onStrategyChange(s)
      setDrillStack([])
    }}
    title={getStrategyDescription(s)}  // 新增 hover 提示
    className={`px-3 py-1.5 text-xs font-medium rounded-md ...`}
  >
    {s}
  </button>
))}
```

**Step 3: Commit**

```bash
git add web-frontend/src/components/Modal/ChartModal.tsx
git commit -m "feat(frontend): add hover tooltip in ChartModal"
```

---

## Task 14: 运行分析验证

**Files:**
- None (验证步骤)

**Step 1: 运行 C++ 分析程序**

```bash
cd cpp/build
./stock_for_cpp --once
```

Expected: 
- 分析完成
- 数据库中 strategy_name 字段存储 emoji
- FundamentalStrategy 生成买入/卖出信号

**Step 2: 检查数据库**

```sql
SELECT DISTINCT strategy_name FROM analysis_results LIMIT 20;
```

Expected: 看到 emoji 字符（🔗、⬆️、💎 等）

**Step 3: 运行前端验证**

```bash
cd web-frontend
npm run dev
```

访问页面，验证：
- 策略按钮显示 emoji
- hover 显示策略描述

---

## Task 15: 最终提交

**Step 1: 检查所有变更**

```bash
git status
git log --oneline -15
```

**Step 2: 推送**

```bash
git push origin master
```

---

**计划完成，保存到 `cpp/docs/plans/2026-05-23-fundamental-strategy-implementation.md`**