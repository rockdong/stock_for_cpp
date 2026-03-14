//
// Created by 蔡冬 on 2026/3/14.
//

#include "EMA25CrossoverStrategy.h"
#include "../../analysis/indicators/EMA.h"

namespace core {

EMA25CrossoverStrategy::EMA25CrossoverStrategy(
    const std::map<std::string, double>& params
)
    : StrategyBase("EMA25_CROSSOVER", "EMA17 上穿 EMA25 金叉策略")
{
    setParameter("fast_period", 17.0);
    setParameter("slow_period", 25.0);
    setParameter("confirm_days", 1.0);
    setParameters(params);
}

std::optional<AnalysisResult> EMA25CrossoverStrategy::analyze(
    const std::string& tsCode,
    const std::vector<StockData>& data
) {
    int fastPeriod = static_cast<int>(getParameter("fast_period", 17.0));
    int slowPeriod = static_cast<int>(getParameter("slow_period", 25.0));
    int confirmDays = static_cast<int>(getParameter("confirm_days", 1.0));
    
    if (!hasEnoughData(data, static_cast<size_t>(slowPeriod + confirmDays + 2))) {
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
    
    if (!isGoldenCross(fastEMA, slowEMA)) {
        return std::nullopt;
    }
    
    std::string tradeDate = data.back().trade_date;
    return createResult(tsCode, tradeDate, "买入");
}

bool EMA25CrossoverStrategy::validateParameters() const {
    int fastPeriod = static_cast<int>(getParameter("fast_period", 17.0));
    int slowPeriod = static_cast<int>(getParameter("slow_period", 25.0));
    int confirmDays = static_cast<int>(getParameter("confirm_days", 1.0));
    
    if (fastPeriod <= 0 || slowPeriod <= 0) {
        return false;
    }
    if (fastPeriod >= slowPeriod) {
        return false;
    }
    if (confirmDays < 0) {
        return false;
    }
    return true;
}

bool EMA25CrossoverStrategy::isGoldenCross(
    const std::vector<double>& fastEMA,
    const std::vector<double>& slowEMA
) const {
    if (fastEMA.size() < 2 || slowEMA.size() < 2) {
        return false;
    }
    
    size_t lastIndex = fastEMA.size() - 1;
    size_t prevIndex = lastIndex - 1;
    
    double prevFast = fastEMA[prevIndex];
    double prevSlow = slowEMA[prevIndex];
    double currFast = fastEMA[lastIndex];
    double currSlow = slowEMA[lastIndex];
    
    bool wasBelow = prevFast < prevSlow;
    bool isAboveOrEqual = currFast >= currSlow;
    
    return wasBelow && isAboveOrEqual;
}

} // namespace core