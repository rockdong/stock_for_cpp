#ifndef CORE_STRATEGIES_BOLL_STRATEGY_H
#define CORE_STRATEGIES_BOLL_STRATEGY_H

#include "../Strategy.h"

namespace core {

/**
 * @brief 布林带策略
 * 
 * 使用布林带产生买卖信号
 * - 价格触及下轨：买入信号
 * - 价格触及上轨：卖出信号
 * 
 * 参数：
 * - period: 周期（默认 20）
 * - std_dev: 标准差倍数（默认 2.0）
 */
class BOLLStrategy : public StrategyBase {
public:
    explicit BOLLStrategy(const std::map<std::string, double>& params = {});
    
    TradeSignal analyze(
        const std::string& tsCode,
        const std::vector<StockData>& data,
        const Portfolio& portfolio
    ) override;
    
    bool validateParameters() const override;

private:
    struct BOLLResult {
        std::vector<double> upper;  // 上轨
        std::vector<double> middle; // 中轨
        std::vector<double> lower;  // 下轨
    };
    
    /**
     * @brief 计算布林带
     */
    BOLLResult calculateBOLL(const std::vector<double>& prices, int period, double stdDev) const;
};

} // namespace core

#endif // CORE_STRATEGIES_BOLL_STRATEGY_H

