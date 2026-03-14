//
// Created by 蔡冬 on 2026/3/14.
//

#ifndef STOCK_FOR_CPP_EMA25CROSSOVERSTRATEGY_H
#define STOCK_FOR_CPP_EMA25CROSSOVERSTRATEGY_H

#include "../Strategy.h"

namespace core {

/**
 * @brief EMA17 上穿 EMA25 金叉策略
 *
 * 当 EMA17 从下方穿越 EMA25 时给出买入信号：
 * - 昨天: EMA17 < EMA25
 * - 今天: EMA17 >= EMA25
 *
 * 这是一种提前识别策略，在上涨趋势开始时及时入场
 *
 * 参数：
 * - fast_period: EMA17 周期（默认 17）
 * - slow_period: EMA25 周期（默认 25）
 * - confirm_days: 确认天数，要求金叉后保持一段时间（默认 1）
 */
class EMA25CrossoverStrategy : public StrategyBase {
public:
    explicit EMA25CrossoverStrategy(const std::map<std::string, double>& params = {});

    std::optional<AnalysisResult> analyze(
        const std::string& tsCode,
        const std::vector<StockData>& data
    ) override;

    bool validateParameters() const override;

private:
    bool isGoldenCross(const std::vector<double>& fastEMA, const std::vector<double>& slowEMA) const;
};

} // namespace core

#endif //STOCK_FOR_CPP_EMA25CROSSOVERSTRATEGY_H