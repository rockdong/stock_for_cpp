#include "EMA17BreakoutStrategy.h"
#include "../../analysis/indicators/EMA.h"

namespace core {

EMA17BreakoutStrategy::EMA17BreakoutStrategy(const std::map<std::string, double>& params)
    : StrategyBase("EMA17_BREAKOUT", "EMA17突破策略")
{
    // 设置默认参数
    setParameter("ema_period", 17);
    setParameter("min_breakout_pct", 0.0);  // 最小突破幅度百分比
    
    // 应用用户参数
    setParameters(params);
}

std::optional<AnalysisResult> EMA17BreakoutStrategy::analyze(
    const std::string& tsCode,
    const std::vector<StockData>& data
) {
    int emaPeriod = static_cast<int>(getParameter("ema_period", 17));
    double minBreakoutPct = getParameter("min_breakout_pct", 0.0);
    
    // 检查数据是否足够（需要至少 EMA 周期 + 2 天的数据）
    if (!hasEnoughData(data, emaPeriod + 2)) {
        return std::nullopt;
    }
    
    // 提取收盘价
    std::vector<double> closes;
    closes.reserve(data.size());
    for (const auto& d : data) {
        closes.push_back(d.close);
    }
    
    // 计算 EMA
    auto ema = analysis::EMA::compute(closes, emaPeriod);
    
    // 获取最新交易日期
    std::string tradeDate = data.back().trade_date;
    
    // 检测是否发生突破
    if (!isBreakout(closes, ema)) {
        return std::nullopt;
    }
    
    // 计算突破幅度
    double currentPrice = closes.back();
    double currentEMA = ema.back();
    double breakoutPct = calculateBreakoutPercent(currentPrice, currentEMA);
    
    // 检查是否满足最小突破幅度要求
    if (breakoutPct < minBreakoutPct) {
        return std::nullopt;
    }
    
    // 生成买入信号
    return createResult(tsCode, tradeDate, "买入", "buy");
}

bool EMA17BreakoutStrategy::validateParameters() const {
    int emaPeriod = static_cast<int>(getParameter("ema_period", 17));
    double minBreakoutPct = getParameter("min_breakout_pct", 0.0);
    
    if (emaPeriod <= 0) {
        return false;
    }
    
    if (minBreakoutPct < 0) {
        return false;
    }
    
    return true;
}

bool EMA17BreakoutStrategy::isBreakout(
    const std::vector<double>& closes,
    const std::vector<double>& ema
) const {
    if (closes.size() < 2 || ema.size() < 2) {
        return false;
    }
    
    size_t idx = closes.size() - 1;
    
    // 前一天：收盘价 < EMA
    // 当天：收盘价 >= EMA
    bool prevBelow = closes[idx - 1] < ema[idx - 1];
    bool currentAbove = closes[idx] >= ema[idx];
    
    return prevBelow && currentAbove;
}

double EMA17BreakoutStrategy::calculateBreakoutPercent(double price, double emaValue) const {
    if (emaValue == 0) {
        return 0.0;
    }
    
    return ((price - emaValue) / emaValue) * 100.0;
}

} // namespace core

