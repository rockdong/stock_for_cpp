# 智能股票筛选系统设计方案

## 1. 设计概述

### 1.1 目标
设计一个综合股票筛选系统，能够：
- **追踪市场热点**：识别当前热门板块、概念、龙头股
- **综合评分筛选**：结合基本面、技术面、资金流向、热点等多维度评分
- **风险过滤**：排除停牌、ST、退市等风险股票
- **输出推荐**：生成可买入股票列表及详细分析报告

### 1.2 系统架构

```
┌─────────────────────────────────────────────────────────────────────────┐
│                      智能股票筛选系统 (Stock Selector)                     │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                           │
│  ┌──────────────────┐  ┌──────────────────┐  ┌──────────────────────┐  │
│  │   数据获取层      │  │   分析处理层      │  │   输出展示层          │  │
│  │                  │  │                  │  │                      │  │
│  │ • TushareClient  │  │ • HotspotTracker │  │ • ResultExporter     │  │
│  │ • 数据缓存       │  │ • CompositeScorer│  │ • ReportGenerator    │  │
│  │ • 数据验证       │  │ • RiskFilter     │  │ • AlertNotifier      │  │
│  └──────────────────┘  └──────────────────┘  └──────────────────────┘  │
│           │                     │                      │                 │
│           └─────────────────────┼──────────────────────┘                 │
│                                 │                                        │
│  ┌──────────────────────────────┴──────────────────────────────────┐   │
│  │                      核心数据结构层                                │   │
│  │                                                                   │   │
│  │  Stock, StockData, FinancialIndicator, MoneyFlow,                │   │
│  │  SuspendInfo, Forecast, Express, Dividend                        │   │
│  │  HotspotInfo, SectorScore, CompositeScore, SelectionResult       │   │
│  └───────────────────────────────────────────────────────────────────┘   │
│                                                                           │
└─────────────────────────────────────────────────────────────────────────┘
```

---

## 2. 热点追踪系统设计

### 2.1 核心概念

热点追踪系统负责识别市场热点，包括：
- **行业热点**：涨幅领先的行业板块
- **概念热点**：当前热门概念题材
- **个股热点**：涨幅榜、涨停板、龙虎榜个股

### 2.2 新增数据结构

```cpp
// core/HotspotData.h

namespace core {

/**
 * @brief 行业热点评分
 */
struct SectorHotspot {
    std::string sector_name;        // 行业名称
    double avg_change_pct;          // 行业平均涨跌幅
    int leading_count;              // 领涨股数量
    double total_amount;            // 行业总成交额
    double net_inflow;              // 资金净流入
    int hotspot_score;              // 热点评分 (0-100)
    std::string hotspot_level;      // 热度等级: HOT/WARM/COLD
    std::vector<std::string> leading_stocks;  // 领涨股票列表
};

/**
 * @brief 个股热点评分
 */
struct StockHotspot {
    std::string ts_code;            // 股票代码
    std::string sector;             // 所属行业
    
    // 涨幅维度
    double pct_change_1d;           // 1日涨跌幅
    double pct_change_5d;           // 5日涨跌幅
    double pct_change_20d;          // 20日涨跌幅
    
    // 资金维度
    double net_inflow_1d;           // 1日净流入
    double net_inflow_5d;           // 5日净流入
    double main_inflow_ratio;       // 主力资金占比
    
    // 量价维度
    double volume_ratio;            // 量比（当日/5日均量）
    double turnover_rate;           // 换手率
    
    // 热点评分
    int hotspot_score = 0;          // 热点评分 (0-100)
    std::string hotspot_type;       // 热点类型: SURGE/ACTIVE/DORMANT
    
    // 龙头标识
    bool is_sector_leader = false;  // 是否板块龙头
    bool is_concept_leader = false; // 是否概念龙头
};

/**
 * @brief 热点追踪结果
 */
struct HotspotTrackingResult {
    std::string tracking_date;      // 追踪日期
    
    // 行业热点排行
    std::vector<SectorHotspot> hot_sectors;      // 热门行业（前10）
    std::vector<SectorHotspot> warm_sectors;     // 温热行业
    
    // 个股热点排行
    std::vector<StockHotspot> surge_stocks;      // 暴涨股（涨幅>5%）
    std::vector<StockHotspot> active_stocks;     // 活跃股
    std::vector<StockHotspot> leader_stocks;     // 龙头股
    
    // 概念热点（需要外部数据源）
    std::vector<std::string> hot_concepts;       // 热门概念列表
    
    int total_hot_count;           // 热点股票总数
};

} // namespace core
```

