#include "SpdlogLogger.h"
#include <iostream>
#include <filesystem>

namespace logger {

SpdlogLogger::SpdlogLogger(const LogConfig& config, const std::string& logger_name) {
    initialize(config, logger_name);
}

void SpdlogLogger::initialize(const LogConfig& config, const std::string& logger_name) {
    try {
        std::vector<spdlog::sink_ptr> sinks;

        // 添加控制台输出
        if (config.isConsoleEnabled()) {
            auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            console_sink->set_level(toSpdlogLevel(config.getLogLevel()));
            console_sink->set_pattern(config.getLogPattern());
            sinks.push_back(console_sink);
        }

        // 添加文件输出
        if (config.isFileEnabled()) {
            // 确保日志目录存在
            std::filesystem::path log_path(config.getLogFilePath());
            std::filesystem::path log_dir = log_path.parent_path();
            
            if (!log_dir.empty() && !std::filesystem::exists(log_dir)) {
                std::filesystem::create_directories(log_dir);
            }

            auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
                config.getLogFilePath(),
                config.getMaxFileSize(),
                config.getMaxFiles()
            );
            file_sink->set_level(toSpdlogLevel(config.getLogLevel()));
            file_sink->set_pattern(config.getLogPattern());
            sinks.push_back(file_sink);
        }

        // 创建日志器
        if (config.isAsyncEnabled()) {
            // 异步日志
            spdlog::init_thread_pool(config.getAsyncQueueSize(), 1);
            logger_ = std::make_shared<spdlog::async_logger>(
                logger_name,
                sinks.begin(),
                sinks.end(),
                spdlog::thread_pool(),
                spdlog::async_overflow_policy::block
            );
        } else {
            // 同步日志
            logger_ = std::make_shared<spdlog::logger>(logger_name, sinks.begin(), sinks.end());
        }

        logger_->set_level(toSpdlogLevel(config.getLogLevel()));
        logger_->flush_on(spdlog::level::err);

        // 注册日志器
        spdlog::register_logger(logger_);

    } catch (const spdlog::spdlog_ex& ex) {
        std::cerr << "日志初始化失败: " << ex.what() << std::endl;
    }
}

void SpdlogLogger::trace(const std::string& message) {
    if (logger_) {
        logger_->trace(message);
    }
}

void SpdlogLogger::debug(const std::string& message) {
    if (logger_) {
        logger_->debug(message);
    }
}

void SpdlogLogger::info(const std::string& message) {
    if (logger_) {
        logger_->info(message);
    }
}

void SpdlogLogger::warn(const std::string& message) {
    if (logger_) {
        logger_->warn(message);
    }
}

void SpdlogLogger::error(const std::string& message) {
    if (logger_) {
        logger_->error(message);
    }
}

void SpdlogLogger::critical(const std::string& message) {
    if (logger_) {
        logger_->critical(message);
    }
}

void SpdlogLogger::setLevel(LogLevel level) {
    if (logger_) {
        logger_->set_level(toSpdlogLevel(level));
    }
}

void SpdlogLogger::flush() {
    if (logger_) {
        logger_->flush();
    }
}

spdlog::level::level_enum SpdlogLogger::toSpdlogLevel(LogLevel level) const {
    switch (level) {
        case LogLevel::TRACE:    return spdlog::level::trace;
        case LogLevel::DEBUG:    return spdlog::level::debug;
        case LogLevel::INFO:     return spdlog::level::info;
        case LogLevel::WARN:     return spdlog::level::warn;
        case LogLevel::ERROR:    return spdlog::level::err;
        case LogLevel::CRITICAL: return spdlog::level::critical;
        case LogLevel::OFF:      return spdlog::level::off;
        default:                 return spdlog::level::info;
    }
}

} // namespace logger

