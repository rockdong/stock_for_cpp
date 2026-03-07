#ifndef ANALYSIS_INDICATORS_RSI_H
#define ANALYSIS_INDICATORS_RSI_H

#include "../IndicatorBase.h"
#include <ta_libc.h>

namespace analysis {

/**
 * @brief RSI（Relative Strength Index）指标
 * 
 * 相对强弱指标，用于衡量价格变动的速度和幅度
 */
class RSI : public IndicatorBase {
public:
    /**
     * @brief 构造函数
     * @param period 周期（默认 14）
     */
    explicit RSI(int period = 14);
    
    /**
     * @brief 计算 RSI
     * @param prices 价格数据
     * @return 指标结果
     */
    std::shared_ptr<IndicatorResult> calculate(const std::vector<double>& prices) override;
    
    /**
     * @brief 获取指标名称
     * @return 指标名称
     */
    std::string getName() const override { return "RSI"; }
    
    /**
     * @brief 静态计算方法（便捷接口）
     * @param prices 价格数据
     * @param period 周期
     * @return RSI 值序列
     */
    static std::vector<double> compute(
        const std::vector<double>& prices,
        int period = 14
    );

private:
    int period_;
};

} // namespace analysis

#endif // ANALYSIS_INDICATORS_RSI_H

