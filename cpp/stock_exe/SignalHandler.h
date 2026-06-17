#pragma once

#include <csignal>

// 前置声明
namespace scheduler {
    class Scheduler;
}

namespace stock_exe {

/**
 * @brief 信号处理器
 * 
 * 处理系统信号（SIGINT、SIGTERM），实现优雅关闭
 */
class SignalHandler {
public:
    /**
     * @brief 注册信号处理器
     */
    static void registerHandlers();
    
    /**
     * @brief 设置调度器指针（用于优雅关闭）
     * @param scheduler 调度器指针
     */
    static void setScheduler(scheduler::Scheduler* scheduler);
    
private:
    /**
     * @brief 信号处理函数
     * @param signal 信号类型
     */
    static void handleSignal(int signal);
    
    static scheduler::Scheduler* scheduler_;
};

} // namespace stock_exe