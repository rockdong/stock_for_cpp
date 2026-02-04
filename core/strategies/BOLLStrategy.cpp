#include "BOLLStrategy.h"
#include <cmath>

namespace core {

BOLLStrategy::BOLLStrategy(const std::map<std::string, double>& params)
    : StrategyBase("BOLL", "布林带策略")
{
    // 设置默认参数
    setParameter("period", 20);
    setParameter("std_dev", 2.0);
    
    // 应用用户参数
    setParameters(params);
}

TradeSignal BOLLStrategy::analyze(
    const std::string& tsCode,
    const std::vector<StockData>& data,
    const Portfolio& portfolio
) {
    int period = static_cast<int>(getParameter("period", 20));
    double stdDev = getParameter("std_dev", 2.0);
    
    // 检查数据是否足够
    if (!hasEnoughData(data, period + 2)) {
        return createSignal(tsCode, Signal::NONE, SignalStrength::WEAK, 0.0, 0,
                          "数据不足，需要至少 " + std::to_string(period + 2) + " 条数据");
    }
    
    // 提取收盘价
    std::vector<double> closes;
    closes.reserve(data.size());
    for (const auto& d : data) {
        closes.push_back(d.close);
    }
    
    // 计算布林带
    auto boll = calculateBOLL(closes, period, stdDev);
    
    // 获取当前价格和布林带值
    double currentPrice = closes.back();
    double upperBand = boll.upper.back();
    double middleBand = boll.middle.back();
    double lowerBand = boll.lower.back();
    
    // 价格触及下轨（买入信号）
    if (currentPrice <= lowerBand) {
        double availableCash = portfolio.getCash();
        int quantity = static_cast<int>(availableCash * 0.3 / currentPrice / 100) * 100;
        
        if (quantity > 0) {
            double distance = (lowerBand - currentPrice) / lowerBand * 100;
            SignalStrength strength = (distance > 2.0) ? SignalStrength::STRONG : SignalStrength::MEDIUM;
            
            return createSignal(
                tsCode,
                Signal::BUY,
                strength,
                currentPrice,
                quantity,
                "价格触及布林带下轨，买入信号"
            );
        }
    }
    
    // 价格触及上轨（卖出信号）
    if (currentPrice >= upperBand) {
        auto position = portfolio.getPosition(tsCode);
        if (position && position->getQuantity() > 0) {
            int quantity = position->getQuantity();
            double distance = (currentPrice - upperBand) / upperBand * 100;
            SignalStrength strength = (distance > 2.0) ? SignalStrength::STRONG : SignalStrength::MEDIUM;
            
            return createSignal(
                tsCode,
                Signal::SELL,
                strength,
                currentPrice,
                quantity,
                "价格触及布林带上轨，卖出信号"
            );
        }
    }
    
    // 无明确信号
    return createSignal(tsCode, Signal::HOLD, SignalStrength::WEAK, currentPrice, 0, "无明确交易信号");
}

bool BOLLStrategy::validateParameters() const {
    int period = static_cast<int>(getParameter("period", 20));
    double stdDev = getParameter("std_dev", 2.0);
    
    if (period <= 0) {
        return false;
    }
    
    if (stdDev <= 0) {
        return false;
    }
    
    return true;
}

BOLLStrategy::BOLLResult BOLLStrategy::calculateBOLL(
    const std::vector<double>& prices,
    int period,
    double stdDev
) const {
    BOLLResult result;
    result.upper.reserve(prices.size());
    result.middle.reserve(prices.size());
    result.lower.reserve(prices.size());
    
    for (size_t i = 0; i < prices.size(); ++i) {
        if (i < static_cast<size_t>(period - 1)) {
            result.upper.push_back(0.0);
            result.middle.push_back(0.0);
            result.lower.push_back(0.0);
        } else {
            // 计算中轨（移动平均）
            double sum = 0.0;
            for (int j = 0; j < period; ++j) {
                sum += prices[i - j];
            }
            double ma = sum / period;
            
            // 计算标准差
            double variance = 0.0;
            for (int j = 0; j < period; ++j) {
                double diff = prices[i - j] - ma;
                variance += diff * diff;
            }
            double sd = std::sqrt(variance / period);
            
            // 计算上下轨
            result.middle.push_back(ma);
            result.upper.push_back(ma + stdDev * sd);
            result.lower.push_back(ma - stdDev * sd);
        }
    }
    
    return result;
}

} // namespace core

