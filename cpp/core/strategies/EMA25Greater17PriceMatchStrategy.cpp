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
    setParameter("below_days", 3.0);
    setParameters(params);
}

std::optional<AnalysisResult> EMA25Greater17PriceMatchStrategy::analyze(
    const std::string& tsCode,
    const std::vector<StockData>& data
) {
    int fastPeriod = static_cast<int>(getParameter("fast_period", 17.0));
    int slowPeriod = static_cast<int>(getParameter("slow_period", 25.0));
    int belowDays = static_cast<int>(getParameter("below_days", 3.0));
    if (!hasEnoughData(data, static_cast<size_t>(slowPeriod + belowDays))) {
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
    // 条件 1：最近 belowDays 天内，EMA25 始终大于 EMA17（包含今天）
    if (!isSlowAboveFastForDays(fastEMA, slowEMA, static_cast<size_t>(belowDays))) {
        return std::nullopt;
    }
    // 条件 2：最近 belowDays 天内，收盘价都小于 EMA17（不包含今天）
    if (!isCloseBelowFastForDays(closes, fastEMA, static_cast<size_t>(belowDays))) {
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
    int belowDays = static_cast<int>(getParameter("below_days", 3.0));
    if (fastPeriod <= 0 || slowPeriod <= 0) {
        return false;
    }
    if (fastPeriod >= slowPeriod) {
        return false;
    }
    if (belowDays <= 0) {
        return false;
    }
    return true;
}

double EMA25Greater17PriceMatchStrategy::roundToTwoDecimals(double value) const {
    double scaled = value * 100.0;
    double rounded = std::round(scaled);
    return rounded / 100.0;
}

bool EMA25Greater17PriceMatchStrategy::isCloseBelowFastForDays(
    const std::vector<double>& closes,
    const std::vector<double>& fastEMA,
    size_t days
) const {
    if (closes.size() < days + 1 || fastEMA.size() < days + 1) {
        return false;
    }
    size_t lastIndex = closes.size() - 1;
    for (size_t i = 1; i <= days; ++i) { // 从昨天开始往前数 days 天
        size_t idx = lastIndex - i;
        if (closes[idx] >= fastEMA[idx]) {
            return false;
        }
    }
    return true;
}

bool EMA25Greater17PriceMatchStrategy::isSlowAboveFastForDays(
    const std::vector<double>& fastEMA,
    const std::vector<double>& slowEMA,
    size_t days
) const {
    if (fastEMA.size() < days || slowEMA.size() < days) {
        return false;
    }
    size_t lastIndex = fastEMA.size() - 1;
    for (size_t i = 0; i < days; ++i) { // 从今天开始往前数 days 天
        size_t idx = lastIndex - i;
        if (slowEMA[idx] <= fastEMA[idx]) {
            return false;
        }
    }
    return true;
}

} // namespace core


