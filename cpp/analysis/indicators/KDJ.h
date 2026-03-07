#ifndef ANALYSIS_INDICATORS_KDJ_H
#define ANALYSIS_INDICATORS_KDJ_H

#include "../IndicatorBase.h"
#include <ta_libc.h>

namespace analysis {

/**
 * @brief KDJ 结果
 */
struct KDJResult : IndicatorResult {
    std::vector<double> k;  // K 线
    std::vector<double> d;  // D 线
    std::vector<double> j;  // J 线
    
    KDJResult() : IndicatorResult("KDJ") {}
};

/**
 * @brief KDJ（随机指标）
 * 
 * KDJ 指标是一个超买超卖指标，由 K、D、J 三条线组成
 */
class KDJ : public IndicatorBase {
public:
    /**
     * @brief 构造函数
     * @param fastK_Period K 线周期（默认 9）
     * @param slowK_Period K 线平滑周期（默认 3）
     * @param slowD_Period D 线平滑周期（默认 3）
     */
    explicit KDJ(int fastK_Period = 9, int slowK_Period = 3, int slowD_Period = 3);
    
    /**
     * @brief 计算 KDJ（需要高低收数据）
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
     * @brief 计算 KDJ（只有收盘价时使用收盘价代替高低价）
     * @param prices 价格数据
     * @return 指标结果
     */
    std::shared_ptr<IndicatorResult> calculate(const std::vector<double>& prices) override;
    
    /**
     * @brief 获取指标名称
     * @return 指标名称
     */
    std::string getName() const override { return "KDJ"; }
    
    /**
     * @brief 静态计算方法（便捷接口）
     * @param high 最高价
     * @param low 最低价
     * @param close 收盘价
     * @param fastK_Period K 线周期
     * @param slowK_Period K 线平滑周期
     * @param slowD_Period D 线平滑周期
     * @return K 值序列
     */
    static std::vector<double> compute(
        const std::vector<double>& high,
        const std::vector<double>& low,
        const std::vector<double>& close,
        int fastK_Period = 9,
        int slowK_Period = 3,
        int slowD_Period = 3
    );

private:
    int fastK_Period_;
    int slowK_Period_;
    int slowD_Period_;
};

} // namespace analysis

#endif // ANALYSIS_INDICATORS_KDJ_H

