#include "MACrossStrategy.h"
#include "../../utils/MathUtil.h"
#include <numeric>

namespace core {

MACrossStrategy::MACrossStrategy(const std::map<std::string, double>& params)
    : StrategyBase("MA_CROSS", "双均线交叉策略")
{
    // 设置默认参数
    setParameter("short_period", 5);
    setParameter("long_period", 20);
    
    // 应用用户参数
    setParameters(params);
}

AnalysisResult MACrossStrategy::analyze(
    const std::string& tsCode,
    const std::vector<StockData>& data
) {
    int shortPeriod = static_cast<int>(getParameter("short_period", 5));
    int longPeriod = static_cast<int>(getParameter("long_period", 20));
    
    // 检查数据是否足够
    if (!hasEnoughData(data, longPeriod + 2)) {
        return createResult(tsCode, "", "数据不足");
    }
    
    // 提取收盘价
    std::vector<double> closes;
    closes.reserve(data.size());
    for (const auto& d : data) {
        closes.push_back(d.close);
    }
    
    // 计算短期和长期均线
    auto shortMA = calculateMA(closes, shortPeriod);
    auto longMA = calculateMA(closes, longPeriod);
    
    // 获取最新交易日期
    std::string tradeDate = data.back().trade_date;
    
    // 检测金叉（买入信号）
    if (isGoldenCross(shortMA, longMA)) {
        return createResult(tsCode, tradeDate, "买入");
    }
    
    // 检测死叉（卖出信号）
    if (isDeathCross(shortMA, longMA)) {
        return createResult(tsCode, tradeDate, "卖出");
    }
    
    // 无明确信号
    return createResult(tsCode, tradeDate, "持有");
}

bool MACrossStrategy::validateParameters() const {
    int shortPeriod = static_cast<int>(getParameter("short_period", 5));
    int longPeriod = static_cast<int>(getParameter("long_period", 20));
    
    if (shortPeriod <= 0 || longPeriod <= 0) {
        return false;
    }
    
    if (shortPeriod >= longPeriod) {
        return false;
    }
    
    return true;
}

std::vector<double> MACrossStrategy::calculateMA(const std::vector<double>& prices, int period) const {
    std::vector<double> ma;
    ma.reserve(prices.size());
    
    for (size_t i = 0; i < prices.size(); ++i) {
        if (i < static_cast<size_t>(period - 1)) {
            ma.push_back(0.0); // 数据不足，填充0
        } else {
            double sum = 0.0;
            for (int j = 0; j < period; ++j) {
                sum += prices[i - j];
            }
            ma.push_back(sum / period);
        }
    }
    
    return ma;
}

bool MACrossStrategy::isGoldenCross(const std::vector<double>& shortMA, const std::vector<double>& longMA) const {
    if (shortMA.size() < 2 || longMA.size() < 2) {
        return false;
    }
    
    size_t idx = shortMA.size() - 1;
    
    // 当前：短期 > 长期，前一天：短期 <= 长期
    return shortMA[idx] > longMA[idx] && shortMA[idx - 1] <= longMA[idx - 1];
}

bool MACrossStrategy::isDeathCross(const std::vector<double>& shortMA, const std::vector<double>& longMA) const {
    if (shortMA.size() < 2 || longMA.size() < 2) {
        return false;
    }
    
    size_t idx = shortMA.size() - 1;
    
    // 当前：短期 < 长期，前一天：短期 >= 长期
    return shortMA[idx] < longMA[idx] && shortMA[idx - 1] >= longMA[idx - 1];
}

} // namespace core

