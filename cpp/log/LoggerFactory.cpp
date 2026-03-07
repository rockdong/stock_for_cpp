#include "LoggerFactory.h"
#include "SpdlogLogger.h"
#include <stdexcept>

namespace logger {

LoggerPtr LoggerFactory::createLogger(
    LoggerType type,
    const LogConfig& config,
    const std::string& logger_name
) {
    switch (type) {
        case LoggerType::SPDLOG:
            return std::make_shared<SpdlogLogger>(config, logger_name);
        
        // 可以在这里添加其他日志库的实现
        // case LoggerType::GLOG:
        //     return std::make_shared<GlogLogger>(config, logger_name);
        
        default:
            throw std::runtime_error("不支持的日志器类型");
    }
}

LoggerPtr LoggerFactory::createDefaultLogger(
    const LogConfig& config,
    const std::string& logger_name
) {
    return createLogger(LoggerType::SPDLOG, config, logger_name);
}

} // namespace logger

