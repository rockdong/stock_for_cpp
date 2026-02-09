#ifndef CORE_STRATEGIES_EMA_CONVERGENCE_STRATEGY_H
#define CORE_STRATEGIES_EMA_CONVERGENCE_STRATEGY_H

#include "../Strategy.h"

namespace core {

/**
 * @brief EMA 收敛策略
 * 
 * 检测 EMA25 略高于 EMA17 的情况（即将金叉）
 * - 计算 EMA25 - EMA17 的差值（不是绝对值）
 * - 当 0 < 差值 <= 阈值时，产生买入信号
 * - 用于预测即将发生的金叉
 * 
 * 参数：
 * - ema_short_period: 短期 EMA 周期（默认 17）
 * - ema_long_period: 长期 EMA 周期（默认 25）
 * - convergence_threshold: 收敛阈值（默认 0.03）
 * - use_percentage: 是否使用百分比模式（默认 false，使用绝对值）
 */
class EMAConvergenceStrategy : public StrategyBase {
public:
    explicit EMAConvergenceStrategy(const std::map<std::string, double>& params = {});
    
    std::optional<AnalysisResult> analyze(
        const std::string& tsCode,
        const std::vector<StockData>& data
    ) override;
    
    bool validateParameters() const override;

private:
    /**
     * @brief 检测 EMA 是否满足收敛条件
     * @param emaShort 短期 EMA 序列
     * @param emaLong 长期 EMA 序列
     * @param threshold 收敛阈值
     * @param usePercentage 是否使用百分比模式
     * @return 是否满足条件：0 < (EMA25 - EMA17) <= threshold
     */
    bool isConverging(
        const std::vector<double>& emaShort,
        const std::vector<double>& emaLong,
        double threshold,
        bool usePercentage
    ) const;
    
    /**
     * @brief 计算两条 EMA 的差值
     * @param emaShort 短期 EMA 值
     * @param emaLong 长期 EMA 值
     * @param usePercentage 是否使用百分比模式
     * @return 差值 (EMA25 - EMA17)，可能为正或负
     */
    double calculateDifference(
        double emaShort,
        double emaLong,
        bool usePercentage
    ) const;
    
    /**
     * @brief 判断趋势方向
     * @param emaShort 短期 EMA 值
     * @param emaLong 长期 EMA 值
     * @return "买入"（满足条件时，即将金叉）
     */
    std::string determineTrend(double emaShort, double emaLong) const;
};

} // namespace core

#endif // CORE_STRATEGIES_EMA_CONVERGENCE_STRATEGY_H

