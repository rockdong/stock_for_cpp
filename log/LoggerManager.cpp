#include "LoggerManager.h"
#include <spdlog/spdlog.h>
#include "Logger.h"

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
    // 使用 try_lock 避免在析构时出现死锁或 mutex 已销毁的问题
    std::unique_lock<std::mutex> lock(mutex_, std::try_to_lock);
    
    if (!lock.owns_lock()) {
        // 如果无法获取锁，说明可能在析构阶段，直接返回
        return;
    }
    
    if (!initialized_) {
        return;
    }
    
    // 刷新所有日志
    for (auto& pair : loggers_) {
        if (pair.second) {
            try {
                pair.second->flush();
            } catch (...) {
                // 忽略刷新时的异常
            }
        }
    }
    
    // 清空日志器
    loggers_.clear();
    
    // 关闭 spdlog
    try {
        spdlog::shutdown();
    } catch (...) {
        // 忽略关闭时的异常
    }
    
    initialized_ = false;
}

LoggerManager::~LoggerManager() {
    // 在析构时不使用锁，直接清理
    try {
        // 刷新所有日志
        for (auto& pair : loggers_) {
            if (pair.second) {
                try {
                    pair.second->flush();
                } catch (...) {
                    // 忽略异常
                }
            }
        }
        
        // 清空日志器
        loggers_.clear();
        
        // 关闭 spdlog
        spdlog::shutdown();
    } catch (...) {
        // 析构函数中不抛出异常
    }
}

} // namespace logger

