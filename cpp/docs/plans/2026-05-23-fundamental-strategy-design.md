# 基本面策略化 + Emoji 设计文档

## 变更目标

1. **移除自适应学习系统**：删除 `PredictionHistory`、`AccuracyAnalyzer`、`ParameterOptimizer` 三个模块及相关调用
2. **基本面策略化**：将 `FundamentalFilter` 改造为 `FundamentalStrategy`，作为完整的交易策略
3. **策略 Emoji 映射**：所有策略使用 emoji 标识，写入数据库和前端展示

---

## 架构变更图

```
变更前：
┌─────────────────────────────────────────────────────────┐
│  main.cpp                                               │
│  ├─ Phase 1: performFundamentalScreening()              │
│  │   └─ FundamentalFilter → 筛选优质股票池               │
│  ├─ Phase 2: performBatchAnalysis()                     │
│  │   └─ StrategyManager.analyzeAll() → 11种技术策略     │
│  └─ 自适应学习系统（未实际调用）                          │
│      ├─ PredictionHistory                               │
│      ├─ AccuracyAnalyzer                                │
│      └─ ParameterOptimizer                              │
└─────────────────────────────────────────────────────────┘

变更后：
┌─────────────────────────────────────────────────────────┐
│  main.cpp                                               │
│  ├─ performBatchAnalysis()                              │
│  │   └─ StrategyManager.analyzeAll()                    │
│  │       ├─ 11种技术策略（保持不变）                      │
│  │       └─ FundamentalStrategy（新增）                  │
│  │           └─ 调用 FundamentalFilter 计算评分          │
│  │           └─ 根据阈值生成买入/卖出/持有信号            │
│  └─ 自适应学习系统（已删除）                              │
└─────────────────────────────────────────────────────────┘
```

---

## 策略 Emoji 映射表

| 策略名 | Emoji | 含义 | 设计理由 |
|--------|-------|------|----------|
| EMA17TO25Strategy | 🔗 | 黄金交叉链接 | EMA17 与 EMA25 交叉连接 |
| EMA25Greater17PriceMatchStrategy | ⬆️ | 价格匹配上涨 | EMA25 > EMA17，价格匹配上涨趋势 |
| EMA17BreakoutStrategy | 🚀 | 突破起飞 | EMA17 突破，突破即起飞 |
| EMA25CrossoverStrategy | 🔄 | 交叉循环 | EMA25 交叉信号 |
| EMAConvergenceStrategy | 🎯 | 收敛精准 | EMA 收敛，精准定位 |
| MACrossStrategy | 📈 | 均线上涨 | 均线交叉，上涨信号 |
| MACDStrategy | 📊 | MACD图表 | MACD 指标分析 |
| RSIStrategy | 🎢 | RSI波动 | RSI 超买超卖，波动信号 |
| BOLLStrategy | 🎪 | 布林带通道 | 布林带通道边界 |
| GridStrategy | 🧱 | 网格结构 | 网格交易策略 |
| SurgeSignalStrategy | 💥 | 暴涨爆发 | 暴涨预警，爆发信号 |
| FundamentalStrategy | 💎 | 基本面钻石 | 基本面优质，钻石品质 |

---

## FundamentalStrategy 信号阈值

| 总分范围 | 信号 | strength | label | 说明 |
|---------|------|----------|-------|------|
| ≥80分 | 买入 | STRONG | 强买入 | 基本面优秀 |
| 60-79分 | 买入 | MEDIUM | 中买入 | 基本面良好 |
| 40-59分 | 买入 | WEAK | 弱买入 | 基本面一般 |
| <40分 | 卖出 | - | 卖出 | 基本面较差 |
| 数据缺失 | 持有 | - | 持有 | 无法评分 |

---

## 文件变更清单

### C++ 后端

| 操作 | 文件 | 说明 |
|------|------|------|
| 新增 | `core/strategies/FundamentalStrategy.h` | 基本面策略头文件 |
| 新增 | `core/strategies/FundamentalStrategy.cpp` | 基本面策略实现 |
| 新增 | `core/StrategyEmojiMapper.h` | Emoji 映射头文件 |
| 新增 | `core/StrategyEmojiMapper.cpp` | Emoji 映射实现 |
| 修改 | `core/StrategyFactory.h/cpp` | 注册 FundamentalStrategy 和 emoji |
| 修改 | `core/Core.h` | 添加头文件引用 |
| 修改 | `main.cpp` | 移除 Phase 1，移除自适应学习调用 |
| 修改 | `CMakeLists.txt` | 添加新文件，移除删除文件 |
| 删除 | `core/PredictionHistory.h/cpp` | 自适应学习模块 |
| 删除 | `core/AccuracyAnalyzer.h/cpp` | 自适应学习模块 |
| 删除 | `core/ParameterOptimizer.h/cpp` | 自适应学习模块 |

