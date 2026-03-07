#ifndef SPDLOG_LOGGER_H
#define SPDLOG_LOGGER_H

#include "ILogger.h"
#include "LogConfig.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/async.h>
#include <vector>

namespace logger {

/**
 * @brief spdlog 日志实现类
 * 单一职责原则：只负责 spdlog 的具体实现
 * 里氏替换原则：可以替换 ILogger 接口的任何使用
 */
class SpdlogLogger : public ILogger {
public:
    /**
     * @brief 构造函数
     * @param config 日志配置
     * @param logger_name 日志器名称
     */
    explicit SpdlogLogger(const LogConfig& config, const std::string& logger_name = "default");

    ~SpdlogLogger() override = default;

    void trace(const std::string& message) override;
    void debug(const std::string& message) override;
    void info(const std::string& message) override;
    void warn(const std::string& message) override;
    void error(const std::string& message) override;
    void critical(const std::string& message) override;
    void setLevel(LogLevel level) override;
    void flush() override;

private:
    /**
     * @brief 初始化日志器
     * @param config 日志配置
     * @param logger_name 日志器名称
     */
    void initialize(const LogConfig& config, const std::string& logger_name);

    /**
     * @brief 转换日志级别
     * @param level 自定义日志级别
     * @return spdlog 日志级别
     */
    spdlog::level::level_enum toSpdlogLevel(LogLevel level) const;

    std::shared_ptr<spdlog::logger> logger_;
};

} // namespace logger

#endif // SPDLOG_LOGGER_H

