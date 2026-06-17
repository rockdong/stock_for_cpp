#include "LogScreen.h"
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

namespace stock_tui {

LogScreen::LogScreen() = default;

Element LogScreen::Render() {
    return vbox({
        text("日志查看") | bold | color(Color::Yellow),
        separator(),
        text("(日志界面待实现)") | color(Color::GrayLight),
    });
}

std::unique_ptr<LogScreen> CreateLogScreen() {
    return std::make_unique<LogScreen>();
}

} // namespace stock_tui