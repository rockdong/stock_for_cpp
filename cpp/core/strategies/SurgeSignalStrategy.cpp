#include "SurgeSignalStrategy.h"
#include <algorithm>
#include <cmath>

namespace core {

SurgeSignalStrategy::SurgeSignalStrategy(const std::map<std::string, double>& params)
    : StrategyBase("SURGE_SIGNAL", "暴涨预兆策略")
{
    setParameter("volume_multiplier", 2.0);
    setParameter("shrink_ratio", 0.5);
    setParameter("ma_deviation_threshold", 0.02);
    setParameter("lookback_period", 20);
    
    setParameters(params);
}

std::optional<AnalysisResult> SurgeSignalStrategy::analyze(
    const std::string& tsCode,
    const std::vector<StockData>& data
) {
    if (!hasEnoughData(data, 30)) {
        return std::nullopt;
    }
    
    auto analysis = performAnalysis(tsCode, data);
    std::string tradeDate = data.back().trade_date;
    
    if (analysis.totalScore >= 3) {
        std::string opt = analysis.volumeTest.description + ";" +
                          analysis.shrinkBack.description + ";" +
                          analysis.maConvergence.description + ";" +
                          analysis.macdDivergence.description + ";" +
                          analysis.obvDivergence.description;
        
        return createResult(
            tsCode,
            tradeDate,
            "暴涨预兆",
            evaluateSignalStrength(analysis.totalScore, analysis.confidence),
            analysis.confidence,
            "",
            opt
        );
    }
    
    return createResult(tsCode, tradeDate, "持有");
}

bool SurgeSignalStrategy::validateParameters() const {
    double volumeMultiplier = getParameter("volume_multiplier", 2.0);
    double shrinkRatio = getParameter("shrink_ratio", 0.5);
    double maDeviation = getParameter("ma_deviation_threshold", 0.02);
    int lookback = static_cast<int>(getParameter("lookback_period", 20));
    
    return volumeMultiplier > 1.0 && 
           shrinkRatio > 0.0 && shrinkRatio < 1.0 &&
           maDeviation > 0.0 && maDeviation < 0.1 &&
           lookback >= 10 && lookback <= 60;
}

std::vector<double> SurgeSignalStrategy::calculateMA(const std::vector<double>& prices, int period) const {
    std::vector<double> ma;
    if (prices.size() < period) return ma;
    
    ma.resize(prices.size());
    for (size_t i = 0; i < prices.size(); ++i) {
        if (i < period - 1) {
            ma[i] = 0.0;
        } else {
            double sum = 0.0;
            for (size_t j = i - period + 1; j <= i; ++j) {
                sum += prices[j];
            }
            ma[i] = sum / period;
        }
    }
    return ma;
}

std::vector<double> SurgeSignalStrategy::calculateEMA(const std::vector<double>& prices, int period) const {
    std::vector<double> ema;
    if (prices.empty()) return ema;
    
    ema.reserve(prices.size());
    double multiplier = 2.0 / (period + 1);
    
    double sum = 0.0;
    for (int i = 0; i < period && i < static_cast<int>(prices.size()); ++i) {
        sum += prices[i];
    }
    double prevEMA = sum / std::min(period, static_cast<int>(prices.size()));
    
    for (size_t i = 0; i < prices.size(); ++i) {
        if (i < static_cast<size_t>(period - 1)) {
            ema.push_back(0.0);
        } else if (i == static_cast<size_t>(period - 1)) {
            ema.push_back(prevEMA);
        } else {
            double currentEMA = (prices[i] - prevEMA) * multiplier + prevEMA;
            ema.push_back(currentEMA);
            prevEMA = currentEMA;
        }
    }
    return ema;
}

SurgeSignalStrategy::MACDResult SurgeSignalStrategy::calculateMACD(
    const std::vector<double>& prices, int fast, int slow, int signal
) const {
    MACDResult result;
    
    auto fastEMA = calculateEMA(prices, fast);
    auto slowEMA = calculateEMA(prices, slow);
    
    result.macd.resize(prices.size());
    for (size_t i = 0; i < prices.size(); ++i) {
        result.macd[i] = fastEMA[i] - slowEMA[i];
    }
    
    result.signal = calculateEMA(result.macd, signal);
    
    result.histogram.resize(prices.size());
    for (size_t i = 0; i < prices.size(); ++i) {
        result.histogram[i] = result.macd[i] - result.signal[i];
    }
    
    return result;
}

std::vector<double> SurgeSignalStrategy::calculateOBV(const std::vector<StockData>& data) const {
    std::vector<double> obv;
    if (data.empty()) return obv;
    
    obv.reserve(data.size());
    double cumulative = 0.0;
    
    for (size_t i = 0; i < data.size(); ++i) {
        if (i == 0) {
            cumulative = static_cast<double>(data[i].volume);
        } else {
            if (data[i].close > data[i - 1].close) {
                cumulative += static_cast<double>(data[i].volume);
            } else if (data[i].close < data[i - 1].close) {
                cumulative -= static_cast<double>(data[i].volume);
            }
        }
        obv.push_back(cumulative);
    }
    return obv;
}

SurgeSignalStrategy::SignalResult SurgeSignalStrategy::detectVolumeTest(const std::vector<StockData>& data) const {
    SignalResult result;
    if (data.size() < 6) return result;
    
    double multiplier = getParameter("volume_multiplier", 2.0);
    
    size_t idx = data.size() - 1;
    long currentVolume = data[idx].volume;
    
    double avgVolume = 0.0;
    for (size_t i = idx - 5; i < idx; ++i) {
        avgVolume += static_cast<double>(data[i].volume);
    }
    avgVolume /= 5.0;
    
    double pct_chg = data[idx].pct_chg;
    
    if (currentVolume >= avgVolume * multiplier && pct_chg >= 3.0 && pct_chg <= 7.0) {
        result.detected = true;
        result.strength = std::min(currentVolume / avgVolume / multiplier, 1.0);
        result.description = "倍量试盘(量=" + std::to_string(static_cast<int>(currentVolume / avgVolume * 100)) + "%均量)";
    }
    
    return result;
}

SurgeSignalStrategy::SignalResult SurgeSignalStrategy::detectShrinkBack(const std::vector<StockData>& data) const {
    SignalResult result;
    if (data.size() < 3) return result;
    
    double shrinkRatio = getParameter("shrink_ratio", 0.5);
    
    size_t idx = data.size() - 1;
    long currentVolume = data[idx].volume;
    long prevVolume = data[idx - 1].volume;
    
    double pct_chg = data[idx].pct_chg;
    
    if (pct_chg < 0 && currentVolume < prevVolume * shrinkRatio) {
        result.detected = true;
        result.strength = 1.0 - static_cast<double>(currentVolume) / prevVolume;
        result.description = "缩量回调(量缩" + std::to_string(static_cast<int>((1.0 - currentVolume / prevVolume) * 100)) + "%)";
    }
    
    return result;
}

SurgeSignalStrategy::SignalResult SurgeSignalStrategy::detectMAConvergence(const std::vector<double>& prices) const {
    SignalResult result;
    if (prices.size() < 20) return result;
    
    double threshold = getParameter("ma_deviation_threshold", 0.02);
    
    auto ma5 = calculateMA(prices, 5);
    auto ma10 = calculateMA(prices, 10);
    auto ma20 = calculateMA(prices, 20);
    
    size_t idx = prices.size() - 1;
    double ma5Val = ma5[idx];
    double ma10Val = ma10[idx];
    double ma20Val = ma20[idx];
    
    if (ma5Val <= 0 || ma10Val <= 0 || ma20Val <= 0) return result;
    
    double avgMA = (ma5Val + ma10Val + ma20Val) / 3.0;
    double maxDeviation = std::max({
        std::abs(ma5Val - avgMA) / avgMA,
        std::abs(ma10Val - avgMA) / avgMA,
        std::abs(ma20Val - avgMA) / avgMA
    });
    
    if (maxDeviation <= threshold) {
        result.detected = true;
        result.strength = 1.0 - maxDeviation / threshold;
        result.description = "均线粘合(偏离度=" + std::to_string(static_cast<int>(maxDeviation * 100)) + "%)";
    }
    
    return result;
}

SurgeSignalStrategy::SignalResult SurgeSignalStrategy::detectMACDDivergence(const std::vector<double>& prices) const {
    SignalResult result;
    int lookback = static_cast<int>(getParameter("lookback_period", 20));
    
    if (prices.size() < lookback + 35) return result;
    
    auto macdResult = calculateMACD(prices, 12, 26, 9);
    
    size_t recentStart = prices.size() - lookback;
    size_t prevStart = prices.size() - lookback * 2;
    
    double recentLowPrice = prices[recentStart];
    size_t recentLowIdx = recentStart;
    for (size_t i = recentStart; i < prices.size(); ++i) {
        if (prices[i] < recentLowPrice) {
            recentLowPrice = prices[i];
            recentLowIdx = i;
        }
    }
    
    double prevLowPrice = prices[prevStart];
    size_t prevLowIdx = prevStart;
    for (size_t i = prevStart; i < recentStart; ++i) {
        if (prices[i] < prevLowPrice) {
            prevLowPrice = prices[i];
            prevLowIdx = i;
        }
    }
    
    if (recentLowPrice < prevLowPrice) {
        double recentMACD = macdResult.macd[recentLowIdx];
        double prevMACD = macdResult.macd[prevLowIdx];
        
        if (recentMACD > prevMACD) {
            result.detected = true;
            result.strength = (recentMACD - prevMACD) / std::abs(prevMACD + 0.001);
            result.strength = std::min(result.strength, 1.0);
            result.description = "MACD底背离";
        }
    }
    
    return result;
}

SurgeSignalStrategy::SignalResult SurgeSignalStrategy::detectOBVDivergence(const std::vector<StockData>& data) const {
    SignalResult result;
    int lookback = static_cast<int>(getParameter("lookback_period", 20));
    
    if (data.size() < lookback * 2) return result;
    
    auto obv = calculateOBV(data);
    
    std::vector<double> prices;
    prices.reserve(data.size());
    for (const auto& d : data) {
        prices.push_back(d.close);
    }
    
    size_t recentStart = data.size() - lookback;
    size_t prevStart = data.size() - lookback * 2;
    
    double recentLowPrice = prices[recentStart];
    size_t recentLowIdx = recentStart;
    for (size_t i = recentStart; i < prices.size(); ++i) {
        if (prices[i] < recentLowPrice) {
            recentLowPrice = prices[i];
            recentLowIdx = i;
        }
    }
    
    double prevLowPrice = prices[prevStart];
    size_t prevLowIdx = prevStart;
    for (size_t i = prevStart; i < recentStart; ++i) {
        if (prices[i] < prevLowPrice) {
            prevLowPrice = prices[i];
            prevLowIdx = i;
        }
    }
    
    if (recentLowPrice < prevLowPrice) {
        double recentOBV = obv[recentLowIdx];
        double prevOBV = obv[prevLowIdx];
        
        if (recentOBV > prevOBV) {
            result.detected = true;
            result.strength = (recentOBV - prevOBV) / std::abs(prevOBV + 0.001);
            result.strength = std::min(result.strength, 1.0);
            result.description = "OBV底背离";
        }
    }
    
    return result;
}

SurgeSignalStrategy::SurgeAnalysis SurgeSignalStrategy::performAnalysis(
    const std::string& tsCode, const std::vector<StockData>& data
) const {
    SurgeAnalysis analysis;
    
    analysis.volumeTest = detectVolumeTest(data);
    analysis.shrinkBack = detectShrinkBack(data);
    
    std::vector<double> prices;
    prices.reserve(data.size());
    for (const auto& d : data) {
        prices.push_back(d.close);
    }
    
    analysis.maConvergence = detectMAConvergence(prices);
    analysis.macdDivergence = detectMACDDivergence(prices);
    analysis.obvDivergence = detectOBVDivergence(data);
    
    int score = 0;
    double totalStrength = 0.0;
    
    if (analysis.volumeTest.detected) {
        score += 2;
        totalStrength += analysis.volumeTest.strength;
    }
    if (analysis.shrinkBack.detected) {
        score += 1;
        totalStrength += analysis.shrinkBack.strength;
    }
    if (analysis.maConvergence.detected) {
        score += 1;
        totalStrength += analysis.maConvergence.strength;
    }
    if (analysis.macdDivergence.detected) {
        score += 2;
        totalStrength += analysis.macdDivergence.strength;
    }
    if (analysis.obvDivergence.detected) {
        score += 1;
        totalStrength += analysis.obvDivergence.strength;
    }
    
    analysis.totalScore = score;
    
    int detectedCount = (analysis.volumeTest.detected ? 1 : 0) +
                        (analysis.shrinkBack.detected ? 1 : 0) +
                        (analysis.maConvergence.detected ? 1 : 0) +
                        (analysis.macdDivergence.detected ? 1 : 0) +
                        (analysis.obvDivergence.detected ? 1 : 0);
    
    analysis.confidence = detectedCount > 0 ? totalStrength / detectedCount : 0.0;
    
    return analysis;
}

SignalStrength SurgeSignalStrategy::evaluateSignalStrength(int score, double confidence) const {
    if (score >= 5 && confidence >= 0.7) {
        return SignalStrength::STRONG;
    } else if (score >= 3 && confidence >= 0.5) {
        return SignalStrength::MEDIUM;
    }
    return SignalStrength::WEAK;
}

} // namespace core