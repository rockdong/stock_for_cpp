#include "ConfigScreen.h"
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

namespace stock_tui {

ConfigScreen::ConfigScreen() = default;

Element ConfigScreen::Render() {
    return vbox({
        text("配置界面") | bold | color(Color::Blue),
        separator(),
        text("(配置界面待实现)") | color(Color::GrayLight),
    });
}

std::unique_ptr<ConfigScreen> CreateConfigScreen() {
    return std::make_unique<ConfigScreen>();
}

} // namespace stock_tui