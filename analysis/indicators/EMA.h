#ifndef ANALYSIS_INDICATORS_EMA_H
#define ANALYSIS_INDICATORS_EMA_H

#include "../IndicatorBase.h"
#include <ta_libc.h>

namespace analysis {

/**
 * @brief EMA（指数移动平均线）指标
 * 
 * 指数移动平均线对近期价格赋予更高的权重
 */
class EMA : public IndicatorBase {
public:
    /**
     * @brief 构造函数
     * @param period 周期（默认 12）
     */
    explicit EMA(int period = 12);
    
    /**
     * @brief 计算 EMA
     * @param prices 价格数据
     * @return 指标结果
     */
    std::shared_ptr<IndicatorResult> calculate(const std::vector<double>& prices) override;
    
    /**
     * @brief 获取指标名称
     * @return 指标名称
     */
    std::string getName() const override { return "EMA"; }
    
    /**
     * @brief 静态计算方法（便捷接口）
     * @param prices 价格数据
     * @param period 周期
     * @return EMA 值序列
     */
    static std::vector<double> compute(
        const std::vector<double>& prices,
        int period
    );

private:
    int period_;
};

} // namespace analysis

#endif // ANALYSIS_INDICATORS_EMA_H

