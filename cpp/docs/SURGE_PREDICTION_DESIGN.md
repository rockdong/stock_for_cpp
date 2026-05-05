# 暴涨预兆捕捉系统设计方案

## 核心目标

**提前1-2天识别即将大涨的股票，买入等待大涨。**

---

## 1. 关键发现

### 1.1 现有系统已有的暴涨预兆策略

**SurgeSignalStrategy**（已实现）检测5大信号：

| 信号 | 权重 | 描述 | 提前预测价值 |
|------|------|------|-------------|
| ⭐⭐⭐⭐⭐ **倍量试盘** | 2分 | 成交量达5日均量2倍以上，涨幅3%-7% | **最高** - 主力试探拉升意愿 |
| ⭐⭐⭐⭐ **缩量回调** | 1分 | 价跌量缩至前日50%以下 | **高** - 主力洗盘，等待突破 |
| ⭐⭐⭐⭐ **均线粘合** | 1分 | 5/10/20日均线偏离度≤2% | **高** - 即将变盘突破 |
| ⭐⭐⭐⭐⭐ **MACD底背离** | 2分 | 股价新低+MACD不新低 | **最高** - 反转信号 |
| ⭐⭐⭐⭐ **OBV底背离** | 1分 | 股价新低+OBV不新低 | **高** - 主力持续吸筹 |

**EMA17TO25Strategy**（已实现）预测金叉：
- 检测 EMA17 与 EMA25 接近程度
- **预测明日金叉目标价**
- 输出"金叉预警"信号

### 1.2 Tushare API 涨停/热点数据

| API | 功能 | 提前预警价值 |
|------|------|-------------|
| `limit_list_d` | 涨停板股票列表 | 识别当日强势股 |
| `limit_cpt_list` | 涨停概念板块排行 | **核心** - 热点板块识别 |
| `dc_concept` | 概念题材热度排行 | 热度+主力净流入+领涨股 |
| `top_list` | 龙虎榜交易明细 | 主力动向追踪 |
| `moneyflow` | 资金流向 | 主力吸筹特征 |

---

## 2. 暴涨预兆组合信号设计

### 2.1 一级信号（强买入预警）

**组合条件（满足以下≥3个即触发）**：

```
┌─────────────────────────────────────────────────────────────┐
│ 一级预警信号组合                                             │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│ ① 倍量试盘 + 缩量回调（连续出现）                            │
│    → 前日倍量试盘，今日缩量回调                              │
│    → 主力吸筹完成，即将拉升 ⭐⭐⭐⭐⭐                          │
│                                                              │
│ ② 均线粘合 + MACD底背离                                     │
│    → 变盘前夕 + 反转确认                                    │
│    → 大涨概率极高 ⭐⭐⭐⭐⭐                                    │
│                                                              │
│ ③ OBV底背离 + 主力资金连续3日净流入                         │
│    → 隐藏吸筹 + 资金确认                                    │
│    → 主力建仓完成 ⭐⭐⭐⭐                                      │
│                                                              │
│ ④ EMA金叉预警 + 量比>1.5                                    │
│    → 技术突破 + 量能配合                                    │
│    → 1-2日内突破 ⭐⭐⭐⭐                                      │
│                                                              │
│ ⑤ 所属概念板块涨停数≥5 + 领涨股特征                         │
│    → 板块热度高 + 跟涨机会                                  │
│    → 板块轮动机会 ⭐⭐⭐                                       │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

### 2.2 二级信号（关注备选）

**组合条件（满足以下≥2个）**：

```
① 量比>2 + 换手率3%-8% + 涨幅<3%
   → 试盘特征，未大涨，有空间

② MACD即将金叉（柱状图由负转正）
   → 0轴附近金叉信号

③ 主力净流入>500万 + 散户净流出
   → 典型吸筹模式

④ 板块涨幅>2% + 股票涨幅<2%
   → 板块强势，个股跟涨潜力

⑤ 连续3日缩量 + 价格稳定
   → 洗盘尾声，蓄势待发
