//
// Created by 蔡冬 on 2026/2/6.
//

#include "EMA17TO25Strategy.h"
#include <iostream>
#include <algorithm>
#include "../../analysis/indicators/EMA.h"

namespace core {

EMA17TO25Strategy::EMA17TO25Strategy(const std::map<std::string, double>& params)
    : StrategyBase("EMA17TO25", "EMA17/EMA25交叉策略")
{
    setParameter("fast_period", 17);
    setParameter("slow_period", 25);
    setParameter("trend_days", 3);
    setParameter("min_confidence", 30);
    
    setParameters(params);
}

std::optional<AnalysisResult> EMA17TO25Strategy::analyze(
    const std::string& tsCode,
    const std::vector<StockData>& data
) {
    int fastPeriod = static_cast<int>(getParameter("fast_period", 17));
    int slowPeriod = static_cast<int>(getParameter("slow_period", 25));
    double minConfidence = getParameter("min_confidence", 0);
    
    if (!hasEnoughData(data, slowPeriod + 2)) {
        return std::nullopt;
    }
    
    std::vector<double> closes;
    closes.reserve(data.size());
    for (const auto& d : data) {
        closes.push_back(d.close);
    }

    #ifdef DEBUG
    std::cout << "--------------------------------" << std::endl;
    for (size_t i = 0; i < 5; i++) {
        std::cout << "收盘价: " << data[data.size() - 1 - i].close << ", 日期: " << data[data.size() - 1 - i].trade_date << std::endl;
    }
    std::cout << "--------------------------------" << std::endl;
    #endif

    auto fastEMA = analysis::EMA::compute(closes, fastPeriod);
    auto slowEMA = analysis::EMA::compute(closes, slowPeriod);
    
    std::string tradeDate = data.back().trade_date;

    if (!isUpTrend(fastEMA) || !isUpTrend(slowEMA)) {
        return std::nullopt;
    }

    if (isGoldenCross(fastEMA, slowEMA)) {
        double confidence = calculateConfidence(fastEMA, slowEMA);
        
        if (confidence < minConfidence) {
            return std::nullopt;
        }
        
        SignalStrength strength = evaluateStrength(confidence);
        return createResult(tsCode, tradeDate, "买入", strength, confidence, "", "buy");
    }

    return std::nullopt;
}

bool EMA17TO25Strategy::isUpTrend(const std::vector<double>& ema) const {
    int trendDays = static_cast<int>(getParameter("trend_days", 3));
    
    if (trendDays <= 0 || ema.size() < static_cast<size_t>(trendDays + 1)) {
        return false;
    }
    
    for (size_t i = ema.size() - trendDays; i < ema.size(); i++) {
        if (ema[i] <= ema[i - 1]) {
            return false;
        }
    }
    return true;
}

bool EMA17TO25Strategy::validateParameters() const {
    int fastPeriod = static_cast<int>(getParameter("fast_period", 17));
    int slowPeriod = static_cast<int>(getParameter("slow_period", 25));
    
    if (fastPeriod <= 0 || slowPeriod <= 0) {
        return false;
    }
    
    if (fastPeriod >= slowPeriod) {
        return false;
    }
    
    return true;
}

bool EMA17TO25Strategy::isGoldenCross(const std::vector<double>& fastEMA, const std::vector<double>& slowEMA) const {
    if (fastEMA.size() < 2 || slowEMA.size() < 2) {
        return false;
    }
    
    size_t idx = fastEMA.size() - 1;
    
    // 当前：快线 > 慢线，前一天：快线 <= 慢线
    return fastEMA[idx] > slowEMA[idx] && fastEMA[idx - 1] <= slowEMA[idx - 1];
}

bool EMA17TO25Strategy::isDeathCross(const std::vector<double>& fastEMA, const std::vector<double>& slowEMA) const {
    if (fastEMA.size() < 2 || slowEMA.size() < 2) {
        return false;
    }
    
    size_t idx = fastEMA.size() - 1;
    
    return fastEMA[idx] < slowEMA[idx] && fastEMA[idx - 1] >= slowEMA[idx - 1];
}

double EMA17TO25Strategy::calculateConfidence(
    const std::vector<double>& fastEMA,
    const std::vector<double>& slowEMA
) const {
    if (fastEMA.empty() || slowEMA.empty()) {
        return 50.0;
    }
    
    int trendDays = static_cast<int>(getParameter("trend_days", 3));
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
