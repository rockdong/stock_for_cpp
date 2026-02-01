#ifndef ANALYSIS_INDICATOR_FACTORY_H
#define ANALYSIS_INDICATOR_FACTORY_H

#include "IIndicator.h"
#include <string>
#include <map>
#include <memory>

namespace analysis {

/**
 * @brief 指标工厂类
 * 
 * 使用工厂模式创建技术指标实例
 */
class IndicatorFactory {
public:
    /**
     * @brief 指标类型枚举
     */
    enum class IndicatorType {
        MA,      // 移动平均线
        EMA,     // 指数移动平均线
        MACD,    // MACD 指标
        RSI,     // RSI 指标
        KDJ,     // KDJ 指标
        BOLL,    // 布林带
        ATR      // 平均真实波幅
    };
    
    /**
     * @brief 创建指标实例
     * @param type 指标类型
     * @param params 参数映射
     * @return 指标实例
     */
    static IndicatorPtr create(IndicatorType type, const std::map<std::string, double>& params = {});
    
    /**
     * @brief 创建指标实例（通过名称）
     * @param name 指标名称（MA, EMA, MACD, RSI, KDJ, BOLL, ATR）
     * @param params 参数映射
     * @return 指标实例
     */
    static IndicatorPtr create(const std::string& name, const std::map<std::string, double>& params = {});
    
    /**
     * @brief 获取所有支持的指标名称
     * @return 指标名称列表
     */
    static std::vector<std::string> getSupportedIndicators();

private:
    static IndicatorType stringToType(const std::string& name);
};

} // namespace analysis

#endif // ANALYSIS_INDICATOR_FACTORY_H

