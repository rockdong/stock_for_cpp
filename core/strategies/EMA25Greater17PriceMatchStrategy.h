//
// Created by 蔡冬 on 2026/2/12.
//

#ifndef STOCK_FOR_CPP_EMA25GREATER17PRICEMATCHSTRATEGY_H
#define STOCK_FOR_CPP_EMA25GREATER17PRICEMATCHSTRATEGY_H

#include "../Strategy.h"

namespace core {

/**
 * @brief EMA25 大于 EMA17 且收盘价小数点后两位等于 EMA17 的策略
 *
 * 使用 EMA17 和 EMA25 判断趋势，并在满足以下条件时给出买入信号：
 * - 当日 EMA25 > EMA17
 * - 当日收盘价保留两位小数后与 EMA17 保留两位小数后的数值完全相等
 *
 * 参数：
 * - fast_period: EMA17 周期（默认 17）
 * - slow_period: EMA25 周期（默认 25）
 */
class EMA25Greater17PriceMatchStrategy : public StrategyBase {
public:
    explicit EMA25Greater17PriceMatchStrategy(const std::map<std::string, double>& params = {});

    std::optional<AnalysisResult> analyze(
        const std::string& tsCode,
        const std::vector<StockData>& data
    ) override;

    bool validateParameters() const override;

private:
    /**
     * @brief 把数值四舍五入到小数点后两位
     * @param value 原始数值
     * @return 保留两位小数后的数值
     */
    double roundToTwoDecimals(double value) const;
};

} // namespace core

#endif // STOCK_FOR_CPP_EMA25GREATER17PRICEMATCHSTRATEGY_H


