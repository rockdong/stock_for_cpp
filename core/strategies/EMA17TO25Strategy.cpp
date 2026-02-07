//
// Created by 蔡冬 on 2026/2/6.
//

#include "EMA17TO25Strategy.h"
#include "../../analysis/indicators/EMA.h"

namespace core {

EMA17TO25Strategy::EMA17TO25Strategy(const std::map<std::string, double>& params)
    : StrategyBase("EMA17TO25", "EMA17/EMA25交叉策略")
{
    // 设置默认参数
    setParameter("fast_period", 17);
    setParameter("slow_period", 25);
    
    // 应用用户参数
    setParameters(params);
}

std::optional<AnalysisResult> EMA17TO25Strategy::analyze(
    const std::string& tsCode,
    const std::vector<StockData>& data
) {
    int fastPeriod = static_cast<int>(getParameter("fast_period", 17));
    int slowPeriod = static_cast<int>(getParameter("slow_period", 25));
    
    // 检查数据是否足够
    if (!hasEnoughData(data, slowPeriod + 2)) {
        return std::nullopt;
    }
    
    // 提取收盘价
    std::vector<double> closes;
    closes.reserve(data.size());
    for (const auto& d : data) {
        closes.push_back(d.close);
    }
    
    // 使用 analysis 模块的 EMA 计算方法
    auto fastEMA = analysis::EMA::compute(closes, fastPeriod);
    auto slowEMA = analysis::EMA::compute(closes, slowPeriod);
    
    // 获取最新交易日期
    std::string tradeDate = data.back().trade_date;
    
    // 检测金叉（买入信号）
    if (isGoldenCross(fastEMA, slowEMA)) {
        return createResult(tsCode, tradeDate, "买入");
    }
    
    // 检测死叉（卖出信号）
    if (isDeathCross(fastEMA, slowEMA)) {
        return createResult(tsCode, tradeDate, "卖出");
    }
    
    // 无明确信号
    return createResult(tsCode, tradeDate, "持有");
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
    
    // 当前：快线 < 慢线，前一天：快线 >= 慢线
    return fastEMA[idx] < slowEMA[idx] && fastEMA[idx - 1] >= slowEMA[idx - 1];
}

} // namespace core
