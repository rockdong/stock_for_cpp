#pragma once

#include <string>
#include <vector>

namespace stock_tui {

/**
 * @brief 日志适配器
 * 
 * 将核心库的日志适配到 TUI 显示
 */
class LogAdapter {
public:
    LogAdapter();

    void AddLog(const std::string& level, const std::string& message);
    std::vector<std::string> GetLogs();
    void Clear();

private:
    std::vector<std::string> logs_;
    size_t maxSize_ = 100;  // 最多保存 100 条日志
};

} // namespace stock_tui