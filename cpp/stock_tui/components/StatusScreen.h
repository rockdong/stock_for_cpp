#pragma once

#include <ftxui/component/component.hpp>

namespace stock_tui {

class StatusScreen {
public:
    StatusScreen();
    ftxui::Element Render();
};

std::unique_ptr<StatusScreen> CreateStatusScreen();

} // namespace stock_tui