#ifndef ANALYSIS_INDICATORS_ATR_H
#define ANALYSIS_INDICATORS_ATR_H

#include "../IndicatorBase.h"
#include <ta_libc.h>

namespace analysis {

/**
 * @brief ATR（Average True Range）指标
 * 
 * 平均真实波幅，用于衡量市场波动性
 */
class ATR : public IndicatorBase {
public:
    /**
     * @brief 构造函数
     * @param period 周期（默认 14）
     */
    explicit ATR(int period = 14);
    
    /**
     * @brief 计算 ATR（需要高低收数据）
     * @param high 最高价
     * @param low 最低价
     * @param close 收盘价
     * @return 指标结果
     */
    std::shared_ptr<IndicatorResult> calculate(
        const std::vector<double>& high,
        const std::vector<double>& low,
        const std::vector<double>& close
    ) override;
    
    /**
     * @brief 计算 ATR（只有收盘价时使用收盘价代替高低价）
     * @param prices 价格数据
     * @return 指标结果
     */
    std::shared_ptr<IndicatorResult> calculate(const std::vector<double>& prices) override;
    
    /**
     * @brief 获取指标名称
     * @return 指标名称
     */
    std::string getName() const override { return "ATR"; }
    
    /**
     * @brief 静态计算方法（便捷接口）
     * @param high 最高价
     * @param low 最低价
     * @param close 收盘价
     * @param period 周期
     * @return ATR 值序列
     */
    static std::vector<double> compute(
        const std::vector<double>& high,
        const std::vector<double>& low,
        const std::vector<double>& close,
        int period = 14
    );

private:
    int period_;
};

} // namespace analysis

#endif // ANALYSIS_INDICATORS_ATR_H

