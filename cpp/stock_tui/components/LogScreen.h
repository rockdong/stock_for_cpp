#pragma once

#include <ftxui/component/component.hpp>

namespace stock_tui {

class LogScreen {
public:
    LogScreen();
    ftxui::Element Render();
};

std::unique_ptr<LogScreen> CreateLogScreen();

} // namespace stock_tui