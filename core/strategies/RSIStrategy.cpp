#include "RSIStrategy.h"

namespace core {

RSIStrategy::RSIStrategy(const std::map<std::string, double>& params)
    : StrategyBase("RSI", "RSI指标策略")
{
    // 设置默认参数
    setParameter("period", 14);
    setParameter("oversold", 30);
    setParameter("overbought", 70);
    
    // 应用用户参数
    setParameters(params);
}

std::optional<AnalysisResult> RSIStrategy::analyze(
    const std::string& tsCode,
    const std::vector<StockData>& data
) {
    int period = static_cast<int>(getParameter("period", 14));
    double oversold = getParameter("oversold", 30);
    double overbought = getParameter("overbought", 70);
    
    // 检查数据是否足够
    if (!hasEnoughData(data, period + 2)) {
        return std::nullopt;
    }
    
    // 提取收盘价
    std::vector<double> closes;
    closes.reserve(data.size());
    for (const auto& d : data) {
        closes.push_back(d.close);
    }
    
    // 计算 RSI
    auto rsi = calculateRSI(closes, period);
    
    // 获取当前 RSI
    double currentRSI = rsi.back();
    
    // 获取最新交易日期
    std::string tradeDate = data.back().trade_date;
    
    // 超卖信号（买入）
    if (currentRSI < oversold) {
        return createResult(tsCode, tradeDate, "买入");
    }
    
    // 超买信号（卖出）
    if (currentRSI > overbought) {
        return createResult(tsCode, tradeDate, "卖出");
    }
    
    // 无明确信号
    return createResult(tsCode, tradeDate, "持有");
}

bool RSIStrategy::validateParameters() const {
    int period = static_cast<int>(getParameter("period", 14));
    double oversold = getParameter("oversold", 30);
    double overbought = getParameter("overbought", 70);
    
    if (period <= 0) {
        return false;
    }
    
    if (oversold <= 0 || oversold >= 50) {
        return false;
    }
    
    if (overbought <= 50 || overbought >= 100) {
        return false;
    }
    
    if (oversold >= overbought) {
        return false;
    }
    
    return true;
}

std::vector<double> RSIStrategy::calculateRSI(const std::vector<double>& prices, int period) const {
    std::vector<double> rsi;
    rsi.reserve(prices.size());
    
    if (prices.size() < 2) {
        return rsi;
    }
    
    // 计算价格变化
    std::vector<double> gains;
    std::vector<double> losses;
    
    for (size_t i = 1; i < prices.size(); ++i) {
        double change = prices[i] - prices[i - 1];
        gains.push_back(change > 0 ? change : 0);
        losses.push_back(change < 0 ? -change : 0);
    }
    
    // 计算平均涨幅和跌幅
    for (size_t i = 0; i < gains.size(); ++i) {
        if (i < static_cast<size_t>(period - 1)) {
            rsi.push_back(50.0); // 数据不足，使用中性值
        } else {
            double avgGain = 0.0;
            double avgLoss = 0.0;
            
            for (int j = 0; j < period; ++j) {
                avgGain += gains[i - j];
                avgLoss += losses[i - j];
            }
            
            avgGain /= period;
            avgLoss /= period;
            
            if (avgLoss == 0.0) {
                rsi.push_back(100.0);
            } else {
                double rs = avgGain / avgLoss;
                rsi.push_back(100.0 - (100.0 / (1.0 + rs)));
            }
        }
    }
    
    return rsi;
}

} // namespace core

