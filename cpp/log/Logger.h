#ifndef LOGGER_H
#define LOGGER_H

/**
 * @file Logger.h
 * @brief 日志系统统一头文件
 * 
 * 这个文件提供了日志系统的所有公共接口
 * 使用者只需要包含这一个头文件即可使用完整的日志功能
 */

#include "LogLevel.h"
#include "LogConfig.h"
#include "ILogger.h"
#include "LoggerFactory.h"
#include "LoggerManager.h"

namespace logger {

/**
 * @brief 初始化日志系统
 * @param config 日志配置对象
 * 
 * 使用示例：
 * @code
 * logger::LogConfig config;
 * logger::init(config);
 * @endcode
 */
inline void init(const LogConfig& config) {
    // 使用提供的配置初始化日志管理器
    LoggerManager::getInstance().initialize(config);
}

/**
 * @brief 初始化日志系统（从环境变量）
 * 
 * 使用示例：
 * @code
 * logger::init();
 * @endcode
 */
inline void init() {
    // 创建配置（从环境变量读取）并初始化日志管理器
    LogConfig config;
    LoggerManager::getInstance().initialize(config);
}

/**
 * @brief 便捷的日志宏定义
 */
#define LOG_TRACE(msg)    logger::getLogger()->trace(msg)
#define LOG_DEBUG(msg)    logger::getLogger()->debug(msg)
#define LOG_INFO(msg)     logger::getLogger()->info(msg)
#define LOG_WARN(msg)     logger::getLogger()->warn(msg)
#define LOG_ERROR(msg)    logger::getLogger()->error(msg)
#define LOG_CRITICAL(msg) logger::getLogger()->critical(msg)

/**
 * @brief 带日志器名称的日志宏定义
 */
#define LOG_TRACE_N(name, msg)    logger::getLogger(name)->trace(msg)
#define LOG_DEBUG_N(name, msg)    logger::getLogger(name)->debug(msg)
#define LOG_INFO_N(name, msg)     logger::getLogger(name)->info(msg)
#define LOG_WARN_N(name, msg)     logger::getLogger(name)->warn(msg)
#define LOG_ERROR_N(name, msg)    logger::getLogger(name)->error(msg)
#define LOG_CRITICAL_N(name, msg) logger::getLogger(name)->critical(msg)

} // namespace logger

#endif // LOGGER_H

