#pragma once

#include <string>
#include <functional>
#include <mutex>
#include <atomic>

namespace stock_exe {

/**
 * @brief 命令行进度显示
 * 
 * 提供进度回调接口，用于显示分析进度
 */
class CliProgress {
public:
    CliProgress();
    
    /**
     * @brief 获取进度回调函数
     * @return 回调函数
     */
    std::function<void(int, int, int, const std::string&)> getCallback();
    
    /**
     * @brief 更新进度显示
     * @param total 总数
     * @param completed 已完成数
     * @param failed 失败数
     * @param status 状态描述
     */
    void update(int total, int completed, int failed, const std::string& status);
    
    /**
     * @brief 重置进度
     */
    void reset();
    
private:
    std::atomic<int> total_;
    std::atomic<int> completed_;
    std::atomic<int> failed_;
    std::string status_;
    std::mutex mutex_;
};

} // namespace stock_exe