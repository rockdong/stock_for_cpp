#include "CliProgress.h"
#include <iostream>
#include <iomanip>
#include "../log/Logger.h"

namespace stock_exe {

CliProgress::CliProgress()
    : total_(0)
    , completed_(0)
    , failed_(0)
    , status_("等待开始") {
}

std::function<void(int, int, int, const std::string&)> CliProgress::getCallback() {
    return [this](int total, int completed, int failed, const std::string& status) {
        this->update(total, completed, failed, status);
    };
}

void CliProgress::update(int total, int completed, int failed, const std::string& status) {
    total_ = total;
    completed_ = completed;
    failed_ = failed;
    
    {
        std::lock_guard<std::mutex> lock(mutex_);
        status_ = status;
    }
    
    // 计算进度百分比
    double percentage = (total > 0) ? (completed * 100.0 / total) : 0.0;
    
    // 输出进度条
    std::cout << "\r";
    std::cout << "[";
    
    // 进度条长度 50 个字符
    int barWidth = 50;
    int pos = static_cast<int>(barWidth * percentage / 100.0);
    
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) {
            std::cout << "█";
        } else if (i == pos) {
            std::cout << "▌";
        } else {
            std::cout << " ";
        }
    }
    
    std::cout << "] ";
    std::cout << std::setw(5) << std::fixed << std::setprecision(1) << percentage << "%";
    std::cout << " | 完成: " << completed << "/" << total;
    std::cout << " | 失败: " << failed;
    std::cout << " | " << status;
    
    std::cout << std::flush;
    
    // 完成时换行
    if (completed >= total) {
        std::cout << std::endl;
        LOG_INFO("分析完成: " << completed << "/" << total << "，失败: " << failed);
    }
}

void CliProgress::reset() {
    total_ = 0;
    completed_ = 0;
    failed_ = 0;
    
    {
        std::lock_guard<std::mutex> lock(mutex_);
        status_ = "等待开始";
    }
    
    std::cout << "\r" << std::string(120, ' ') << "\r" << std::flush;
}

} // namespace stock_exe