### 2.3 热点追踪器类设计

```cpp
// analysis/HotspotTracker.h

namespace analysis {

/**
 * @brief 热点追踪器
 * 
 * 负责识别市场热点，包括行业热点、概念热点、个股热点
 */
class HotspotTracker {
public:
    explicit HotspotTracker(network::DataSourcePtr dataSource);
    
    /**
     * @brief 追踪市场热点（每日执行）
     * @param trade_date 交易日期
     * @return 热点追踪结果
     */
    core::HotspotTrackingResult track(const std::string& trade_date);
    
    /**
     * @brief 计算行业热点评分
     * @param sector_stocks 行业内所有股票
     * @return 行业热点评分
     */
    core::SectorHotspot evaluateSector(
        const std::string& sector,
        const std::vector<core::Stock>& stocks,
        const std::string& trade_date
    );
    
    /**
     * @brief 计算个股热点评分
     * @param ts_code 股票代码
     * @param trade_date 交易日期
     * @return 个股热点评分
     */
    core::StockHotspot evaluateStock(
        const std::string& ts_code,
        const std::string& trade_date
    );
    
    /**
     * @brief 获取板块龙头股
     * @param sector 行业名称
     * @return 龙头股代码
     */
    std::string getSectorLeader(const std::string& sector);
    
    /**
     * @brief 设置热点阈值参数
     */
    void setThresholds(
        double surge_threshold = 5.0,     // 暴涨阈值
        double active_threshold = 3.0,    // 活跃阈值
        double volume_ratio_min = 2.0,    // 量比最低值
        double main_inflow_min = 0.3      // 主力流入占比最低值
    );

private:
    network::DataSourcePtr dataSource_;
    
    // 热点阈值参数
    struct Thresholds {
        double surge_threshold = 5.0;
        double active_threshold = 3.0;
        double volume_ratio_min = 2.0;
        double main_inflow_min = 0.3;
    } thresholds_;
    
    // 缓存数据
    std::map<std::string, std::vector<core::StockData>> stockDataCache_;
    std::map<std::string, std::vector<core::MoneyFlow>> moneyFlowCache_;
    
    /**
     * @brief 计算行业统计指标
     */
    struct SectorStats {
        double avg_change;
        double total_amount;
        double net_inflow;
        int leading_count;
    };
    SectorStats calculateSectorStats(
        const std::vector<core::Stock>& stocks,
        const std::string& trade_date
    );
    
    /**
     * @brief 计算热点评分算法
     */
    int calculateHotspotScore(
        double change_pct,
        double volume_ratio,
        double main_inflow_ratio,
        double turnover_rate
    );
};

} // namespace analysis
```

### 2.4 热点评分算法

```
热点评分 = 涨幅权重 + 量价权重 + 资金权重 + 趋势权重

1. 涨幅权重 (30分):
   - 当日涨幅 ≥ 5%: 15分
   - 5日涨幅 ≥ 10%: 10分
   - 20日涨幅 ≥ 20%: 5分

2. 量价权重 (25分):
   - 量比 ≥ 2.0: 10分
   - 换手率 5%-15%: 8分
   - 价涨量增: 7分

3. 资金权重 (30分):
   - 主力净流入 > 0: 10分
   - 大单净流入占比 > 30%: 10分
   - 5日持续流入: 10分

4. 趋势权重 (15分):
   - EMA17 > EMA25: 5分
   - MACD金叉: 5分
   - RSI超买区间: 5分
```

---

## 3. 综合评分模型设计

### 3.1 多因子评分体系

