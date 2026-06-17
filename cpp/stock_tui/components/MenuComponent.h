#pragma once

#include <ftxui/component/component.hpp>
#include <memory>

namespace stock_tui {

class MainScreen;

/**
 * @brief 菜单导航组件
 */
ftxui::Component CreateMenu(std::unique_ptr<MainScreen>& mainScreen);

} // namespace stock_tui