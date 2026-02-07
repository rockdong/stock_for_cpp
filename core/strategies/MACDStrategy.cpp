#include "MACDStrategy.h"

namespace core {

MACDStrategy::MACDStrategy(const std::map<std::string, double>& params)
    : StrategyBase("MACD", "MACD指标策略")
{
    // 设置默认参数
    setParameter("fast_period", 12);
    setParameter("slow_period", 26);
    setParameter("signal_period", 9);
    
    // 应用用户参数
    setParameters(params);
}

AnalysisResult MACDStrategy::analyze(
    const std::string& tsCode,
    const std::vector<StockData>& data
) {
    int fastPeriod = static_cast<int>(getParameter("fast_period", 12));
    int slowPeriod = static_cast<int>(getParameter("slow_period", 26));
    int signalPeriod = static_cast<int>(getParameter("signal_period", 9));
    
    // 检查数据是否足够
    size_t minSize = slowPeriod + signalPeriod + 2;
    if (!hasEnoughData(data, minSize)) {
        return createResult(tsCode, "", "数据不足");
    }
    
    // 提取收盘价
    std::vector<double> closes;
    closes.reserve(data.size());
    for (const auto& d : data) {
        closes.push_back(d.close);
    }
    
    // 计算 MACD
    auto macdResult = calculateMACD(closes, fastPeriod, slowPeriod, signalPeriod);
    
    // 获取最近的 MACD 值
    size_t idx = macdResult.macd.size() - 1;
    double currentMACD = macdResult.macd[idx];
    double currentSignal = macdResult.signal[idx];
    double currentHist = macdResult.histogram[idx];
    
    double prevMACD = macdResult.macd[idx - 1];
    double prevSignal = macdResult.signal[idx - 1];
    double prevHist = macdResult.histogram[idx - 1];
    
    // 获取最新交易日期
    std::string tradeDate = data.back().trade_date;
    
    // 检测 MACD 金叉（买入信号）
    bool macdGoldenCross = (currentMACD > currentSignal) && (prevMACD <= prevSignal);
    bool histogramTurnsPositive = (currentHist > 0) && (prevHist <= 0);
    
    if (macdGoldenCross || histogramTurnsPositive) {
        return createResult(tsCode, tradeDate, "买入");
    }
    
    // 检测 MACD 死叉（卖出信号）
    bool macdDeathCross = (currentMACD < currentSignal) && (prevMACD >= prevSignal);
    bool histogramTurnsNegative = (currentHist < 0) && (prevHist >= 0);
    
    if (macdDeathCross || histogramTurnsNegative) {
        return createResult(tsCode, tradeDate, "卖出");
    }
    
    // 无明确信号
    return createResult(tsCode, tradeDate, "持有");
}

bool MACDStrategy::validateParameters() const {
    int fastPeriod = static_cast<int>(getParameter("fast_period", 12));
    int slowPeriod = static_cast<int>(getParameter("slow_period", 26));
    int signalPeriod = static_cast<int>(getParameter("signal_period", 9));
    
    if (fastPeriod <= 0 || slowPeriod <= 0 || signalPeriod <= 0) {
        return false;
    }
    
    if (fastPeriod >= slowPeriod) {
        return false;
    }
    
    return true;
}

MACDStrategy::MACDResult MACDStrategy::calculateMACD(
    const std::vector<double>& prices,
    int fastPeriod,
    int slowPeriod,
    int signalPeriod
) const {
    MACDResult result;
    
    // 计算快线和慢线 EMA
    auto fastEMA = calculateEMA(prices, fastPeriod);
    auto slowEMA = calculateEMA(prices, slowPeriod);
    
    // 计算 MACD 线（快线 - 慢线）
    result.macd.resize(prices.size());
    for (size_t i = 0; i < prices.size(); ++i) {
        result.macd[i] = fastEMA[i] - slowEMA[i];
    }
    
    // 计算信号线（MACD 的 EMA）
    result.signal = calculateEMA(result.macd, signalPeriod);
    
    // 计算柱状图（MACD - 信号线）
    result.histogram.resize(prices.size());
    for (size_t i = 0; i < prices.size(); ++i) {
        result.histogram[i] = result.macd[i] - result.signal[i];
    }
    
    return result;
}

std::vector<double> MACDStrategy::calculateEMA(const std::vector<double>& prices, int period) const {
    std::vector<double> ema;
    ema.reserve(prices.size());
    
    if (prices.empty()) {
        return ema;
    }
    
    double multiplier = 2.0 / (period + 1);
    
    // 第一个 EMA 值使用简单平均
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

} // namespace core

