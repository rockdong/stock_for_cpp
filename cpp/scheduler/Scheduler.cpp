#include "Scheduler.h"
#include "TradeCalendar.h"
#include "TimeUtil.h"
#include "Logger.h"
#include "../eventbus/EventBusManager.h"
#include "../eventbus/events/SystemEvents.h"
#include <thread>
#include <chrono>
#include <fstream>
#include <atomic>
#include <csignal>

namespace scheduler {

class Scheduler::Impl {
public:
    Impl(const std::string& executeTime, ExecuteCallback callback)
        : executeTime_(executeTime)
        , callback_(callback)
        , running_(false)
        , onceMode_(false) {
        parseExecuteTime();
    }
    
    void run() {
        running_ = true;
        LOG_INFO("调度器启动，每日执行时间: " + executeTime_);
        
        while (running_) {
            if (shouldRunNow()) {
                LOG_INFO("触发定时任务执行...");
                executeAnalysis();
                if (onceMode_) {
                    LOG_INFO("单次模式执行完成，退出调度器");
                    break;
                }
            }
            
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        
        LOG_INFO("调度器已停止");
    }
    
    void stop() {
        running_ = false;
    }
    
    void setOnceMode(bool once) {
        onceMode_ = once;
    }
    
    bool hasRunToday() {
        std::string today = getCurrentDate();
        std::string lastRunFile = "data/.last_run";
        
        std::ifstream file(lastRunFile);
        if (file.is_open()) {
            std::string lastDate;
            file >> lastDate;
            file.close();
            return lastDate == today;
        }
        return false;
    }
    
    std::string getLastRunDate() const {
        std::string lastRunFile = "data/.last_run";
        std::ifstream file(lastRunFile);
        if (file.is_open()) {
            std::string lastDate;
            file >> lastDate;
            file.close();
            return lastDate;
        }
        return "";
    }
    
private:
    bool shouldRunNow() {
        std::string currentTime = getCurrentTime();
        if (currentTime != executeTime_) {
            return false;
        }
        
        if (hasRunToday()) {
            return false;
        }
        
        if (!TradeCalendar::isTradingDayToday()) {
            LOG_INFO("今日非交易日，跳过执行");
            markAsRun();
            return false;
        }
        
        return true;
    }
    
    void markAsRun() {
        std::string today = getCurrentDate();
        std::string lastRunFile = "data/.last_run";
        
        std::ofstream file(lastRunFile);
        if (file.is_open()) {
            file << today;
            file.close();
            LOG_INFO("已记录执行日期: " + today);
        } else {
            LOG_WARN("无法写入执行记录文件: " + lastRunFile);
        }
    }
    
    void executeAnalysis() {
        auto bus = eventbus::EventBusManager::getInstance().getCoreBus();
        
        bus->postpone(eventbus::events::AppStarted{
            "", ""
        });
        
        try {
            markAsRun();
            callback_();
            LOG_INFO("定时任务执行完成");
        } catch (const std::exception& e) {
            LOG_ERROR("定时任务执行失败: " + std::string(e.what()));
        }
        
        bus->process();
    }
    
    void parseExecuteTime() {
        int hour = 20, minute = 0;
        sscanf(executeTime_.c_str(), "%d:%d", &hour, &minute);
        executeHour_ = hour;
        executeMinute_ = minute;
    }
    
    std::string getCurrentDate() const {
        // 考虑切换时间（默认 02:00），与 calculateAnalysisDate() 逻辑一致
        // 当前时间 < 切换时间时，使用昨天的日期
        time_t now = time(nullptr);
        struct tm* tm_now = localtime(&now);
        
        int currentMinutes = tm_now->tm_hour * 60 + tm_now->tm_min;
        int switchMinutes = 2 * 60; // 02:00
        
        if (currentMinutes < switchMinutes) {
            tm_now->tm_mday -= 1;
            mktime(tm_now); // 规范化 tm 结构
        }
        
        char buf[11];
        strftime(buf, sizeof(buf), "%Y-%m-%d", tm_now);
        return std::string(buf);
    }
    
    std::string getCurrentTime() const {
        time_t now = time(nullptr);
        struct tm* tm_now = localtime(&now);
        char buf[6];
        snprintf(buf, sizeof(buf), "%02d:%02d", tm_now->tm_hour, tm_now->tm_min);
        return std::string(buf);
    }
    
    int getCurrentHour() const {
        time_t now = time(nullptr);
        struct tm* tm_now = localtime(&now);
        return tm_now->tm_hour;
    }
    
    int getCurrentMinute() const {
        time_t now = time(nullptr);
        struct tm* tm_now = localtime(&now);
        return tm_now->tm_min;
    }
    
    std::string executeTime_;
    int executeHour_;
    int executeMinute_;
    ExecuteCallback callback_;
    std::atomic<bool> running_;
    bool onceMode_;
};

Scheduler::Scheduler(const std::string& executeTime, ExecuteCallback callback)
    : pImpl(std::make_unique<Impl>(executeTime, callback)) {}

Scheduler::~Scheduler() = default;

void Scheduler::run() {
    pImpl->run();
}

void Scheduler::stop() {
    pImpl->stop();
}

void Scheduler::setOnceMode(bool once) {
    pImpl->setOnceMode(once);
}

bool Scheduler::hasRunToday() const {
    return pImpl->hasRunToday();
}

std::string Scheduler::getLastRunDate() const {
    return pImpl->getLastRunDate();
}

} // namespace scheduler
