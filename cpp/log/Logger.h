// 修改 Logger.h 以支持流式操作
// 使用 spdlog 的 fmt 格式化，避免流式操作问题

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
#include <sstream>
#include <string>

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
    LogConfig config;
    LoggerManager::getInstance().initialize(config);
}

/**
 * @brief 支持流式操作的日志宏
 * 使用 std::ostringstream 来支持 << 操作
 */
#define LOG_TRACE(msg)    do { std::ostringstream oss; oss << msg; logger::getLogger()->trace(oss.str()); } while(0)
#define LOG_DEBUG(msg)    do { std::ostringstream oss; oss << msg; logger::getLogger()->debug(oss.str()); } while(0)
#define LOG_INFO(msg)     do { std::ostringstream oss; oss << msg; logger::getLogger()->info(oss.str()); } while(0)
#define LOG_WARN(msg)     do { std::ostringstream oss; oss << msg; logger::getLogger()->warn(oss.str()); } while(0)
#define LOG_ERROR(msg)    do { std::ostringstream oss; oss << msg; logger::getLogger()->error(oss.str()); } while(0)
#define LOG_CRITICAL(msg) do { std::ostringstream oss; oss << msg; logger::getLogger()->critical(oss.str()); } while(0)

/**
 * @brief 带日志器名称的日志宏定义
 */
#define LOG_TRACE_N(name, msg)    do { std::ostringstream oss; oss << msg; logger::getLogger(name)->trace(oss.str()); } while(0)
#define LOG_DEBUG_N(name, msg)    do { std::ostringstream oss; oss << msg; logger::getLogger(name)->debug(oss.str()); } while(0)
#define LOG_INFO_N(name, msg)     do { std::ostringstream oss; oss << msg; logger::getLogger(name)->info(oss.str()); } while(0)
#define LOG_WARN_N(name, msg)     do { std::ostringstream oss; oss << msg; logger::getLogger(name)->warn(oss.str()); } while(0)
#define LOG_ERROR_N(name, msg)    do { std::ostringstream oss; oss << msg; logger::getLogger(name)->error(oss.str()); } while(0)
#define LOG_CRITICAL_N(name, msg) do { std::ostringstream oss; oss << msg; logger::getLogger(name)->critical(oss.str()); } while(0)

} // namespace logger

#endif // LOGGER_H