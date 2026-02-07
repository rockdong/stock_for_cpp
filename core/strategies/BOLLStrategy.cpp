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

AnalysisResult BOLLStrategy::analyze(
    const std::string& tsCode,
    const std::vector<StockData>& data
) {
    int period = static_cast<int>(getParameter("period", 20));
    double stdDev = getParameter("std_dev", 2.0);
    
    // 检查数据是否足够
    if (!hasEnoughData(data, period + 2)) {
        return createResult(tsCode, "", "数据不足");
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
    double lowerBand = boll.lower.back();
    
    // 获取最新交易日期
    std::string tradeDate = data.back().trade_date;
    
    // 价格触及下轨（买入信号）
    if (currentPrice <= lowerBand) {
        return createResult(tsCode, tradeDate, "买入");
    }
    
    // 价格触及上轨（卖出信号）
    if (currentPrice >= upperBand) {
        return createResult(tsCode, tradeDate, "卖出");
    }
    
    // 无明确信号
    return createResult(tsCode, tradeDate, "持有");
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

