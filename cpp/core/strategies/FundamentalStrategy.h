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