```

---

## 3. 新增数据结构与接口

### 3.1 暴涨预兆检测结果

```cpp
// core/SurgePrediction.h

namespace core {

/**
 * @brief 暴涨预兆检测结果（增强版）
 */
struct SurgePrediction {
    std::string ts_code;                // 股票代码
    std::string name;                   // 股票名称
    std::string trade_date;             // 分析日期
    
    // 信号检测结果（继承 SurgeSignalStrategy）
    bool volume_test = false;           // 倍量试盘
    bool shrink_back = false;           // 缩量回调
    bool ma_convergence = false;        // 均线粘合
    bool macd_divergence = false;       // MACD底背离
    bool obv_divergence = false;        // OBV底背离
    bool ema_cross_warning = false;     // EMA金叉预警
    
    // 新增信号
    bool main_inflow_3d = false;        // 主力3日连续流入
    bool sector_hot = false;            // 所属板块热门
    bool dragon_tiger = false;          // 龙虎榜上榜
    
    // 量价指标
    double volume_ratio = 0.0;          // 量比
    double turnover_rate = 0.0;         // 换手率
    double pct_chg_1d = 0.0;            // 1日涨幅
    double net_inflow = 0.0;            // 净流入额
    
    // 综合评分
    int signal_count = 0;               // 信号数量
    double confidence = 0.0;            // 置信度
    std::string prediction_level;       // 预警等级: LEVEL1/LEVEL2/LEVEL3
    int expected_surge_days = 0;        // 预计大涨天数（1-2）
    
    // 预测详情
    std::string signal_combo;           // 信号组合描述
    std::string reason;                 // 预警理由
    double predicted_price = 0.0;       // 预测突破价
    double suggested_buy_price = 0.0;   // 建议买入价
    
    // 风险提示
    std::string risk_warning;           // 风险提示
};

/**
 * @brief 板块热点数据
 */
struct SectorHotData {
    std::string sector_code;            // 板块代码
    std::string sector_name;            // 板块名称
    std::string trade_date;             // 日期
    
    int limit_up_count = 0;             // 涨停股数量
    double pct_change = 0.0;            // 板块涨幅
    double main_inflow = 0.0;           // 主力净流入
    int hot_rank = 0;                   // 热度排名
    
    std::string lead_stock;             // 领涨股名称
    std::string lead_stock_code;        // 领涨股代码
    double lead_stock_pct = 0.0;        // 领涨股涨幅
};

/**
 * @brief 暴涨预测结果汇总
 */
struct SurgePredictionResult {
    std::string analysis_date;          // 分析日期
    
    // 一级预警（强买入）
    std::vector<SurgePrediction> level1_predictions;  // 预警股票
    
    // 二级预警（关注备选）
    std::vector<SurgePrediction> level2_predictions;
    
    // 三级预警（观察）
    std::vector<SurgePrediction> level3_predictions;
    
    // 板块热点
    std::vector<SectorHotData> hot_sectors;
    
    // 统计
    int total_analyzed = 0;
    int level1_count = 0;
    int level2_count = 0;
    
    std::string summary;                // 汇总说明
};

} // namespace core
```

### 3.2 暴涨预测引擎

```cpp
// core/SurgePredictionEngine.h

namespace core {

/**
 * @brief 暴涨预测引擎
 * 
 * 专门用于提前1-2天识别即将大涨的股票
 */
class SurgePredictionEngine {
public:
    explicit SurgePredictionEngine(
        network::DataSourcePtr dataSource,
        const std::map<std::string, double>& params = {}
    );
    
    /**
     * @brief 执行暴涨预测分析
     * @param trade_date 交易日期
     * @return 预测结果
     */
    SurgePredictionResult predict(const std::string& trade_date);
    
    /**
     * @brief 分析单只股票的暴涨预兆
     * @param ts_code 股票代码
     * @param trade_date 交易日期
     * @return 预测详情
     */
    SurgePrediction analyzeStock(
        const std::string& ts_code,
        const std::string& trade_date
    );
    
