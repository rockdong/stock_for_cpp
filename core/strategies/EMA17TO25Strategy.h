//
// Created by 蔡冬 on 2026/2/6.
//

#ifndef STOCK_FOR_CPP_EMA17TO25STRATEGY_H
#define STOCK_FOR_CPP_EMA17TO25STRATEGY_H

#include "../Strategy.h"

namespace core {

/**
 * @brief EMA17/EMA25 交叉策略
 * 
 * 使用 EMA17 和 EMA25 的交叉产生买卖信号
 * - EMA17 上穿 EMA25：买入信号（金叉）
 * - EMA17 下穿 EMA25：卖出信号（死叉）
 * 
 * 参数：
 * - fast_period: 快线周期（默认 17）
 * - slow_period: 慢线周期（默认 25）
 */
class EMA17TO25Strategy : public StrategyBase {
public:
    explicit EMA17TO25Strategy(const std::map<std::string, double>& params = {});
    
    AnalysisResult analyze(
        const std::string& tsCode,
        const std::vector<StockData>& data
    ) override;
    
    bool validateParameters() const override;

private:
    /**
     * @brief 检测金叉（快线上穿慢线）
     * @param fastEMA 快线 EMA
     * @param slowEMA 慢线 EMA
     * @return 是否发生金叉
     */
    bool isGoldenCross(const std::vector<double>& fastEMA, const std::vector<double>& slowEMA) const;
    
    /**
     * @brief 检测死叉（快线下穿慢线）
     * @param fastEMA 快线 EMA
     * @param slowEMA 慢线 EMA
     * @return 是否发生死叉
     */
    bool isDeathCross(const std::vector<double>& fastEMA, const std::vector<double>& slowEMA) const;
};

} // namespace core

#endif //STOCK_FOR_CPP_EMA17TO25STRATEGY_H