```cpp
// analysis/CompositeScorer.h

namespace analysis {

/**
 * @brief 综合评分配置
 */
struct ScorerConfig {
    // 因子权重配置
    double fundamental_weight = 0.30;    // 基本面权重
    double technical_weight = 0.25;      // 技术面权重
    double hotspot_weight = 0.25;        // 热点权重
    double money_flow_weight = 0.20;     // 资金流向权重
    
    // 风险惩罚权重
    double suspend_penalty = 0.5;        // 停牌惩罚（扣除50%）
    double st_penalty = 0.3;             // ST惩罚（扣除30%）
    double high_pe_penalty = 0.2;        // 高PE惩罚
    
    // 筛选阈值
    double min_total_score = 70.0;       // 最低总分
    double min_fundamental = 60.0;       // 最低基本面分
};

/**
 * @brief 综合评分结果
 */
struct CompositeScore {
    std::string ts_code;             // 股票代码
    std::string trade_date;          // 评分日期
    
    // 各维度得分
    double fundamental_score = 0.0;  // 基本面得分 (0-100)
    double technical_score = 0.0;    // 技术面得分 (0-100)
    double hotspot_score = 0.0;      // 热点得分 (0-100)
    double money_flow_score = 0.0;   // 资金流向得分 (0-100)
    
    // 加权总分
    double total_score = 0.0;        // 综合总分 (0-100)
    
    // 评级
    std::string grade;               // 评级: A+/A/B+/B/C/D
    bool is_recommended = false;     // 是否推荐买入
    
    // 风险标识
    bool has_suspend_risk = false;   // 停牌风险
    bool has_st_risk = false;        // ST风险
    bool has_high_valuation = false; // 高估值风险
    
    // 详细说明
    std::string recommendation_reason;  // 推荐理由
    std::string risk_warning;           // 风险提示
};

/**
 * @brief 综合评分器
 * 
 * 整合基本面、技术面、热点、资金流向等多维度评分
 */
class CompositeScorer {
public:
    explicit CompositeScorer(const ScorerConfig& config = {});
    
    /**
     * @brief 对单只股票进行综合评分
     * @param ts_code 股票代码
     * @param trade_date 交易日期
     * @param dataSource 数据源
     * @return 综合评分结果
     */
    CompositeScore score(
        const std::string& ts_code,
        const std::string& trade_date,
        network::DataSourcePtr dataSource
    );
    
    /**
     * @brief 批量评分（并行处理）
     * @param stocks 股票列表
     * @param trade_date 交易日期
     * @return 评分结果列表
     */
    std::vector<CompositeScore> batchScore(
        const std::vector<std::string>& ts_codes,
        const std::string& trade_date,
        network::DataSourcePtr dataSource
    );
    
    /**
     * @brief 获取推荐股票列表
     * @param scores 评分结果
     * @return 推荐股票列表（按评分排序）
     */
    std::vector<CompositeScore> getRecommended(
        const std::vector<CompositeScore>& scores
    );

private:
    ScorerConfig config_;
    
    /**
     * @brief 计算基本面评分（使用现有 FundamentalScore）
     */
    double calculateFundamentalScore(
        const core::FinancialIndicator& indicator
    );
    
    /**
     * @brief 计算技术面评分
     */
    double calculateTechnicalScore(
        const std::vector<core::StockData>& data
    );
    
    /**
     * @brief 计算资金流向评分
     */
    double calculateMoneyFlowScore(
        const std::vector<core::MoneyFlow>& flows
    );
    
    /**
     * @brief 应用风险惩罚
     */
    double applyRiskPenalty(
        double score,
        const core::SuspendInfo& suspend,
        const core::FinancialIndicator& indicator
    );
    
    /**
     * @brief 计算加权总分
     */
    double calculateTotalScore(const CompositeScore& score);
    
    /**
     * @brief 生成评级
     */
    std::string generateGrade(double total_score);
};

} // namespace analysis
```

### 3.2 技术面评分算法

```cpp
// analysis/TechnicalScorer.cpp

double CompositeScorer::calculateTechnicalScore(
    const std::vector<core::StockData>& data
) {
    if (data.size() < 30) return 0.0;
    
    double score = 0.0;
    
    // 1. EMA趋势评分 (25分)
    auto ema17 = analysis::EMA::compute(prices, 17);
    auto ema25 = analysis::EMA::compute(prices, 25);
    if (ema17.back() > ema25.back()) {
        score += 15;  // EMA17 > EMA25
        if (ema17[ema17.size()-5] < ema25[ema25.size()-5]) {
            score += 10;  // 近期金叉
        }
    }
    
    // 2. MACD评分 (25分)
    auto macd = analysis::MACD::compute(prices, 12, 26, 9);
    if (macd.histogram.back() > 0) {
        score += 15;  // MACD柱为正
        if (macd.macd.back() > macd.signal.back()) {
            score += 10;  // MACD金叉
        }
    }
    
    // 3. RSI评分 (20分)
    auto rsi = analysis::RSI::compute(prices, 14);
    double rsi_val = rsi.back();
    if (rsi_val >= 30 && rsi_val <= 70) {
        score += 20;  // 正常区间
    } else if (rsi_val < 30) {
        score += 10;  // 超卖（有反弹机会）
    }
    
    // 4. 布林带评分 (15分)
    auto boll = analysis::BOLL::compute(prices, 20, 2.0, 2.0);
    double current_price = prices.back();
    if (current_price > boll.middle.back()) {
        score += 10;  // 价格在布林中轨之上
        if (current_price < boll.upper.back()) {
            score += 5;  // 未突破上轨（未过热）
        }
    }
    
    // 5. KDJ评分 (15分)
    auto kdj = analysis::KDJ::compute(highs, lows, closes, 9, 3, 3);
    if (kdj.k.back() > kdj.d.back()) {
        score += 8;  // K > D
        if (kdj.j.back() < 100) {
            score += 7;  // J未超买
        }
    }
    
    return std::min(score, 100.0);
}
```

