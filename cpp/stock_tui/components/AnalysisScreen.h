#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

namespace stock_tui {

/**
 * @brief 分析界面
 * 
 * 显示分析进度、结果统计、失败记录
 */
class AnalysisScreen {
public:
    AnalysisScreen();
    ftxui::Element Render();
    void StartAnalysis();
    void StopAnalysis();

private:
    int total_ = 0;
    int completed_ = 0;
    int failed_ = 0;
    std::string status_ = "idle";
    bool running_ = false;
};

std::unique_ptr<AnalysisScreen> CreateAnalysisScreen();

} // namespace stock_tui