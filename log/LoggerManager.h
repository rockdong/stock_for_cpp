#ifndef LOGGER_MANAGER_H
#define LOGGER_MANAGER_H

#include "ILogger.h"
#include "LogConfig.h"
#include "LoggerFactory.h"
#include <string>
#include <unordered_map>
#include <mutex>

namespace logger {

/**
 * @brief 日志管理器类
 * 单例模式：全局唯一的日志管理器
 * 单一职责原则：只负责管理日志器的生命周期
 */
class LoggerManager {
public:
    /**
     * @brief 获取单例实例
     * @return 日志管理器引用
     */
    static LoggerManager& getInstance();

    /**
     * @brief 初始化日志系统
     * @param config 日志配置
     */
    void initialize(const LogConfig& config);

    /**
     * @brief 获取默认日志器
     * @return 日志器智能指针
     */
    LoggerPtr getLogger();

    /**
     * @brief 获取指定名称的日志器
     * @param logger_name 日志器名称
     * @return 日志器智能指针
     */
    LoggerPtr getLogger(const std::string& logger_name);

    /**
     * @brief 创建新的日志器
     * @param logger_name 日志器名称
     * @param config 日志配置
     * @return 日志器智能指针
     */
    LoggerPtr createLogger(const std::string& logger_name, const LogConfig& config);

    /**
     * @brief 移除指定的日志器
     * @param logger_name 日志器名称
     */
    void removeLogger(const std::string& logger_name);

    /**
     * @brief 刷新所有日志器
     */
    void flushAll();

    /**
     * @brief 关闭日志系统
     */
    void shutdown();

    // 禁用拷贝和赋值
    LoggerManager(const LoggerManager&) = delete;
    LoggerManager& operator=(const LoggerManager&) = delete;

private:
    LoggerManager() = default;
    ~LoggerManager();

    std::unordered_map<std::string, LoggerPtr> loggers_;
    LogConfig default_config_;
    std::mutex mutex_;
    bool initialized_ = false;

    static const std::string DEFAULT_LOGGER_NAME;
};

/**
 * @brief 便捷的全局日志函数
 */
inline LoggerPtr getLogger() {
    return LoggerManager::getInstance().getLogger();
}

inline LoggerPtr getLogger(const std::string& name) {
    return LoggerManager::getInstance().getLogger(name);
}

} // namespace logger

#endif // LOGGER_MANAGER_H

