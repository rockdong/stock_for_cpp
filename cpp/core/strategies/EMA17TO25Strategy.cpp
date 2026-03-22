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
    double minConfidence = getParameter("min_confidence", 30);
    
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
    
    if (fastEMA.empty() || slowEMA.empty()) {
        return std::nullopt;
    }
    
    std::string tradeDate = data.back().trade_date;
    double currentPrice = closes.back();
    double currFastEMA = fastEMA.back();
    double currSlowEMA = slowEMA.back();

    // 检查是否已发生金叉
    if (isGoldenCross(fastEMA, slowEMA)) {
        if (!isUpTrend(fastEMA) || !isUpTrend(slowEMA)) {
            return std::nullopt;
        }
        
        double confidence = calculateConfidence(fastEMA, slowEMA);
        
        if (confidence < minConfidence) {
            return std::nullopt;
        }
        
        SignalStrength strength = evaluateStrength(confidence);
        return createResult(tsCode, tradeDate, "买入", strength, confidence, "", "buy");
    }

    // 预测金叉：快线在慢线下方但正在接近
    if (currFastEMA < currSlowEMA) {
        double predictedPrice = predictGoldenCrossPrice(currFastEMA, currSlowEMA, fastPeriod, slowPeriod);
        
        if (predictedPrice <= 0 || predictedPrice < currentPrice * 0.9 || predictedPrice > currentPrice * 1.1) {
            return std::nullopt;
        }
        
        double emaGap = (currSlowEMA - currFastEMA) / currSlowEMA;
        if (emaGap > 0.02) {
            return std::nullopt;
        }
        
        if (!isUpTrend(fastEMA)) {
            return std::nullopt;
        }
        
        double confidence = calculatePredictionConfidence(currentPrice, predictedPrice, fastEMA, slowEMA);
        
        if (confidence < minConfidence) {
            return std::nullopt;
        }
        
        SignalStrength strength = evaluateStrength(confidence);
        std::string warning = "预测明日金叉，目标价: " + std::to_string(static_cast<int>(predictedPrice * 100) / 100.0);
        
        return createResult(tsCode, tradeDate, "金叉预警", strength, confidence, warning, "predict");
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

double EMA17TO25Strategy::predictGoldenCrossPrice(
    double fastEMA,
    double slowEMA,
    int fastPeriod,
    int slowPeriod
) const {
    double kFast = 2.0 / (fastPeriod + 1);
    double kSlow = 2.0 / (slowPeriod + 1);
    
    double numerator = slowEMA * (1 - kSlow) - fastEMA * (1 - kFast);
    double denominator = kFast - kSlow;
    
    if (std::abs(denominator) < 0.0001) {
        return -1.0;
    }
    
    return numerator / denominator;
}

double EMA17TO25Strategy::calculatePredictionConfidence(
    double currentPrice,
    double predictedPrice,
    const std::vector<double>& fastEMA,
    const std::vector<double>& slowEMA
) const {
    double baseConfidence = 50.0;
    
    double priceDiff = std::abs(predictedPrice - currentPrice) / currentPrice;
    double proximityScore = std::max(0.0, 20.0 - priceDiff * 200);
    
    double emaGap = (slowEMA.back() - fastEMA.back()) / slowEMA.back();
    double gapScore = std::max(0.0, 20.0 - emaGap * 1000);
    
    int trendDays = static_cast<int>(getParameter("trend_days", 3));
    double trendScore = 0.0;
    
    if (fastEMA.size() >= static_cast<size_t>(trendDays + 1)) {
        int upDays = 0;
        for (size_t i = fastEMA.size() - trendDays + 1; i < fastEMA.size(); i++) {
            if (fastEMA[i] > fastEMA[i - 1]) upDays++;
        }
        trendScore = (static_cast<double>(upDays) / trendDays) * 10.0;
    }
    
    return baseConfidence + proximityScore + gapScore + trendScore;
}

} // namespace core
