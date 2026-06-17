#include "SignalHandler.h"
#include "../scheduler/Scheduler.h"  // 包含 Scheduler 定义
#include "../log/Logger.h"
#include "../utils/RateLimiter.h"
#include <iostream>

namespace stock_exe {

scheduler::Scheduler* SignalHandler::scheduler_ = nullptr;

void SignalHandler::registerHandlers() {
    std::signal(SIGINT, handleSignal);
    std::signal(SIGTERM, handleSignal);
    LOG_INFO("信号处理器已注册");
}

void SignalHandler::setScheduler(scheduler::Scheduler* scheduler) {
    scheduler_ = scheduler;
}

void SignalHandler::handleSignal(int signal) {
    std::string signalName;
    switch (signal) {
        case SIGINT:
            signalName = "SIGINT (Ctrl+C)";
            break;
        case SIGTERM:
            signalName = "SIGTERM (终止信号)";
            break;
        default:
            signalName = "未知信号";
            break;
    }
    
    LOG_INFO("收到退出信号: " + signalName + "，正在优雅关闭...");
    
    // 停止限流器
    RateLimiter::getInstance().stop();
    
    // 停止调度器
    if (scheduler_) {
        scheduler_->stop();
    }
    
    LOG_INFO("优雅关闭完成");
}

} // namespace stock_exe