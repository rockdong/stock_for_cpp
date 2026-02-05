#ifndef CORE_STRATEGIES_MA_CROSS_STRATEGY_H
#define CORE_STRATEGIES_MA_CROSS_STRATEGY_H

#include "../Strategy.h"

namespace core {

/**
 * @brief 均线交叉策略
 * 
 * 使用双均线交叉产生买卖信号
 * - 短期均线上穿长期均线：买入信号（金叉）
 * - 短期均线下穿长期均线：卖出信号（死叉）
 * 
 * 参数：
 * - short_period: 短期均线周期（默认 5）
 * - long_period: 长期均线周期（默认 20）
 */
class MACrossStrategy : public StrategyBase {
public:
    explicit MACrossStrategy(const std::map<std::string, double>& params = {});
    
    TradeSignal analyze(
        const std::string& tsCode,
        const std::vector<StockData>& data
    ) override;
    
    bool validateParameters() const override;

private:
    /**
     * @brief 计算移动平均线
     */
    std::vector<double> calculateMA(const std::vector<double>& prices, int period) const;
    
    /**
     * @brief 检测金叉（短期均线上穿长期均线）
     */
    bool isGoldenCross(const std::vector<double>& shortMA, const std::vector<double>& longMA) const;
    
    /**
     * @brief 检测死叉（短期均线下穿长期均线）
     */
    bool isDeathCross(const std::vector<double>& shortMA, const std::vector<double>& longMA) const;
};

} // namespace core

#endif // CORE_STRATEGIES_MA_CROSS_STRATEGY_H

