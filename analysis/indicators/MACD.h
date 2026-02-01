#ifndef ANALYSIS_INDICATORS_MACD_H
#define ANALYSIS_INDICATORS_MACD_H

#include "../IndicatorBase.h"
#include <ta_libc.h>

namespace analysis {

/**
 * @brief MACD 结果
 */
struct MACDResult : IndicatorResult {
    std::vector<double> macd;       // MACD 线
    std::vector<double> signal;     // 信号线
    std::vector<double> histogram;  // 柱状图
    
    MACDResult() : IndicatorResult("MACD") {}
};

/**
 * @brief MACD（Moving Average Convergence Divergence）指标
 * 
 * MACD 是一个趋势跟踪动量指标
 */
class MACD : public IndicatorBase {
public:
    /**
     * @brief 构造函数
     * @param fastPeriod 快线周期（默认 12）
     * @param slowPeriod 慢线周期（默认 26）
     * @param signalPeriod 信号线周期（默认 9）
     */
    explicit MACD(int fastPeriod = 12, int slowPeriod = 26, int signalPeriod = 9);
    
    /**
     * @brief 计算 MACD
     * @param prices 价格数据
     * @return 指标结果
     */
    std::shared_ptr<IndicatorResult> calculate(const std::vector<double>& prices) override;
    
    /**
     * @brief 获取指标名称
     * @return 指标名称
     */
    std::string getName() const override { return "MACD"; }
    
    /**
     * @brief 静态计算方法（便捷接口）
     * @param prices 价格数据
     * @param fastPeriod 快线周期
     * @param slowPeriod 慢线周期
     * @param signalPeriod 信号线周期
     * @return MACD 值序列
     */
    static std::vector<double> compute(
        const std::vector<double>& prices,
        int fastPeriod = 12,
        int slowPeriod = 26,
        int signalPeriod = 9
    );

private:
    int fastPeriod_;
    int slowPeriod_;
    int signalPeriod_;
};

} // namespace analysis

#endif // ANALYSIS_INDICATORS_MACD_H

