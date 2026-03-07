#pragma once

#include <string>
#include <memory>
#include <functional>

namespace scheduler {

/**
 * @brief 定时调度器
 * 
 * 管理每日定时执行任务，支持：
 * - 定时执行（每日指定时间）
 * - 交易日判断
 * - 优雅退出
 */
class Scheduler {
public:
    using ExecuteCallback = std::function<void()>;
    
    /**
     * @brief 构造函数
     * @param executeTime 执行时间，格式 HH:MM
     * @param callback 执行回调
     */
    Scheduler(const std::string& executeTime, ExecuteCallback callback);
    
    /**
     * @brief 启动调度器（阻塞）
     */
    void run();
    
    /**
     * @brief 停止调度器
     */
    void stop();
    
    /**
     * @brief 设置是否只运行一次
     */
    void setOnceMode(bool once);
    
    /**
     * @brief 析构函数
     */
    ~Scheduler();
    
private:
    bool shouldRunNow();
    bool hasRunToday();
    void markAsRun();
    void executeAnalysis();
    std::string getCurrentDate() const;
    std::string getCurrentTime() const;
    int getCurrentHour() const;
    int getCurrentMinute() const;
    
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace scheduler
