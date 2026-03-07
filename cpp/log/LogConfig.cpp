#include "LogConfig.h"
#include <algorithm>

namespace logger {

LogConfig::LogConfig() {
    // 从环境变量读取配置
    std::string level_str = getEnvString("LOG_LEVEL", "INFO");
    log_level_ = stringToLogLevel(level_str);

    log_pattern_ = getEnvString("LOG_PATTERN", "[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%t] %v");
    log_file_path_ = getEnvString("LOG_FILE_PATH", "logs/app.log");
    console_enabled_ = getEnvBool("LOG_CONSOLE_ENABLED", true);
    file_enabled_ = getEnvBool("LOG_FILE_ENABLED", true);
    max_file_size_ = getEnvSize("LOG_MAX_FILE_SIZE", 1048576 * 10); // 默认 10MB
    max_files_ = getEnvSize("LOG_MAX_FILES", 3);
    async_enabled_ = getEnvBool("LOG_ASYNC_ENABLED", false);
    async_queue_size_ = getEnvSize("LOG_ASYNC_QUEUE_SIZE", 8192);
}

std::string LogConfig::getEnvString(const char* key, const std::string& default_value) const {
    const char* value = std::getenv(key);
    return value ? std::string(value) : default_value;
}

bool LogConfig::getEnvBool(const char* key, bool default_value) const {
    const char* value = std::getenv(key);
    if (!value) return default_value;
    
    std::string str_value(value);
    // 转换为小写
    std::transform(str_value.begin(), str_value.end(), str_value.begin(), ::tolower);
    
    return str_value == "true" || str_value == "1" || str_value == "yes" || str_value == "on";
}

size_t LogConfig::getEnvSize(const char* key, size_t default_value) const {
    const char* value = std::getenv(key);
    if (!value) return default_value;
    
    try {
        return static_cast<size_t>(std::stoul(value));
    } catch (...) {
        return default_value;
    }
}

} // namespace logger

