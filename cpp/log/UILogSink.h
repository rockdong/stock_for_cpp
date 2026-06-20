#ifndef UI_LOG_SINK_H
#define UI_LOG_SINK_H

#include <spdlog/sinks/base_sink.h>
#include <spdlog/details/log_msg.h>
#include <mutex>
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <atomic>

namespace logger {

/**
 * @brief UI 日志缓冲区管理器
 * 
 * 管理内存日志缓冲区，供多个 UI 客户端（TUI/GUI）共享访问
 * 支持订阅机制，日志更新时通知订阅者
 */
class LogBufferManager {
public:
    /**
     * @brief 获取单例实例
     */
    static LogBufferManager& getInstance();
    
    /**
     * @brief 添加日志消息到缓冲区
     * @param message 格式化的日志消息
     */
    void appendLog(const std::string& message);
    
    /**
     * @brief 获取最新的日志内容（拷贝）
     * @param maxLines 最大行数（默认50行）
     * @return 日志行列表
     */
    std::vector<std::string> getRecentLogs(int maxLines = 50) const;
    
    /**
     * @brief 获取所有日志内容（拷贝）
     * @return 所有日志行列表
     */
    std::vector<std::string> getAllLogs() const;
    
    /**
     * @brief 清空日志缓冲区
     */
    void clear();
    
    /**
     * @brief 获取日志总行数
     */
    size_t size() const;
    
    /**
     * @brief 订阅日志更新通知
     * @param callback 更新回调函数
     * @return 订阅ID（用于取消订阅）
     */
    int subscribe(std::function<void()> callback);
    
    /**
     * @brief 取消订阅
     * @param subscriptionId 订阅ID
     */
    void unsubscribe(int subscriptionId);
    
    /**
     * @brief 设置最大缓冲区大小（防止内存溢出）
     * @param maxSize 最大行数（默认1000行）
     */
    void setMaxBufferSize(size_t maxSize);
    
    // 禁用拷贝和赋值
    LogBufferManager(const LogBufferManager&) = delete;
    LogBufferManager& operator=(const LogBufferManager&) = delete;
    
private:
    LogBufferManager();
    ~LogBufferManager();
    
    std::vector<std::string> buffer_;
    mutable std::mutex mutex_;  // mutable：允许在 const 成员函数中加锁
    size_t maxBufferSize_ = 1000;
    
    // 订阅机制
    std::unordered_map<int, std::function<void()>> subscribers_;
    std::atomic<int> nextSubscriptionId_{0};
    mutable std::mutex subscriberMutex_;  // mutable：允许在 const 成员函数中加锁
    
    void notifySubscribers();
};

/**
 * @brief spdlog UI Sink（内存缓冲区 sink）
 * 
 * 将日志写入到 LogBufferManager 的内存缓冲区，供 UI 客户端读取
 * 继承 spdlog::sinks::base_sink_mt（多线程安全）
 */
template<typename Mutex>
class UILogSink : public spdlog::sinks::base_sink<Mutex> {
public:
    UILogSink() : bufferManager_(LogBufferManager::getInstance()) {}
    
protected:
    /**
     * @brief sink_it_：将日志消息写入缓冲区
     */
    void sink_it_(const spdlog::details::log_msg& msg) override {
        // 格式化日志消息
        spdlog::memory_buf_t formatted;
        this->formatter_->format(msg, formatted);
        
        // 转换为字符串并追加到缓冲区
        std::string logMessage = fmt::to_string(formatted);
        bufferManager_.appendLog(logMessage);
    }
    
    /**
     * @brief flush_：刷新缓冲区（UI sink 不需要 flush）
     */
    void flush_() override {
        // UI sink 不需要 flush 操作（内存缓冲区）
    }
    
private:
    LogBufferManager& bufferManager_;
};

// 多线程安全的 UI Sink 类型
using UILogSinkMt = UILogSink<std::mutex>;

} // namespace logger

#endif // UI_LOG_SINK_H