### 3.3 资金流向评分算法

```cpp
// analysis/MoneyFlowScorer.cpp

double CompositeScorer::calculateMoneyFlowScore(
    const std::vector<core::MoneyFlow>& flows
) {
    if (flows.empty()) return 0.0;
    
    double score = 0.0;
    
    // 最近5日资金流向数据
    size_t recent = std::min(flows.size(), static_cast<size_t>(5));
    
    // 1. 主力资金净流入 (40分)
    double total_main_inflow = 0.0;
    for (size_t i = 0; i < recent; ++i) {
        // 大单 + 特大单净流入
        double main_in = flows[i].buy_lg_amount + flows[i].buy_elg_amount;
        double main_out = flows[i].sell_lg_amount + flows[i].sell_elg_amount;
        total_main_inflow += (main_in - main_out);
    }
    if (total_main_inflow > 0) {
        score += 30;
        // 持续流入加分
        int inflow_days = 0;
        for (size_t i = 0; i < recent; ++i) {
            double net = flows[i].net_mf_amount;
            if (net > 0) inflow_days++;
        }
        if (inflow_days >= 3) score += 10;
    }
    
    // 2. 主力资金占比 (30分)
    double latest = flows.back();
    double total_amount = latest.buy_sm_amount + latest.buy_md_amount + 
                          latest.buy_lg_amount + latest.buy_elg_amount;
    double main_ratio = (latest.buy_lg_amount + latest.buy_elg_amount) / total_amount;
    if (main_ratio >= 0.4) {
        score += 20;  // 主力占比高
        if (main_ratio >= 0.6) score += 10;
    }
    
    // 3. 散户行为分析 (30分)
    // 散户卖出往往意味着主力吸筹
    double retail_net = latest.sell_sm_amount - latest.buy_sm_amount;
    if (retail_net > 0 && total_main_inflow > 0) {
        score += 20;  // 散户出，主力进
        score += 10;  // 典型吸筹模式
    }
    
    return std::min(score, 100.0);
}
```

---

## 4. 风险过滤器设计

### 4.1 风险过滤类

```cpp
// analysis/RiskFilter.h

namespace analysis {

/**
 * @brief 风险类型枚举
 */
enum class RiskType {
    SUSPENDED,          // 停牌
    ST_STOCK,           // ST股票
    DELISTING,          // 退市风险
    HIGH_PEB,           // 高估值
    FINANCIAL_RISK,     // 财务风险
    LOW_VOLUME,         // 流动性不足
    CONSECUTIVE_LOSS    // 连续亏损
};

/**
 * @brief 风险检测结果
 */
struct RiskCheckResult {
    std::string ts_code;
    bool has_risk = false;
    std::vector<RiskType> risk_types;
    std::string risk_description;
    double risk_score = 0.0;  // 风险程度 (0-1)
};

/**
 * @brief 风险过滤器
 * 
 * 过滤存在风险的股票，避免推荐
 */
class RiskFilter {
public:
    explicit RiskFilter(network::DataSourcePtr dataSource);
    
    /**
     * @brief 检查单只股票风险
     * @param ts_code 股票代码
     * @param trade_date 交易日期
     * @return 风险检测结果
     */
    RiskCheckResult checkRisk(
        const std::string& ts_code,
        const std::string& trade_date
    );
    
    /**
     * @brief 批量风险检查
     * @param ts_codes 股票代码列表
     * @param trade_date 交易日期
     * @return 风险股票列表（需要过滤）
     */
    std::vector<std::string> filterRisky(
        const std::vector<std::string>& ts_codes,
        const std::string& trade_date
    );
    
    /**
     * @brief 设置风险阈值
     */
    void setRiskThresholds(
        double max_pe = 100.0,           // PE上限
        double max_pb = 20.0,            // PB上限
        double min_volume = 10000,       // 最小成交量
        int max_consecutive_loss = 3     // 最大连续亏损季数
    );

private:
    network::DataSourcePtr dataSource_;
    
    struct RiskThresholds {
        double max_pe = 100.0;
        double max_pb = 20.0;
        double min_volume = 10000;
        int max_consecutive_loss = 3;
    } thresholds_;
    
    /**
     * @brief 检查停牌风险
     */
    bool checkSuspendRisk(const std::string& ts_code, const std::string& date);
    
    /**
     * @brief 检查ST风险
     */
    bool checkSTRisk(const std::string& ts_code);
    
    /**
     * @brief 检查估值风险
     */
    bool checkValuationRisk(const core::FinancialIndicator& indicator);
    
    /**
     * @brief 检查财务风险
     */
    bool checkFinancialRisk(const core::FinancialIndicator& indicator);
};

} // namespace analysis
```

