#ifndef ANALYSIS_INDICATORS_MA_H
#define ANALYSIS_INDICATORS_MA_H

#include "../IndicatorBase.h"
#include <ta_libc.h>

namespace analysis {

/**
 * @brief MA（移动平均线）指标
 * 
 * 移动平均线是最常用的技术指标之一，用于平滑价格数据
 */
class MA : public IndicatorBase {
public:
    /**
     * @brief 构造函数
     * @param period 周期（默认 20）
     * @param maType MA 类型（默认 SMA）
     */
    explicit MA(int period = 20, TA_MAType maType = TA_MAType_SMA);
    
    /**
     * @brief 计算 MA
     * @param prices 价格数据
     * @return 指标结果
     */
    std::shared_ptr<IndicatorResult> calculate(const std::vector<double>& prices) override;
    
    /**
     * @brief 获取指标名称
     * @return 指标名称
     */
    std::string getName() const override { return "MA"; }
    
    /**
     * @brief 静态计算方法（便捷接口）
     * @param prices 价格数据
     * @param period 周期
     * @param maType MA 类型
     * @return MA 值序列
     */
    static std::vector<double> compute(
        const std::vector<double>& prices,
        int period,
        TA_MAType maType = TA_MAType_SMA
    );

private:
    int period_;
    TA_MAType maType_;
};

} // namespace analysis

#endif // ANALYSIS_INDICATORS_MA_H

