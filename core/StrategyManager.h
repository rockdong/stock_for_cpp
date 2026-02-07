#ifndef CORE_STRATEGY_MANAGER_H
#define CORE_STRATEGY_MANAGER_H

#include <string>
#include <vector>
#include <memory>
#include <map>
#include "Strategy.h"

namespace core {

/**
 * @brief 策略管理器
 * 
 * 负责从配置文件中读取策略列表并自动注册
 * 支持动态加载和管理多个策略
 */
class StrategyManager {
public:
    /**
     * @brief 获取单例实例
     */
    static StrategyManager& getInstance();

    /**
     * @brief 从配置文件初始化策略
     * @param strategies_config 策略配置字符串（如 "EMA17TO25;BOLL;MACD"）
     * @return 是否初始化成功
     */
    bool initialize(const std::string& strategies_config);

    /**
     * @brief 从环境变量初始化策略
     * @param env_key 环境变量名（默认 "STRATEGIES"）
     * @return 是否初始化成功
     */
    bool initializeFromEnv(const std::string& env_key = "STRATEGIES");

    /**
     * @brief 获取所有已注册的策略
     * @return 策略列表
     */
    std::vector<StrategyPtr> getStrategies() const;

    /**
     * @brief 根据名称获取策略
     * @param name 策略名称
     * @return 策略指针（如果不存在则返回 nullptr）
     */
    StrategyPtr getStrategy(const std::string& name) const;

    /**
     * @brief 获取已注册的策略名称列表
     * @return 策略名称列表
     */
    std::vector<std::string> getStrategyNames() const;

    /**
     * @brief 检查策略是否已注册
     * @param name 策略名称
     * @return 是否已注册
     */
    bool hasStrategy(const std::string& name) const;

    /**
     * @brief 手动注册策略
     * @param strategy 策略指针
     * @return 是否注册成功
     */
    bool registerStrategy(StrategyPtr strategy);

    /**
     * @brief 清除所有已注册的策略
     */
    void clear();

    /**
     * @brief 获取已注册策略的数量
     * @return 策略数量
     */
    size_t count() const;

    /**
     * @brief 对所有策略进行分析
     * @param tsCode 股票代码
     * @param data 历史数据
     * @return 所有策略的分析结果（策略名称 -> 分析结果）
     */
    std::map<std::string, std::optional<AnalysisResult>> analyzeAll(
        const std::string& tsCode,
        const std::vector<StockData>& data
    ) const;

    // 禁用拷贝和赋值
    StrategyManager(const StrategyManager&) = delete;
    StrategyManager& operator=(const StrategyManager&) = delete;

private:
    StrategyManager() = default;
    ~StrategyManager() = default;

    /**
     * @brief 解析策略配置字符串
     * @param config 配置字符串（如 "EMA17TO25;BOLL;MACD"）
     * @return 策略名称列表
     */
    std::vector<std::string> parseConfig(const std::string& config) const;

    /**
     * @brief 根据名称创建策略
     * @param name 策略名称
     * @return 策略指针（如果名称无效则返回 nullptr）
     */
    StrategyPtr createStrategy(const std::string& name) const;

    /**
     * @brief 标准化策略名称（转换为大写）
     * @param name 策略名称
     * @return 标准化后的名称
     */
    std::string normalizeName(const std::string& name) const;

    std::map<std::string, StrategyPtr> strategies_;
};

} // namespace core

#endif // CORE_STRATEGY_MANAGER_H