---

## 5. 筛选流程设计

### 5.1 完整筛选流程

```cpp
// core/StockSelector.h

namespace core {

/**
 * @brief 筛选结果
 */
struct SelectionResult {
    std::string selection_date;         // 筛选日期
    int total_candidates;               // 候选股票总数
    int passed_fundamental;             // 基本面通过数
    int passed_technical;               // 技术面通过数
    int passed_hotspot;                 // 热点通过数
    int passed_risk_filter;             // 风险过滤通过数
    
    // 最终推荐列表
    std::vector<analysis::CompositeScore> recommended_stocks;
    
    // 热点分析摘要
    analysis::HotspotTrackingResult hotspot_summary;
    
    // 统计信息
    std::map<std::string, int> sector_distribution;  // 行业分布
    std::string top_sector;                          // 最热门行业
    std::string top_concept;                         // 最热门概念
    
    // 执行时间
    double execution_time_ms;         // 执行耗时
};

/**
 * @brief 股票筛选器
 * 
 * 主控制器，协调热点追踪、综合评分、风险过滤等模块
 */
class StockSelector {
public:
    explicit StockSelector(network::DataSourcePtr dataSource);
    
    /**
     * @brief 执行完整筛选流程
     * @param trade_date 交易日期（默认当天）
     * @return 筛选结果
     */
    SelectionResult select(const std::string& trade_date = "");
    
    /**
     * @brief 快速筛选（仅热点+基本面）
     * @param trade_date 交易日期
     * @return 快速筛选结果
     */
    std::vector<std::string> quickSelect(const std::string& trade_date = "");
    
    /**
     * @brief 配置筛选参数
     */
    void configure(
        const analysis::ScorerConfig& scorer_config,
        const analysis::RiskFilter::RiskThresholds& risk_thresholds,
        const analysis::HotspotTracker::Thresholds& hotspot_thresholds
    );
    
    /**
     * @brief 设置输出回调
     */
    void setOutputCallback(std::function<void(const SelectionResult&)> callback);

private:
    network::DataSourcePtr dataSource_;
    
    // 子模块
    std::unique_ptr<analysis::HotspotTracker> hotspotTracker_;
    std::unique_ptr<analysis::CompositeScorer> scorer_;
    std::unique_ptr<analysis::RiskFilter> riskFilter_;
    
    // 配置
    analysis::ScorerConfig scorerConfig_;
    
    // 输出回调
    std::function<void(const SelectionResult&)> outputCallback_;
    
    /**
     * @brief 执行筛选流程
     */
    SelectionResult executeSelection(const std::string& trade_date);
    
    /**
     * @brief 步骤1：获取股票池
     */
    std::vector<Stock> getStockPool();
    
    /**
     * @brief 步骤2：追踪热点
     */
    analysis::HotspotTrackingResult trackHotspots(const std::string& date);
    
    /**
     * @brief 步骤3：基本面筛选
     */
    std::vector<std::string> filterByFundamental(
        const std::vector<Stock>& stocks
    );
    
    /**
     * @brief 步骤4：技术面筛选
     */
    std::vector<std::string> filterByTechnical(
        const std::vector<std::string>& candidates,
        const std::string& date
    );
    
    /**
     * @brief 步骤5：热点筛选
     */
    std::vector<std::string> filterByHotspot(
        const std::vector<std::string>& candidates,
        const analysis::HotspotTrackingResult& hotspot
    );
    
    /**
     * @brief 步骤6：风险过滤
     */
    std::vector<std::string> filterByRisk(
        const std::vector<std::string>& candidates,
        const std::string& date
    );
    
    /**
     * @brief 步骤7：综合评分排序
     */
    std::vector<analysis::CompositeScore> finalScoring(
        const std::vector<std::string>& candidates,
        const std::string& date
    );
};

} // namespace core
```

