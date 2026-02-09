#include "EMAConvergenceStrategy.h"
#include "../../analysis/indicators/EMA.h"
#include <cmath>

namespace core {

EMAConvergenceStrategy::EMAConvergenceStrategy(const std::map<std::string, double>& params)
    : StrategyBase("EMA_CONVERGENCE", "EMA收敛策略")
{
    // 设置默认参数
    setParameter("ema_short_period", 17);
    setParameter("ema_long_period", 25);
    setParameter("convergence_threshold", 0.03);
    setParameter("use_percentage", 0.0);  // 0 = false, 1 = true
    
    // 应用用户参数
    setParameters(params);
}

std::optional<AnalysisResult> EMAConvergenceStrategy::analyze(
    const std::string& tsCode,
    const std::vector<StockData>& data
) {
    int shortPeriod = static_cast<int>(getParameter("ema_short_period", 17));
    int longPeriod = static_cast<int>(getParameter("ema_long_period", 25));
    double threshold = getParameter("convergence_threshold", 0.03);
    bool usePercentage = getParameter("use_percentage", 0.0) > 0.5;
    
    // 检查数据是否足够
    if (!hasEnoughData(data, longPeriod + 1)) {
        return std::nullopt;
    }
    
    // 提取收盘价
    std::vector<double> closes;
    closes.reserve(data.size());
    for (const auto& d : data) {
        closes.push_back(d.close);
    }
    
    // 计算 EMA
    auto emaShort = analysis::EMA::compute(closes, shortPeriod);
    auto emaLong = analysis::EMA::compute(closes, longPeriod);
    
    // 获取最新交易日期
    std::string tradeDate = data.back().trade_date;
    
    // 检测是否收敛
    if (!isConverging(emaShort, emaLong, threshold, usePercentage)) {
        return std::nullopt;
    }
    
    // 判断趋势方向
    double currentShort = emaShort.back();
    double currentLong = emaLong.back();
    std::string label = determineTrend(currentShort, currentLong);
    
    // 根据趋势设置操作类型
    std::string opt = (label == "买入") ? "buy" : "hold";
    
    // 生成信号
    return createResult(tsCode, tradeDate, label, opt);
}

bool EMAConvergenceStrategy::validateParameters() const {
    int shortPeriod = static_cast<int>(getParameter("ema_short_period", 17));
    int longPeriod = static_cast<int>(getParameter("ema_long_period", 25));
    double threshold = getParameter("convergence_threshold", 0.03);
    
    if (shortPeriod <= 0 || longPeriod <= 0) {
        return false;
    }
    
    if (shortPeriod >= longPeriod) {
        return false;
    }
    
    if (threshold < 0) {
        return false;
    }
    
    return true;
}

bool EMAConvergenceStrategy::isConverging(
    const std::vector<double>& emaShort,
    const std::vector<double>& emaLong,
    double threshold,
    bool usePercentage
) const {
    if (emaShort.empty() || emaLong.empty()) {
        return false;
    }
    
    double shortValue = emaShort.back();
    double longValue = emaLong.back();
    
    // 使用 calculateDifference 方法计算差值
    double diff = calculateDifference(shortValue, longValue, usePercentage);
    
    // 判断：0 < diff <= threshold
    return diff > 0 && diff <= threshold;
}

double EMAConvergenceStrategy::calculateDifference(
    double emaShort,
    double emaLong,
    bool usePercentage
) const {
    // 计算 EMA25 - EMA17 的差值（不是绝对值）
    double diff = emaLong - emaShort;
    
    if (usePercentage && emaLong != 0) {
        // 计算百分比差异
        return (diff / emaLong) * 100.0;
    }
    
    // 返回差值（可能为正或负）
    return diff;
}

std::string EMAConvergenceStrategy::determineTrend(double emaShort, double emaLong) const {
    // 只有当 0 < (EMA25 - EMA17) <= threshold 时才会调用此函数
    // 此时 EMA25 > EMA17，说明短期均线在长期均线下方
    // 但差值很小，即将金叉，产生买入信号
    return "买入";
}

} // namespace core

