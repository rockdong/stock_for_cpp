#include "LoggerManager.h"
#include <spdlog/spdlog.h>

namespace logger {

const std::string LoggerManager::DEFAULT_LOGGER_NAME = "default";

LoggerManager& LoggerManager::getInstance() {
    static LoggerManager instance;
    return instance;
}

void LoggerManager::initialize(const LogConfig& config) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (initialized_) {
        return;
    }

    default_config_ = config;
    
    // 创建默认日志器
    auto default_logger = LoggerFactory::createDefaultLogger(config, DEFAULT_LOGGER_NAME);
    loggers_[DEFAULT_LOGGER_NAME] = default_logger;
    
    initialized_ = true;
}

LoggerPtr LoggerManager::getLogger() {
    return getLogger(DEFAULT_LOGGER_NAME);
}

LoggerPtr LoggerManager::getLogger(const std::string& logger_name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = loggers_.find(logger_name);
    if (it != loggers_.end()) {
        return it->second;
    }
    
    // 如果日志器不存在，使用默认配置创建一个新的
    if (initialized_) {
        auto logger = LoggerFactory::createDefaultLogger(default_config_, logger_name);
        loggers_[logger_name] = logger;
        return logger;
    }
    
    return nullptr;
}

LoggerPtr LoggerManager::createLogger(const std::string& logger_name, const LogConfig& config) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // 如果已存在，先移除
    auto it = loggers_.find(logger_name);
    if (it != loggers_.end()) {
        loggers_.erase(it);
    }
    
    // 创建新的日志器
    auto logger = LoggerFactory::createDefaultLogger(config, logger_name);
    loggers_[logger_name] = logger;
    
    return logger;
}

void LoggerManager::removeLogger(const std::string& logger_name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // 不允许删除默认日志器
    if (logger_name == DEFAULT_LOGGER_NAME) {
        return;
    }
    
    auto it = loggers_.find(logger_name);
    if (it != loggers_.end()) {
        it->second->flush();
        loggers_.erase(it);
    }
}

void LoggerManager::flushAll() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (auto& pair : loggers_) {
        if (pair.second) {
            pair.second->flush();
        }
    }
    
    spdlog::apply_all([](std::shared_ptr<spdlog::logger> l) { l->flush(); });
}

void LoggerManager::shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // 刷新所有日志
    for (auto& pair : loggers_) {
        if (pair.second) {
            pair.second->flush();
        }
    }
    
    // 清空日志器
    loggers_.clear();
    
    // 关闭 spdlog
    spdlog::shutdown();
    
    initialized_ = false;
}

LoggerManager::~LoggerManager() {
    shutdown();
}

} // namespace logger