    /**
     * @brief 快速筛选（仅一级预警）
     * @param candidates 候选股票池
     * @param trade_date 交易日期
     * @return 一级预警股票列表
     */
    std::vector<SurgePrediction> quickFilter(
        const std::vector<std::string>& candidates,
        const std::string& trade_date
    );
    
    /**
     * @brief 设置预测参数
     */
    void setParams(
        double volume_ratio_min = 2.0,      // 量比最低值
        double shrink_ratio_max = 0.5,      // 缩量比例上限
        double ma_deviation_max = 0.02,     // 均线粘合偏离度
        double net_inflow_min = 500.0,      // 净流入最低（万元）
        int signal_count_min = 3,           // 一级预警信号数最低
        int sector_limit_min = 5            // 板块涨停数最低
    );

private:
    network::DataSourcePtr dataSource_;
    
    // 参数
    struct Params {
        double volume_ratio_min = 2.0;
        double shrink_ratio_max = 0.5;
        double ma_deviation_max = 0.02;
        double net_inflow_min = 500.0;
        int signal_count_min = 3;
        int sector_limit_min = 5;
    } params_;
    
    // 子策略
    std::unique_ptr<SurgeSignalStrategy> surgeStrategy_;
    std::unique_ptr<EMA17TO25Strategy> emaStrategy_;
    
    /**
     * @brief 检测一级信号组合
     */
    bool checkLevel1Combo(const SurgePrediction& pred);
    
    /**
     * @brief 检测二级信号组合
     */
    bool checkLevel2Combo(const SurgePrediction& pred);
    
    /**
     * @brief 获取板块热点数据
     */
    std::vector<SectorHotData> fetchSectorHotData(const std::string& trade_date);
    
    /**
     * @brief 计算预测置信度
     */
    double calculateConfidence(const SurgePrediction& pred);
    
    /**
     * @brief 判断股票所属板块是否热门
     */
    bool isInHotSector(
        const std::string& ts_code,
        const std::vector<SectorHotData>& hotSectors
    );
    