### 5.2 筛选流程图

```
筛选流程（7步骤）:

步骤1: 获取股票池
  └─→ 获取所有上市股票（排除退市）
      └─→ 结果: 3000+ 只股票

步骤2: 追踪热点
  └─→ 行业热点评分（涨幅榜、资金流向）
  └─→ 个股热点评分（量比、换手率、资金）
  └─→ 结果: HotspotTrackingResult

步骤3: 基本面筛选（第一层过滤）
  └─→ 使用现有 FundamentalScore 评分
  └─→ 条件: 总分 ≥ 60 且 ROE ≥ 10
  └─→ 结果: ~500 只股票（约15%通过率）

步骤4: 技术面筛选（第二层过滤）
  └─→ EMA趋势向上（EMA17 > EMA25）
  └─→ MACD金叉或即将金叉
  └─→ RSI在正常区间（30-70）
  └─→ 结果: ~200 只股票（约40%通过率）

步骤5: 热点筛选（第三层过滤）
  └─→ 热点评分 ≥ 50
  └─→ 或属于热门行业（前5名）
  └─→ 结果: ~100 只股票（约50%通过率）

步骤6: 风险过滤（第四层过滤）
  └─→ 排除停牌股票
  └─→ 排除ST股票
  └─→ 排除高估值（PE > 100）
  └─→ 排除财务风险（连续亏损）
  └─→ 结果: ~80 只股票（约20%被过滤）

步骤7: 综合评分排序（最终输出）
  └─→ 计算综合评分（加权）
  └─→ 按评分降序排序
  └─→ 输出Top 20推荐股票
  └─→ 结果: 20 只推荐股票
```

---

## 6. 与现有系统的集成

### 6.1 模块依赖关系

```
StockSelector（新增模块）
    │
    ├─── HotspotTracker（新增）
    │       └─── IDataSource（已有）
    │       └─── MoneyFlow（已有数据结构）
    │
    ├─── CompositeScorer（新增）
    │       └─── FundamentalScore（已有）
    │       └─── Technical Indicators（已有）
    │       │       ├─── MA, EMA（已有）
    │       │       ├─── MACD, RSI（已有）
    │       │       ├─── KDJ, BOLL（已有）
    │       └─── MoneyFlow（已有）
    │
    ├─── RiskFilter（新增）
    │       └─── SuspendInfo（已有）
    │       └─── FinancialIndicator（已有）
    │
    └─── SurgeSignalStrategy（已有）
            └─── 可作为技术面评分的补充
```

### 6.2 数据流设计

```
数据获取流程:

Tushare API
    │
    ├─── daily_quote (行情数据) → StockData
    │
    ├─── fina_indicator (财务指标) → FinancialIndicator
    │
    ├─── moneyflow (资金流向) → MoneyFlow
    │
    ├─── suspend_d (停复牌) → SuspendInfo
    │
    ├─── forecast (业绩预告) → Forecast
    │
    ├─── express (业绩快报) → Express
    │
    └─── dividend (分红) → Dividend
    │
    ▼
数据缓存层（MemoryCache）
    │
    ▼
分析处理层
    │
    ├─── HotspotTracker（热点追踪）
    │
    ├─── CompositeScorer（综合评分）
    │
    └─── RiskFilter（风险过滤）
    │
    ▼
输出层
    │
    ├─── SelectionResult（筛选结果）
    │
    └─── ReportGenerator（报告生成）
```

---

## 7. 配置管理

### 7.1 环境变量配置

