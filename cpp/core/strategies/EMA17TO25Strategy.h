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
 * - trend_days: 趋势判断窗口天数（默认 3）
 * - min_confidence: 最小置信度阈值（默认 0）
 */
class EMA17TO25Strategy : public StrategyBase {
public:
    explicit EMA17TO25Strategy(const std::map<std::string, double>& params = {});
    
    std::optional<AnalysisResult> analyze(
        const std::string& tsCode,
        const std::vector<StockData>& data
    ) override;
    
    bool validateParameters() const override;

private:
    bool isUpTrend(const std::vector<double>& ema) const;
    
    bool isGoldenCross(const std::vector<double>& fastEMA, const std::vector<double>& slowEMA) const;
    
    bool isDeathCross(const std::vector<double>& fastEMA, const std::vector<double>& slowEMA) const;
    
    double calculateConfidence(
        const std::vector<double>& fastEMA,
        const std::vector<double>& slowEMA
    ) const;
    
    double predictGoldenCrossPrice(
        double fastEMA,
        double slowEMA,
        int fastPeriod,
        int slowPeriod
    ) const;
    
    double calculatePredictionConfidence(
        double currentPrice,
        double predictedPrice,
        const std::vector<double>& fastEMA,
        const std::vector<double>& slowEMA
    ) const;
};

} // namespace core

#endif //STOCK_FOR_CPP_EMA17TO25STRATEGY_H