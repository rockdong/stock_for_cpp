#include "LogAdapter.h"

namespace stock_tui {

LogAdapter::LogAdapter() = default;

void LogAdapter::AddLog(const std::string& level, const std::string& message) {
    std::string formattedLog = "[" + level + "] " + message;
    logs_.push_back(formattedLog);

    // 保持日志数量不超过最大值
    if (logs_.size() > maxSize_) {
        logs_.erase(logs_.begin());
    }
}

std::vector<std::string> LogAdapter::GetLogs() {
    return logs_;
}

void LogAdapter::Clear() {
    logs_.clear();
}

} // namespace stock_tui