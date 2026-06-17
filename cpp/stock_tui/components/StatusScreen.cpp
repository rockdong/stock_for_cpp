#include "StatusScreen.h"
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

namespace stock_tui {

StatusScreen::StatusScreen() = default;

Element StatusScreen::Render() {
    return vbox({
        text("状态监控") | bold | color(Color::Magenta),
        separator(),
        text("(状态界面待实现)") | color(Color::GrayLight),
    });
}

std::unique_ptr<StatusScreen> CreateStatusScreen() {
    return std::make_unique<StatusScreen>();
}

} // namespace stock_tui