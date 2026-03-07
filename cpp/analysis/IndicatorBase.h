#ifndef ANALYSIS_INDICATOR_BASE_H
#define ANALYSIS_INDICATOR_BASE_H

#include "IIndicator.h"
#include <stdexcept>

namespace analysis {

/**
 * @brief 指标基类
 * 
 * 提供通用功能，遵循模板方法模式
 */
class IndicatorBase : public IIndicator {
public:
    virtual ~IndicatorBase() = default;
    
    /**
     * @brief 设置参数
     * @param params 参数映射
     */
    void setParameters(const std::map<std::string, double>& params) override {
        parameters_ = params;
    }
    
    /**
     * @brief 获取参数
     * @return 参数映射
     */
    std::map<std::string, double> getParameters() const override {
        return parameters_;
    }
    
protected:
    std::map<std::string, double> parameters_;
    
    /**
     * @brief 获取参数值
     * @param key 参数名
     * @param defaultValue 默认值
     * @return 参数值
     */
    double getParameter(const std::string& key, double defaultValue) const {
        auto it = parameters_.find(key);
        if (it != parameters_.end()) {
            return it->second;
        }
        return defaultValue;
    }
    
    /**
     * @brief 验证数据有效性
     * @param data 数据
     * @param minSize 最小数据量
     * @throws std::invalid_argument 数据无效时抛出异常
     */
    void validateData(const std::vector<double>& data, size_t minSize = 1) const {
        if (data.empty()) {
            throw std::invalid_argument("数据不能为空");
        }
        if (data.size() < minSize) {
            throw std::invalid_argument("数据量不足，至少需要 " + std::to_string(minSize) + " 个数据点");
        }
    }
    
    /**
     * @brief 验证多组数据长度一致
     * @param data1 数据1
     * @param data2 数据2
     * @param data3 数据3
     * @throws std::invalid_argument 数据长度不一致时抛出异常
     */
    void validateDataLength(
        const std::vector<double>& data1,
        const std::vector<double>& data2,
        const std::vector<double>& data3
    ) const {
        if (data1.size() != data2.size() || data1.size() != data3.size()) {
            throw std::invalid_argument("数据长度不一致");
        }
    }
};

} // namespace analysis

#endif // ANALYSIS_INDICATOR_BASE_H

