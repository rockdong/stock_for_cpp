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

TradeSignal MACrossStrategy::analyze(
    const std::string& tsCode,
    const std::vector<StockData>& data,
    const Portfolio& portfolio
) {
    int shortPeriod = static_cast<int>(getParameter("short_period", 5));
    int longPeriod = static_cast<int>(getParameter("long_period", 20));
    
    // 检查数据是否足够
    if (!hasEnoughData(data, longPeriod + 2)) {
        return createSignal(tsCode, Signal::NONE, SignalStrength::WEAK, 0.0, 0, 
                          "数据不足，需要至少 " + std::to_string(longPeriod + 2) + " 条数据");
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
    
    // 获取当前价格
    double currentPrice = closes.back();
    
    // 检测金叉（买入信号）
    if (isGoldenCross(shortMA, longMA)) {
        // 计算建议买入数量（使用可用资金的一定比例）
        double availableCash = portfolio.getCash();
        int quantity = static_cast<int>(availableCash * 0.3 / currentPrice / 100) * 100; // 30%资金，100股为单位
        
        if (quantity > 0) {
            return createSignal(
                tsCode, 
                Signal::BUY, 
                SignalStrength::STRONG,
                currentPrice,
                quantity,
                "短期均线(" + std::to_string(shortPeriod) + ")上穿长期均线(" + std::to_string(longPeriod) + ")，金叉买入"
            );
        }
    }
    
    // 检测死叉（卖出信号）
    if (isDeathCross(shortMA, longMA)) {
        // 检查是否持有该股票
        auto position = portfolio.getPosition(tsCode);
        if (position && position->getQuantity() > 0) {
            int quantity = position->getQuantity();
            return createSignal(
                tsCode,
                Signal::SELL,
                SignalStrength::STRONG,
                currentPrice,
                quantity,
                "短期均线(" + std::to_string(shortPeriod) + ")下穿长期均线(" + std::to_string(longPeriod) + ")，死叉卖出"
            );
        }
    }
    
    // 无明确信号
    return createSignal(tsCode, Signal::HOLD, SignalStrength::WEAK, currentPrice, 0, "无明确交易信号");
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

