#ifndef CORE_STRATEGIES_EMA17_BREAKOUT_STRATEGY_H
#define CORE_STRATEGIES_EMA17_BREAKOUT_STRATEGY_H

#include "../Strategy.h"

namespace core {

/**
 * @brief EMA17 突破策略
 * 
 * 检测收盘价首次站上 EMA17 均线的情况
 * - 前一天收盘价 < EMA17
 * - 当天收盘价 >= EMA17
 * - 产生买入信号
 * 
 * 参数：
 * - ema_period: EMA 周期（默认 17）
 * - min_breakout_pct: 最小突破幅度百分比（默认 0，表示不限制）
 */
class EMA17BreakoutStrategy : public StrategyBase {
public:
    explicit EMA17BreakoutStrategy(const std::map<std::string, double>& params = {});
    
    std::optional<AnalysisResult> analyze(
        const std::string& tsCode,
        const std::vector<StockData>& data
    ) override;
    
    bool validateParameters() const override;

private:
    /**
     * @brief 检测是否首次突破 EMA
     * @param closes 收盘价序列
     * @param ema EMA 序列
     * @return 是否发生突破
     */
    bool isBreakout(const std::vector<double>& closes, const std::vector<double>& ema) const;
    
    /**
     * @brief 计算突破幅度百分比
     * @param price 当前价格
     * @param emaValue EMA 值
     * @return 突破幅度百分比
     */
    double calculateBreakoutPercent(double price, double emaValue) const;
};

} // namespace core

#endif // CORE_STRATEGIES_EMA17_BREAKOUT_STRATEGY_H