### 前端

| 操作 | 文件 | 说明 |
|------|------|------|
| 新增 | `src/lib/strategyEmoji.ts` | Emoji → 描述映射 |
| 修改 | `src/components/Filter/AnalysisFilter.tsx` | 添加 title hover 提示 |
| 修改 | `src/components/Modal/ChartModal.tsx` | 添加 title hover 提示 |

### NestJS API (无需改动)

- 直接读取数据库 emoji 字段，自动返回给前端

---

## 数据流

```
C++ 分析引擎
├─ StrategyEmojiMapper.getEmoji("FundamentalStrategy") → "💎"
├─ 写入数据库: strategy_name = "💎"
│
NestJS API
├─ AnalysisService.getStrategies() → ["💎", "🔗", "⬆️", ...]
├─ 返回 emoji 列表给前端
│
React 前端
├─ AnalysisFilter.tsx → 显示 emoji 按钮 + hover 提示
├─ ChartModal.tsx → 显示 emoji 按钮 + hover 提示
```

---

## FundamentalStrategy 类结构

```cpp
class FundamentalStrategy : public StrategyBase {
public:
    FundamentalStrategy();
    
    std::string getName() const override { return "FundamentalStrategy"; }
    std::string getDescription() const override;
    
    std::optional<AnalysisResult> analyze(
        const std::string& tsCode,
        const std::vector<StockData>& data
    ) override;
    
    void setParameters(const std::map<std::string, double>& params) override;
    
private:
    double strongBuyThreshold_ = 80.0;
    double mediumBuyThreshold_ = 60.0;
    double weakBuyThreshold_ = 40.0;
    double sellThreshold_ = 40.0;
    
    FundamentalFilter filter_;
    
    std::optional<FinancialIndicator> getFinancialIndicator(...);
    AnalysisResult generateSignal(...);
};
```

---

## StrategyEmojiMapper 类结构

```cpp
class StrategyEmojiMapper {
public:
    static void initialize();
    static std::string getEmoji(const std::string& strategyName);
    static bool isRegistered(const std::string& strategyName);
    
private:
    static std::map<std::string, std::string> emojiMap_;
    static bool initialized_;
};

// 默认映射表
const std::map<std::string, std::string> DEFAULT_EMOJI_MAP = {
    {"EMA17TO25Strategy",            "🔗"},
    {"EMA25Greater17PriceMatchStrategy", "⬆️"},
    {"EMA17BreakoutStrategy",        "🚀"},
    {"EMA25CrossoverStrategy",       "🔄"},
    {"EMAConvergenceStrategy",       "🎯"},
    {"MACrossStrategy",              "📈"},
    {"MACDStrategy",                 "📊"},
    {"RSIStrategy",                  "🎢"},
    {"BOLLStrategy",                 "🎪"},
    {"GridStrategy",                 "🧱"},
    {"SurgeSignalStrategy",          "💥"},
    {"FundamentalStrategy",          "💎"}
};
```

---

## 错误处理

### FundamentalStrategy

| 场景 | 处理方式 | 返回值 |
|------|----------|--------|
| 无法获取财务数据 | 返回 std::nullopt | 不生成信号 |
| PE/PB 为负值或异常 | 该维度评分 = 0 | 继续计算其他维度 |
| 数据缺失字段过多 | 返回 std::nullopt | 不生成信号 |
| API 调用超时 | 使用缓存或返回 nullopt | |

### StrategyEmojiMapper

| 场景 | 处理方式 | 返回值 |
|------|----------|--------|
| 策略未注册 emoji | 返回原始策略名 | strategyName |
| 空策略名 | 返回空字符串 | "" |

---

## 验证要点

1. FundamentalStrategy 能正确生成买卖信号
2. 所有策略 emoji 正确写入数据库
3. 前端 hover 提示正确显示策略描述
4. 移除自适应学习后编译无错误
5. 筛选功能仍能按策略筛选

---

**创建日期**: 2026-05-23
**状态**: 已批准