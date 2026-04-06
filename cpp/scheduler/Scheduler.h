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
     * @brief 检查今天是否已执行分析
     * @return true: 今天已执行，false: 今天未执行
     */
    bool hasRunToday() const;
    
    /**
     * @brief 获取最后执行日期
     * @return 最后执行日期字符串（YYYY-MM-DD格式），文件不存在时返回空字符串
     */
    std::string getLastRunDate() const;
    
    /**
     * @brief 析构函数
     */
    ~Scheduler();
    
private:
    bool shouldRunNow();
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
