#ifndef ANALYSIS_IINDICATOR_H
#define ANALYSIS_IINDICATOR_H

#include <vector>
#include <string>
#include <map>
#include <memory>

namespace analysis {

/**
 * @brief 指标结果基类
 */
struct IndicatorResult {
    std::string name;                                    // 指标名称
    std::vector<double> values;                          // 主要数值序列
    std::map<std::string, std::vector<double>> series;   // 其他数值序列
    
    IndicatorResult() = default;
    explicit IndicatorResult(const std::string& n) : name(n) {}
    
    virtual ~IndicatorResult() = default;
};

/**
 * @brief 技术指标接口
 * 
 * 定义统一的技术指标接口，遵循策略模式
 */
class IIndicator {
public:
    virtual ~IIndicator() = default;
    
    /**
     * @brief 计算指标
     * @param prices 价格数据
     * @return 指标结果
     */
    virtual std::shared_ptr<IndicatorResult> calculate(const std::vector<double>& prices) = 0;
    
    /**
     * @brief 计算指标（带高低收数据）
     * @param high 最高价
     * @param low 最低价
     * @param close 收盘价
     * @return 指标结果
     */
    virtual std::shared_ptr<IndicatorResult> calculate(
        const std::vector<double>& high,
        const std::vector<double>& low,
        const std::vector<double>& close
    ) {
        // 默认实现：只使用收盘价
        return calculate(close);
    }
    
    /**
     * @brief 获取指标名称
     * @return 指标名称
     */
    virtual std::string getName() const = 0;
    
    /**
     * @brief 设置参数
     * @param params 参数映射
     */
    virtual void setParameters(const std::map<std::string, double>& params) = 0;
    
    /**
     * @brief 获取参数
     * @return 参数映射
     */
    virtual std::map<std::string, double> getParameters() const = 0;
};

using IndicatorPtr = std::shared_ptr<IIndicator>;

} // namespace analysis

#endif // ANALYSIS_IINDICATOR_H