```env
# ========== 股票筛选配置 ==========

# 热点追踪阈值
HOTSPOT_SURGE_THRESHOLD=5.0        # 暴涨阈值（%）
HOTSPOT_ACTIVE_THRESHOLD=3.0       # 活跃阈值（%）
HOTSPOT_VOLUME_RATIO_MIN=2.0       # 量比最低值
HOTSPOT_MAIN_INFLOW_MIN=0.3        # 主力流入占比最低值

# 综合评分权重
SCORER_FUNDAMENTAL_WEIGHT=0.30     # 基本面权重
SCORER_TECHNICAL_WEIGHT=0.25       # 技术面权重
SCORER_HOTSPOT_WEIGHT=0.25         # 热点权重
SCORER_MONEY_FLOW_WEIGHT=0.20      # 资金流向权重

# 筛选阈值
SCORER_MIN_TOTAL_SCORE=70.0        # 最低总分
SCORER_MIN_FUNDAMENTAL=60.0        # 最低基本面分
SCORER_MIN_HOTSPOT=50.0            # 最低热点分

# 风险过滤阈值
RISK_MAX_PE=100.0                  # PE上限
RISK_MAX_PB=20.0                   # PB上限
RISK_MIN_VOLUME=10000              # 最小成交量
RISK_MAX_CONSECUTIVE_LOSS=3        # 最大连续亏损季数

# 输出配置
SELECTOR_TOP_COUNT=20              # 推荐股票数量
SELECTOR_REPORT_FORMAT=json        # 报告格式
SELECTOR_ENABLE_ALERT=true         # 启用告警推送
```

---

## 8. 输出格式设计

### 8.1 筛选结果输出

```cpp
// output/SelectionReport.h

namespace output {

/**
 * @brief 筛选报告生成器
 */
class SelectionReportGenerator {
public:
    /**
     * @brief 生成JSON格式报告
     */
    static std::string generateJson(const core::SelectionResult& result);
    
    /**
     * @brief 生成HTML格式报告
     */
    static std::string generateHtml(const core::SelectionResult& result);
    
    /**
     * @brief 生成飞书消息格式
     */
    static std::string generateFeishuMessage(const core::SelectionResult& result);
    
    /**
     * @brief 生成CSV格式（股票列表）
     */
    static std::string generateCsv(const core::SelectionResult& result);
};

} // namespace output
```

### 8.2 推荐股票输出格式

```json
{
  "selection_date": "20260505",
  "execution_time_ms": 15234,
  "summary": {
    "total_candidates": 3500,
    "passed_fundamental": 520,
    "passed_technical": 210,
    "passed_hotspot": 105,
    "passed_risk_filter": 85,
    "recommended_count": 20
  },
  "hotspot_analysis": {
    "hot_sectors": [
      {"name": "新能源汽车", "avg_change": 8.5, "score": 95},
      {"name": "人工智能", "avg_change": 7.2, "score": 88},
      {"name": "半导体", "avg_change": 6.1, "score": 82}
    ],
    "hot_concepts": ["AI算力", "新能源", "机器人"]
  },
  "recommended_stocks": [
    {
      "ts_code": "000001.SZ",
      "name": "平安银行",
      "sector": "银行",
      "total_score": 88.5,
      "grade": "A",
      "scores": {
        "fundamental": 75.0,
        "technical": 82.0,
        "hotspot": 65.0,
        "money_flow": 78.0
      },
      "recommendation_reason": "基本面优秀+技术面强势+资金持续流入",
      "risk_warning": "",
      "is_sector_leader": true
    }
    // ... 更多推荐股票
  ],
  "sector_distribution": {
    "新能源汽车": 5,
    "人工智能": 4,
    "半导体": 3,
    "医药": 2,
    "其他": 6
  }
}
```

---

## 9. 定时任务设计

### 9.1 执行时间表

```cpp
// scheduler/StockSelectorScheduler.h

namespace scheduler {

/**
 * @brief 筛选任务调度器
 */
class StockSelectorScheduler {
public:
    explicit StockSelectorScheduler(core::StockSelector& selector);
    
    /**
     * @brief 启动定时任务
     */
    void start();
    
    /**
     * @brief 停止定时任务
     */
    void stop();
    
    /**
     * @brief 设置执行时间
     * @param daily_time 每日执行时间（如 "20:00"）
     * @param weekly_time 每周执行时间（如 "周五 18:00"）
     */
    void setSchedule(
        const std::string& daily_time,
        const std::string& weekly_time = ""
    );

private:
    core::StockSelector& selector_;
    bool running_ = false;
    
    // 执行任务
    void executeDailySelection();
    void executeWeeklyAnalysis();
    
    // 输出结果
    void outputResult(const core::SelectionResult& result);
};

} // namespace scheduler
```

### 9.2 执行计划

