#ifndef CORE_STRATEGIES_RSI_STRATEGY_H
#define CORE_STRATEGIES_RSI_STRATEGY_H

#include "../Strategy.h"

namespace core {

/**
 * @brief RSI 策略
 * 
 * 使用 RSI 指标产生买卖信号
 * - RSI < 30：超卖，买入信号
 * - RSI > 70：超买，卖出信号
 * 
 * 参数：
 * - period: RSI 周期（默认 14）
 * - oversold: 超卖阈值（默认 30）
 * - overbought: 超买阈值（默认 70）
 */
class RSIStrategy : public StrategyBase {
public:
    explicit RSIStrategy(const std::map<std::string, double>& params = {});
    
    TradeSignal analyze(
        const std::string& tsCode,
        const std::vector<StockData>& data
    ) override;
    
    bool validateParameters() const override;

private:
    /**
     * @brief 计算 RSI
     */
    std::vector<double> calculateRSI(const std::vector<double>& prices, int period) const;
};

} // namespace core

#endif // CORE_STRATEGIES_RSI_STRATEGY_H

