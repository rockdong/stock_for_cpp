#include "UILogSink.h"
#include <algorithm>

namespace logger {

// LogBufferManager 实现

LogBufferManager& LogBufferManager::getInstance() {
    static LogBufferManager instance;
    return instance;
}

LogBufferManager::LogBufferManager() : buffer_(), mutex_(), maxBufferSize_(1000) {
}

LogBufferManager::~LogBufferManager() {
    // 清空缓冲区和订阅者
    std::lock_guard<std::mutex> lock(mutex_);
    buffer_.clear();
}

void LogBufferManager::appendLog(const std::string& message) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // 添加日志消息
    buffer_.push_back(message);
    
    // 如果超过最大大小，删除旧日志
    if (buffer_.size() > maxBufferSize_) {
        buffer_.erase(buffer_.begin(), 
                     buffer_.begin() + (buffer_.size() - maxBufferSize_));
    }
    
    // 通知订阅者（异步通知，避免阻塞）
    notifySubscribers();
}

std::vector<std::string> LogBufferManager::getRecentLogs(int maxLines) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (maxLines <= 0 || buffer_.size() <= static_cast<size_t>(maxLines)) {
        return buffer_;  // 返回全部日志
    }
    
    // 返回最后 maxLines 行日志
    std::vector<std::string> recentLogs;
    size_t startIdx = buffer_.size() - maxLines;
    for (size_t i = startIdx; i < buffer_.size(); ++i) {
        recentLogs.push_back(buffer_[i]);
    }
    
    return recentLogs;
}

std::vector<std::string> LogBufferManager::getAllLogs() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return buffer_;
}

void LogBufferManager::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    buffer_.clear();
}

size_t LogBufferManager::size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return buffer_.size();
}

int LogBufferManager::subscribe(std::function<void()> callback) {
    std::lock_guard<std::mutex> lock(subscriberMutex_);
    
    int subscriptionId = nextSubscriptionId_++;
    subscribers_[subscriptionId] = callback;
    
    return subscriptionId;
}

void LogBufferManager::unsubscribe(int subscriptionId) {
    std::lock_guard<std::mutex> lock(subscriberMutex_);
    subscribers_.erase(subscriptionId);
}

void LogBufferManager::setMaxBufferSize(size_t maxSize) {
    std::lock_guard<std::mutex> lock(mutex_);
    maxBufferSize_ = maxSize;
    
    // 如果当前缓冲区超过新的大小，立即裁剪
    if (buffer_.size() > maxBufferSize_) {
        buffer_.erase(buffer_.begin(), 
                     buffer_.begin() + (buffer_.size() - maxBufferSize_));
    }
}

void LogBufferManager::notifySubscribers() {
    // 在单独线程中通知订阅者（避免阻塞日志写入线程）
    // 注意：这里简化实现，直接在当前线程通知
    // 如果订阅者回调耗时，应该改用异步通知
    
    std::lock_guard<std::mutex> lock(subscriberMutex_);
    for (auto& pair : subscribers_) {
        if (pair.second) {
            pair.second();  // 调用回调函数
        }
    }
}

} // namespace logger