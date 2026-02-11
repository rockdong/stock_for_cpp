//
// Created by 蔡冬 on 2026/2/12.
//

#include "EMA25Greater17PriceMatchStrategy.h"
#include <cmath>
#include "../../analysis/indicators/EMA.h"

namespace core {

EMA25Greater17PriceMatchStrategy::EMA25Greater17PriceMatchStrategy(
    const std::map<std::string, double>& params
)
    : StrategyBase("EMA25_GREATER_17_PRICE_MATCH", "EMA25 大于 EMA17 且收盘价与 EMA17 小数点后两位相等策略")
{
    setParameter("fast_period", 17.0);
    setParameter("slow_period", 25.0);
    setParameters(params);
}

std::optional<AnalysisResult> EMA25Greater17PriceMatchStrategy::analyze(
    const std::string& tsCode,
    const std::vector<StockData>& data
) {
    int fastPeriod = static_cast<int>(getParameter("fast_period", 17.0));
    int slowPeriod = static_cast<int>(getParameter("slow_period", 25.0));
    if (!hasEnoughData(data, static_cast<size_t>(slowPeriod))) {
        return std::nullopt;
    }
    std::vector<double> closes;
    closes.reserve(data.size());
    for (const auto& item : data) {
        closes.push_back(item.close);
    }
    std::vector<double> fastEMA = analysis::EMA::compute(closes, fastPeriod);
    std::vector<double> slowEMA = analysis::EMA::compute(closes, slowPeriod);
    if (fastEMA.empty() || slowEMA.empty()) {
        return std::nullopt;
    }
    size_t lastIndex = closes.size() - 1;
    double lastClose = closes[lastIndex];
    double lastFast = fastEMA.back();
    double lastSlow = slowEMA.back();
    if (lastSlow <= lastFast) {
        return std::nullopt;
    }
    double roundedClose = roundToTwoDecimals(lastClose);
    double roundedFast = roundToTwoDecimals(lastFast);
    if (roundedClose != roundedFast) {
        return std::nullopt;
    }
    std::string tradeDate = data.back().trade_date;
    return createResult(tsCode, tradeDate, "买入");
}

bool EMA25Greater17PriceMatchStrategy::validateParameters() const {
    int fastPeriod = static_cast<int>(getParameter("fast_period", 17.0));
    int slowPeriod = static_cast<int>(getParameter("slow_period", 25.0));
    if (fastPeriod <= 0 || slowPeriod <= 0) {
        return false;
    }
    if (fastPeriod >= slowPeriod) {
        return false;
    }
    return true;
}

double EMA25Greater17PriceMatchStrategy::roundToTwoDecimals(double value) const {
    double scaled = value * 100.0;
    double rounded = std::round(scaled);
    return rounded / 100.0;
}

} // namespace core


