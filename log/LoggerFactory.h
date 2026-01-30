#ifndef LOGGER_FACTORY_H
#define LOGGER_FACTORY_H

#include "ILogger.h"
#include "LogConfig.h"
#include <string>

namespace logger {

/**
 * @brief 日志工厂类
 * 工厂模式：负责创建不同类型的日志器
 * 单一职责原则：只负责日志器的创建
 */
class LoggerFactory {
public:
    /**
     * @brief 日志器类型枚举
     */
    enum class LoggerType {
        SPDLOG,  // spdlog 实现
        // 可以扩展其他日志库实现
        // GLOG,
        // BOOST_LOG,
    };

    /**
     * @brief 创建日志器
     * @param type 日志器类型
     * @param config 日志配置
     * @param logger_name 日志器名称
     * @return 日志器智能指针
     */
    static LoggerPtr createLogger(
        LoggerType type,
        const LogConfig& config,
        const std::string& logger_name = "default"
    );

    /**
     * @brief 创建默认日志器（spdlog）
     * @param config 日志配置
     * @param logger_name 日志器名称
     * @return 日志器智能指针
     */
    static LoggerPtr createDefaultLogger(
        const LogConfig& config,
        const std::string& logger_name = "default"
    );

private:
    LoggerFactory() = default;
    ~LoggerFactory() = default;
};

} // namespace logger

#endif // LOGGER_FACTORY_H

