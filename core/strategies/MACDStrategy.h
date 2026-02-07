#ifndef CORE_STRATEGIES_MACD_STRATEGY_H
#define CORE_STRATEGIES_MACD_STRATEGY_H

#include "../Strategy.h"

namespace core {

/**
 * @brief MACD 策略
 * 
 * 使用 MACD 指标产生买卖信号
 * - MACD 线上穿信号线：买入信号
 * - MACD 线下穿信号线：卖出信号
 * - 柱状图由负转正：买入确认
 * - 柱状图由正转负：卖出确认
 * 
 * 参数：
 * - fast_period: 快线周期（默认 12）
 * - slow_period: 慢线周期（默认 26）
 * - signal_period: 信号线周期（默认 9）
 */
class MACDStrategy : public StrategyBase {
public:
    explicit MACDStrategy(const std::map<std::string, double>& params = {});
    
    AnalysisResult analyze(
        const std::string& tsCode,
        const std::vector<StockData>& data
    ) override;
    
    bool validateParameters() const override;

private:
    struct MACDResult {
        std::vector<double> macd;      // MACD 线
        std::vector<double> signal;    // 信号线
        std::vector<double> histogram; // 柱状图
    };
    
    /**
     * @brief 计算 MACD
     */
    MACDResult calculateMACD(const std::vector<double>& prices, int fastPeriod, int slowPeriod, int signalPeriod) const;
    
    /**
     * @brief 计算 EMA
     */
    std::vector<double> calculateEMA(const std::vector<double>& prices, int period) const;
};

} // namespace core

#endif // CORE_STRATEGIES_MACD_STRATEGY_H

