#pragma once

#include <functional>

namespace stock_tui {

/**
 * @brief 进度适配器
 * 
 * 将核心库的进度回调适配到 TUI 显示
 */
class ProgressAdapter {
public:
    ProgressAdapter();

    void Update(int total, int completed, int failed, const std::string& status);
    std::function<void(int, int, int, const std::string&)> GetCallback();

private:
    int total_ = 0;
    int completed_ = 0;
    int failed_ = 0;
    std::string status_;
};

} // namespace stock_tui