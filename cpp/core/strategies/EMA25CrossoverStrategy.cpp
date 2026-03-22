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
    setParameter("trend_days", 3.0);
    setParameter("min_confidence", 30.0);
    setParameters(params);
}

std::optional<AnalysisResult> EMA25CrossoverStrategy::analyze(
    const std::string& tsCode,
    const std::vector<StockData>& data
) {
    int fastPeriod = static_cast<int>(getParameter("fast_period", 17.0));
    int slowPeriod = static_cast<int>(getParameter("slow_period", 25.0));
    int confirmDays = static_cast<int>(getParameter("confirm_days", 1.0));
    double minConfidence = getParameter("min_confidence", 30.0);
    
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
    
    // 趋势判断
    if (!isUpTrend(fastEMA) || !isUpTrend(slowEMA)) {
        return std::nullopt;
    }
    
    if (!isGoldenCross(fastEMA, slowEMA)) {
        return std::nullopt;
    }
    
    double confidence = calculateConfidence(fastEMA, slowEMA);
    
    if (confidence < minConfidence) {
        return std::nullopt;
    }
    
    SignalStrength strength = evaluateStrength(confidence);
    std::string tradeDate = data.back().trade_date;
    return createResult(tsCode, tradeDate, "买入", strength, confidence, "", "buy");
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
    
    // 更严格的金叉条件：前一天快线 < 慢线，当天快线 > 慢线（不是 >=）
    bool wasBelow = prevFast < prevSlow;
    bool isAbove = currFast > currSlow;
    
    return wasBelow && isAbove;
}

bool EMA25CrossoverStrategy::isUpTrend(const std::vector<double>& ema) const {
    int trendDays = static_cast<int>(getParameter("trend_days", 3.0));
    
    if (trendDays <= 0 || ema.size() < static_cast<size_t>(trendDays + 1)) {
        return false;
    }
    
    // 检查最近 trendDays 天是否呈上升趋势
    for (size_t i = ema.size() - trendDays + 1; i < ema.size(); i++) {
        if (ema[i] <= ema[i - 1]) {
            return false;
        }
    }
    return true;
}

double EMA25CrossoverStrategy::calculateConfidence(
    const std::vector<double>& fastEMA,
    const std::vector<double>& slowEMA
) const {
    if (fastEMA.empty() || slowEMA.empty()) {
        return 50.0;
    }
    
    int trendDays = static_cast<int>(getParameter("trend_days", 3.0));
    double trendScore = 0.0;
    
    // 修复：从正确的索引开始，避免越界
    if (fastEMA.size() >= static_cast<size_t>(trendDays + 1)) {
        int upDays = 0;
        for (size_t i = fastEMA.size() - trendDays + 1; i < fastEMA.size(); i++) {
            if (fastEMA[i] > fastEMA[i - 1]) upDays++;
        }
        trendScore = (static_cast<double>(upDays) / trendDays) * 40.0;
    }
    
    double crossScore = 30.0;
    
    double spreadScore = 30.0;
    if (slowEMA.back() > 0) {
        double spread = (fastEMA.back() - slowEMA.back()) / slowEMA.back();
        spreadScore = std::min(std::max(spread * 1000, 0.0), 30.0);
    }
    
    return trendScore + crossScore + spreadScore;
}

} // namespace core