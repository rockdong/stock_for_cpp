#ifndef LOG_CONFIG_H
#define LOG_CONFIG_H

#include <string>
#include <cstdlib>
#include "LogLevel.h"

namespace logger {

/**
 * @brief 日志配置类
 * 单一职责原则：只负责管理日志配置
 * 从环境变量中读取配置信息
 */
class LogConfig {
public:
    /**
     * @brief 构造函数，从环境变量加载配置
     */
    LogConfig();

    // Getters
    LogLevel getLogLevel() const { return log_level_; }
    const std::string& getLogPattern() const { return log_pattern_; }
    const std::string& getLogFilePath() const { return log_file_path_; }
    bool isConsoleEnabled() const { return console_enabled_; }
    bool isFileEnabled() const { return file_enabled_; }
    size_t getMaxFileSize() const { return max_file_size_; }
    size_t getMaxFiles() const { return max_files_; }
    bool isAsyncEnabled() const { return async_enabled_; }
    size_t getAsyncQueueSize() const { return async_queue_size_; }

private:
    /**
     * @brief 从环境变量获取字符串值
     * @param key 环境变量名
     * @param default_value 默认值
     * @return 环境变量值或默认值
     */
    std::string getEnvString(const char* key, const std::string& default_value) const;

    /**
     * @brief 从环境变量获取布尔值
     * @param key 环境变量名
     * @param default_value 默认值
     * @return 环境变量值或默认值
     */
    bool getEnvBool(const char* key, bool default_value) const;

    /**
     * @brief 从环境变量获取整数值
     * @param key 环境变量名
     * @param default_value 默认值
     * @return 环境变量值或默认值
     */
    size_t getEnvSize(const char* key, size_t default_value) const;

    LogLevel log_level_;
    std::string log_pattern_;
    std::string log_file_path_;
    bool console_enabled_;
    bool file_enabled_;
    size_t max_file_size_;
    size_t max_files_;
    bool async_enabled_;
    size_t async_queue_size_;
};

} // namespace logger

#endif // LOG_CONFIG_H