    /**
     * @brief 检测主力连续净流入
     */
    bool checkMainInflow3Days(
        const std::string& ts_code,
        const std::string& trade_date
    );
};

} // namespace core
```

---

## 4. 核心算法设计

### 4.1 一级预警检测算法

```cpp
bool SurgePredictionEngine::checkLevel1Combo(const SurgePrediction& pred) {
    // 组合1：倍量试盘 + 缩量回调（连续出现）
    if (pred.volume_test && pred.shrink_back) {
        pred.signal_combo = "倍量试盘后缩量回调";
        pred.expected_surge_days = 1;
        return true;
    }
    
    // 组合2：均线粘合 + MACD底背离
    if (pred.ma_convergence && pred.macd_divergence) {
        pred.signal_combo = "均线粘合+MACD底背离";
        pred.expected_surge_days = 2;
        return true;
    }
    
    // 组合3：OBV底背离 + 主力3日连续流入
    if (pred.obv_divergence && pred.main_inflow_3d) {
        pred.signal_combo = "OBV底背离+主力吸筹";
        pred.expected_surge_days = 2;
        return true;
    }
    
    // 组合4：EMA金叉预警 + 量比>1.5
    if (pred.ema_cross_warning && pred.volume_ratio > 1.5) {
        pred.signal_combo = "EMA金叉预警+量能配合";
        pred.expected_surge_days = 1;
        return true;
    }
    
    // 组合5：板块热门 + 领涨股特征
    if (pred.sector_hot && pred.pct_chg_1d >= 2.0 && pred.pct_chg_1d <= 5.0) {
        pred.signal_combo = "热门板块跟涨机会";
        pred.expected_surge_days = 1;
        return true;
    }
    
    // 单信号高分情况
    int signal_count = pred.volume_test + pred.shrink_back + 
                       pred.ma_convergence + pred.macd_divergence +
                       pred.obv_divergence + pred.main_inflow_3d;
    
    if (signal_count >= 3) {
        pred.signal_combo = "多信号共振";
        pred.expected_surge_days = 1;
        return true;
    }
    
    return false;
}
```

### 4.2 主力吸筹检测算法

```cpp
bool SurgePredictionEngine::checkMainInflow3Days(
    const std::string& ts_code,
    const std::string& trade_date
) {
    // 获取近3日资金流向
    auto flows = dataSource_->getMoneyFlow(ts_code, "", trade_date, "-3d");
    
    if (flows.size() < 3) return false;
    
    // 检查连续3日主力净流入
    int inflowDays = 0;
    double totalMainInflow = 0.0;
    
    for (const auto& flow : flows) {
        // 大单+特大单净流入
        double mainIn = flow.buy_lg_amount + flow.buy_elg_amount;
        double mainOut = flow.sell_lg_amount + flow.sell_elg_amount;
        double netMain = mainIn - mainOut;
        
        if (netMain > 0) {
            inflowDays++;
            totalMainInflow += netMain;
        }
    }
    
    // 连续3日流入 + 总流入达标
    return inflowDays >= 3 && totalMainInflow >= params_.net_inflow_min;
}
```

### 4.3 板块热度检测算法

```cpp
std::vector<SectorHotData> SurgePredictionEngine::fetchSectorHotData(
    const std::string& trade_date
) {
    std::vector<SectorHotData> result;
    
    // 调用 Tushare limit_cpt_list API
    auto tushareSource = std::dynamic_pointer_cast<network::TushareDataSource>(dataSource_);
    if (!tushareSource) return result;
    
    auto response = tushareSource->getClient()->limit_cpt_list(trade_date);
    
    if (response.isSuccess() && response.data.contains("items")) {
        for (const auto& item : response.data["items"]) {
            SectorHotData data;
            data.sector_code = item["ts_code"].get<std::string>();
            data.sector_name = item["name"].get<std::string>();
            data.trade_date = trade_date;
            data.limit_up_count = item["up_nums"].get<int>();
            data.pct_change = item["pct_chg"].get<double>();
            data.hot_rank = item["rank"].get<int>();
            
            if (item.contains("lead_stock")) {
                data.lead_stock = item["lead_stock"].get<std::string>();
            }
            if (item.contains("lead_stock_code")) {
                data.lead_stock_code = item["lead_stock_code"].get<std::string>();
            }
            
            // 热门板块：涨停数≥5 且 排名≤20
            if (data.limit_up_count >= params_.sector_limit_min && data.hot_rank <= 20) {
                result.push_back(data);
            }
        }
    }
    
    return result;
}
```

---

## 5. Tushare API 扩展封装

### 5.1 新增 API 封装

```cpp
// network/TushareClient.h 新增方法

/**
 * @brief 获取涨停板概念板块排行
 * @param trade_date 交易日期
 * @return API响应
 */
TushareResponse limit_cpt_list(const std::string& trade_date);

/**
 * @brief 获取每日涨停股票列表
 * @param trade_date 交易日期
 * @param limit_type U=涨停, D=跌停
 * @return API响应
 */
TushareResponse limit_list_d(
    const std::string& trade_date,
    const std::string& limit_type = "U"
);

/**
 * @brief 获取龙虎榜每日明细
 * @param trade_date 交易日期
 * @param ts_code 股票代码（可选）
 * @return API响应
 */
TushareResponse top_list(
    const std::string& trade_date,
    const std::string& ts_code = ""
);

/**
 * @brief 获取概念题材热度排行
 * @param trade_date 交易日期
 * @return API响应
 */
TushareResponse dc_concept(const std::string& trade_date);
```

### 5.2 新增数据结构

```cpp
// core/FundamentalData.h 新增

/**
 * @brief 涨停板股票数据
 */
struct LimitStock {
    std::string ts_code;            // 股票代码
    std::string name;               // 股票名称
    std::string trade_date;         // 交易日期
    std::string industry;           // 所属行业
    
