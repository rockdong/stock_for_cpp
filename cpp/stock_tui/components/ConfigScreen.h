#pragma once

#include <ftxui/component/component.hpp>

namespace stock_tui {

class ConfigScreen {
public:
    ConfigScreen();
    ftxui::Element Render();
};

std::unique_ptr<ConfigScreen> CreateConfigScreen();

} // namespace stock_tui