| 任务 | 执行时间 | 说明 |
|------|----------|------|
| 每日筛选 | 交易日 20:00 | 获取当日收盘数据，执行完整筛选 |
| 热点追踪 | 交易日 09:30 | 监控开盘热点（可选） |
| 周报生成 | 周五 18:00 | 生成本周筛选总结报告 |
| 月度分析 | 月末 | 分析筛选准确率，优化参数 |

---

## 10. 扩展功能

### 10.1 概念热点追踪（需外部数据）

```cpp
// 当前 Tushare API 不直接提供概念数据
// 可通过以下方式扩展：

1. 行业分类映射：
   - 将 industry 字段映射到概念板块
   - 创建概念-行业映射表

2. 关键词分析：
   - 分析股票名称、简介中的关键词
   - 关联到当前热门概念

3. 外部数据源：
   - 集成财经新闻API
   - 集成社交媒体热点数据
```

### 10.2 自适应参数优化

```cpp
// 核心思想：根据历史筛选结果优化参数

class ParameterOptimizer {
public:
    /**
     * @brief 分析历史筛选准确率
     * @param history_results 历史筛选结果
     * @return 参数调整建议
     */
    ScorerConfig optimize(
        const std::vector<SelectionResult>& history_results
    );
    
private:
    /**
     * @brief 计算各因子权重最优值
     */
    void optimizeWeights();
    
    /**
     * @brief 计算阈值最优值
     */
    void optimizeThresholds();
};
```

---

## 11. 文件结构

```
cpp/
├── core/
│   ├── Stock.h                    # 已有
│   ├── FundamentalData.h          # 已有
│   ├── HotspotData.h              # 新增 - 热点数据结构
│   ├── SelectionResult.h          # 新增 - 筛选结果数据结构
│   └── StockSelector.h/cpp        # 新增 - 主控制器
│
├── analysis/
│   ├── indicators/                # 已有 - 技术指标
│   ├── HotspotTracker.h/cpp       # 新增 - 热点追踪器
│   ├── CompositeScorer.h/cpp      # 新增 - 综合评分器
│   ├── RiskFilter.h/cpp           # 新增 - 风险过滤器
│   └── TechnicalScorer.h/cpp      # 新增 - 技术面评分
│   └── MoneyFlowScorer.h/cpp      # 新增 - 资金流向评分
│
├── output/
│   ├── SelectionReport.h/cpp      # 新增 - 筛选报告生成
│   └── FeishuNotifier.h/cpp       # 新增 - 飞书推送
│
├── scheduler/
│   └── StockSelectorScheduler.h/cpp  # 新增 - 筛选任务调度
│
├── docs/
│   └── STOCK_SELECTOR_DESIGN.md   # 本文档
│
└── CMakeLists.txt                 # 更新编译配置
```

---

## 12. 开发计划

### Phase 1: 数据结构与基础类（1周）
- [ ] 创建 HotspotData.h 数据结构
- [ ] 创建 SelectionResult.h 数据结构
- [ ] 实现 HotspotTracker 基础框架

### Phase 2: 评分系统（1周）
- [ ] 实现 CompositeScorer 综合评分器
- [ ] 实现 TechnicalScorer 技术面评分
- [ ] 实现 MoneyFlowScorer 资金流向评分

### Phase 3: 过滤与输出（1周）
- [ ] 实现 RiskFilter 风险过滤器
- [ ] 实现 StockSelector 主控制器
- [ ] 实现 SelectionReport 报告生成

### Phase 4: 调度与优化（1周）
- [ ] 实现 StockSelectorScheduler 定时任务
- [ ] 集成飞书推送
- [ ] 测试与参数调优

---

## 13. 总结

### 13.1 系统优势

1. **多维度综合评分**：基本面+技术面+热点+资金流向，全方位评估
2. **热点追踪**：实时识别市场热点，把握投资机会
3. **风险控制**：多层过滤机制，规避风险股票
4. **灵活配置**：权重和阈值可动态调整
5. **可扩展**：模块化设计，易于添加新因子

### 13.2 与现有系统整合

- **利用现有基础设施**：Tushare API、技术指标、策略系统
- **扩展而非重构**：新增模块，不影响现有功能
- **统一数据结构**：使用 core 模块定义的数据结构

### 13.3 输出价值

- **推荐股票列表**：每日20只高质量推荐股票
- **热点分析**：热门行业、概念、龙头股
- **评分详情**：各维度得分及推荐理由
- **风险提示**：潜在风险预警

---

**文档版本**: v1.0  
**创建日期**: 2026-05-05  
**最后更新**: 2026-05-05  
**维护者**: Development Team