    double close;                   // 收盘价
    double pct_chg;                 // 涨跌幅
    double amount;                  // 成交额
    
    int open_times;                 // 封板时间
    std::string up_stat;            // 连板状态（如 "3天3板"）
    int limit_times;                // 连板次数
    
    std::string first_time;         // 首次涨停时间
    std::string last_time;          // 最后涨停时间
    double fd_amount;               // 封单金额
};

/**
 * @brief 龙虎榜数据
 */
struct DragonTigerData {
    std::string ts_code;            // 股票代码
    std::string name;               // 名称
    std::string trade_date;         // 交易日期
    
    double close;                   // 收盘价
    double pct_change;              // 涨跌幅
    double turnover_rate;           // 换手率
    double amount;                  // 总成交额
    
    double l_buy;                   // 龙虎榜买入额
    double l_sell;                  // 龙虎榜卖出额
    double net_amount;              // 净买入额
    
    std::string reason;             // 上榜理由
};
```

---

## 6. 完整筛选流程

```
┌─────────────────────────────────────────────────────────────┐
│ 暴涨预兆捕捉流程                                             │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  Step 1: 板块热度扫描                                        │
│  └─→ limit_cpt_list API → 涨停数≥5 的热门板块               │
│  └─→ 输出: Top 20 热门板块列表                               │
│                                                              │
│  Step 2: 涨停池股票获取                                      │
│  └─→ limit_list_d API → 当日涨停股票                         │
│  └─→ 筛选: 非一字板、封板时间早、有连板潜力                   │
│  └─→ 输出: ~100 只强势股票                                   │
│                                                              │
│  Step 3: 基本面快速筛选                                      │
│  └─→ PE<50, PB<10, 流通市值20-500亿                          │
│  └─→ 输出: ~50 只优质候选                                    │
│                                                              │
│  Step 4: 暴涨预兆信号检测                                    │
│  └─→ 对每只候选股执行 SurgePredictionEngine.analyzeStock()   │
│  └─→ 检测: 倍量试盘、缩量回调、均线粘合、MACD底背离、OBV背离 │
│  └─→ 检测: 主力3日流入、EMA金叉预警、板块热度                │
│  └─→ 输出: SurgePrediction 结果                              │
│                                                              │
│  Step 5: 信号组合匹配                                        │
│  └─→ 一级预警: 信号组合≥3 或 高权重组合                      │
│  └─→ 二级预警: 信号组合≥2                                    │
│  └─→ 输出: 分级预警列表                                      │
│                                                              │
│  Step 6: 风险过滤                                            │
│  └─→ 排除: 停牌、ST、退市风险                                │
│  └─→ 排除: 涨幅已>8%（无空间）                               │
│  └─→ 输出: 最终推荐                                          │
│                                                              │
│  Step 7: 输出结果                                            │
│  └─→ 一级预警: 强买入，预计1-2日内大涨                       │
│  └─→ 二级预警: 关注备选                                      │
│  └─→ 板块热点: 推荐跟涨机会                                  │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

---

## 7. 输出格式设计

### 7.1 一级预警输出示例

```json
{
  "analysis_date": "20260505",
  "level1_predictions": [
    {
      "ts_code": "000001.SZ",
      "name": "平安银行",
      "signal_combo": "倍量试盘后缩量回调",
      "signal_count": 3,
      "confidence": 85.0,
      "expected_surge_days": 1,
      "signals": {
        "volume_test": true,
        "shrink_back": true,
        "ma_convergence": true
      },
      "metrics": {
        "volume_ratio": 2.5,
        "turnover_rate": 4.2,
        "pct_chg_1d": 2.8,
        "net_inflow": 1200.0
      },
      "prediction": {
        "suggested_buy_price": 15.20,
        "predicted_price": 16.50,
        "reason": "昨日倍量试盘，今日缩量回调，主力吸筹完成，预计明日突破"
      },
      "risk_warning": ""
    }
  ],
  "hot_sectors": [
    {
      "sector_name": "人工智能",
      "limit_up_count": 15,
      "hot_rank": 1,
      "lead_stock": "科大讯飞",
      "lead_stock_code": "002230.SZ"
    }
  ],
  "summary": "今日发现5只一级预警股票，主要集中在人工智能板块，建议关注"
}
```

