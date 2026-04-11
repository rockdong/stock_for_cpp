#ifndef CORE_STRATEGY_FACTORY_H
#define CORE_STRATEGY_FACTORY_H

#include <string>
#include <memory>
#include <map>
#include <functional>
#include "Strategy.h"

namespace core {

/**
 * @brief 策略工厂
 * 
 * 使用工厂模式创建不同类型的策略
 * 遵循开闭原则（OCP）和单一职责原则（SRP）
 */
class StrategyFactory {
public:
    /**
     * @brief 策略类型枚举
     */
    enum class StrategyType {
        MA_CROSS,       // 均线交叉策略
        MACD,           // MACD 策略
        RSI,            // RSI 策略
        BOLL,           // 布林带策略
        GRID,           // 网格交易策略
        EMA17TO25,      // EMA17/EMA25 交叉策略
        EMA17_BREAKOUT, // EMA17 突破策略
        EMA_CONVERGENCE,// EMA 收敛策略
        EMA25_GREATER_17_PRICE_MATCH, // EMA25 大于 EMA17 且收盘价与 EMA17 小数点后两位相等策略
        EMA25_CROSSOVER,      // EMA17 上穿 EMA25 金叉策略
        SURGE_SIGNAL,         // 暴涨预兆策略
        CUSTOM          // 自定义策略
    };
    
    /**
     * @brief 创建策略
     * @param type 策略类型
     * @param params 策略参数
     * @return 策略指针
     */
    static StrategyPtr create(StrategyType type, const std::map<std::string, double>& params = {});
    
    /**
     * @brief 根据名称创建策略
     * @param name 策略名称
     * @param params 策略参数
     * @return 策略指针
     */
    static StrategyPtr create(const std::string& name, const std::map<std::string, double>& params = {});
    
    /**
     * @brief 注册自定义策略
     * @param name 策略名称
     * @param creator 策略创建函数
     */
    static void registerStrategy(
        const std::string& name,
        std::function<StrategyPtr(const std::map<std::string, double>&)> creator
    );
    
    /**
     * @brief 获取所有支持的策略名称
     */
    static std::vector<std::string> getSupportedStrategies();
    
    /**
     * @brief 获取策略类型对应的名称
     */
    static std::string getStrategyName(StrategyType type);

private:
    // 自定义策略注册表
    static std::map<std::string, std::function<StrategyPtr(const std::map<std::string, double>&)>> creators_;
};

} // namespace core

#endif // CORE_STRATEGY_FACTORY_H

