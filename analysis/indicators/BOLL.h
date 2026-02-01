#ifndef ANALYSIS_INDICATORS_BOLL_H
#define ANALYSIS_INDICATORS_BOLL_H

#include "../IndicatorBase.h"
#include <ta_libc.h>

namespace analysis {

/**
 * @brief BOLL 结果
 */
struct BOLLResult : IndicatorResult {
    std::vector<double> upper;   // 上轨
    std::vector<double> middle;  // 中轨
    std::vector<double> lower;   // 下轨
    
    BOLLResult() : IndicatorResult("BOLL") {}
};

/**
 * @brief BOLL（布林带）指标
 * 
 * 布林带由上轨、中轨、下轨三条线组成，用于衡量价格波动
 */
class BOLL : public IndicatorBase {
public:
    /**
     * @brief 构造函数
     * @param period 周期（默认 20）
     * @param nbDevUp 上轨标准差倍数（默认 2.0）
     * @param nbDevDn 下轨标准差倍数（默认 2.0）
     */
    explicit BOLL(int period = 20, double nbDevUp = 2.0, double nbDevDn = 2.0);
    
    /**
     * @brief 计算 BOLL
     * @param prices 价格数据
     * @return 指标结果
     */
    std::shared_ptr<IndicatorResult> calculate(const std::vector<double>& prices) override;
    
    /**
     * @brief 获取指标名称
     * @return 指标名称
     */
    std::string getName() const override { return "BOLL"; }
    
    /**
     * @brief 静态计算方法（便捷接口）
     * @param prices 价格数据
     * @param period 周期
     * @param nbDevUp 上轨标准差倍数
     * @param nbDevDn 下轨标准差倍数
     * @return 中轨值序列
     */
    static std::vector<double> compute(
        const std::vector<double>& prices,
        int period = 20,
        double nbDevUp = 2.0,
        double nbDevDn = 2.0
    );

private:
    int period_;
    double nbDevUp_;
    double nbDevDn_;
};

} // namespace analysis

#endif // ANALYSIS_INDICATORS_BOLL_H