---

## 8. 与现有系统整合

### 8.1 整合点

| 现有模块 | 整合方式 |
|---------|---------|
| **SurgeSignalStrategy** | 直接复用信号检测逻辑 |
| **EMA17TO25Strategy** | 复用金叉预测功能 |
| **IDataSource** | 扩展涨停板API封装 |
| **main.cpp** | 新增暴涨预测执行流程 |
| **飞书推送** | 新增一级预警推送格式 |

### 8.2 执行调度

```cpp
// 每日执行时间建议
// 15:30 - 收盘后立即分析
// 20:00 - 数据完整后二次分析

void executeSurgePrediction() {
    SurgePredictionEngine engine(dataSource);
    
    std::string tradeDate = utils::TimeUtil::today();
    
    // 执行预测
    auto result = engine.predict(tradeDate);
    
    // 一级预警推送
    if (result.level1_count > 0) {
        LOG_INFO("【一级预警】发现 " + std::to_string(result.level1_count) + " 只即将大涨股票");
        
        for (const auto& pred : result.level1_predictions) {
            LOG_INFO("  " + pred.ts_code + " " + pred.name + 
                     " - " + pred.signal_combo +
                     " 预计" + std::to_string(pred.expected_surge_days) + "日内大涨");
        }
        
        // 飞书推送
        feishu::sendSurgeAlert(result);
    }
}
```

---

## 9. 配置参数

```env
# ========== 暴涨预测配置 ==========

# 信号检测阈值
SURGE_VOLUME_RATIO_MIN=2.0       # 量比最低值
SURGE_SHRINK_RATIO_MAX=0.5       # 缩量比例上限
SURGE_MA_DEVIATION_MAX=0.02      # 均线粘合偏离度
SURGE_NET_INFLOW_MIN=500.0       # 净流入最低（万元）

# 预警级别阈值
SURGE_LEVEL1_SIGNAL_MIN=3        # 一级预警信号数最低
SURGE_LEVEL2_SIGNAL_MIN=2        # 二级预警信号数最低

# 板块热度阈值
SURGE_SECTOR_LIMIT_MIN=5         # 板块涨停数最低
SURGE_SECTOR_RANK_MAX=20         # 板块排名上限

# 输出控制
SURGE_MAX_LEVEL1=10              # 一级预警最大数量
SURGE_MAX_LEVEL2=20              # 二级预警最大数量

# 执行时间
SURGE_EXECUTE_TIME=15:30         # 首次执行时间
SURGE_SECOND_TIME=20:00          # 二次执行时间
```

---

## 10. 开发计划

### Phase 1: API封装（2天）
- [ ] 封装 `limit_list_d` 涨停板API
- [ ] 封装 `limit_cpt_list` 概念板块API
- [ ] 封装 `top_list` 龙虎榜API
- [ ] 封装 `dc_concept` 概念热度API

### Phase 2: 数据结构（1天）
- [ ] 定义 `SurgePrediction` 数据结构
- [ ] 定义 `SectorHotData` 数据结构
- [ ] 定义 `LimitStock` 数据结构

### Phase 3: 预测引擎（2天）
- [ ] 实现 `SurgePredictionEngine` 核心类
- [ ] 实现一级信号组合检测
- [ ] 实现二级信号组合检测
- [ ] 实现主力吸筹检测

### Phase 4: 整合测试（1天）
- [ ] 整合到 main.cpp 执行流程
- [ ] 测试涨停板数据获取
- [ ] 测试预测信号准确性

---

**文档版本**: v1.0  
**创建日期**: 2026-05-05  
**目标**: 提前1-2天识别即将大涨股票