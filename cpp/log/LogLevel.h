#ifndef LOG_LEVEL_H
#define LOG_LEVEL_H

#include <string>

namespace logger {

/**
 * @brief 日志级别枚举
 * 定义了所有支持的日志级别
 */
enum class LogLevel {
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    CRITICAL,
    OFF
};

/**
 * @brief 将字符串转换为日志级别
 * @param level 日志级别字符串
 * @return LogLevel 对应的日志级别枚举
 */
inline LogLevel stringToLogLevel(const std::string& level) {
    if (level == "TRACE" || level == "trace") return LogLevel::TRACE;
    if (level == "DEBUG" || level == "debug") return LogLevel::DEBUG;
    if (level == "INFO" || level == "info") return LogLevel::INFO;
    if (level == "WARN" || level == "warn") return LogLevel::WARN;
    if (level == "ERROR" || level == "error") return LogLevel::ERROR;
    if (level == "CRITICAL" || level == "critical") return LogLevel::CRITICAL;
    if (level == "OFF" || level == "off") return LogLevel::OFF;
    return LogLevel::INFO; // 默认级别
}

} // namespace logger

#endif // LOG_LEVEL_H

