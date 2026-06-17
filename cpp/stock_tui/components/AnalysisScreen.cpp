#include "AnalysisScreen.h"
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

namespace stock_tui {

AnalysisScreen::AnalysisScreen() = default;

Element AnalysisScreen::Render() {
    return vbox({
        text("分析界面") | bold | color(Color::Green),
        separator(),
        hbox({
            vbox({
                text("进度") | bold,
                gauge((float)completed_ / total_) | color(Color::Green),
                text(std::to_string(completed_) + "/" + std::to_string(total_)),
            }) | border | flex,
            vbox({
                text("统计") | bold,
                separator(),
                text("已完成: " + std::to_string(completed_)),
                text("失败数: " + std::to_string(failed_)),
                text("状态: " + status_),
            }) | border | flex,
        }),
        separator(),
        text("  [S] 开始分析"),
        text("  [X] 停止分析"),
    });
}

void AnalysisScreen::StartAnalysis() {
    running_ = true;
    status_ = "running";
    // TODO: 调用 stock_core API
}

void AnalysisScreen::StopAnalysis() {
    running_ = false;
    status_ = "stopped";
}

std::unique_ptr<AnalysisScreen> CreateAnalysisScreen() {
    return std::make_unique<AnalysisScreen>();
}

} // namespace stock_tui