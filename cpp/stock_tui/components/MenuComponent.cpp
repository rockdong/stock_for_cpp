#include "MenuComponent.h"
#include "MainScreen.h"
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

namespace stock_tui {

Component CreateMenu(std::unique_ptr<MainScreen>& mainScreen) {
    // 菜单状态使用 static 确保生命周期
    static int selected = 0;
    static std::vector<std::string> menuEntries = {
        "主界面",
        "分析",
        "配置",
        "日志",
        "状态",
        "退出"
    };

    // 创建交互式菜单
    auto menu = Menu(&menuEntries, &selected);

    // 渲染菜单容器（不再使用 CatchEvent，直接渲染）
    auto menuRenderer = Renderer(menu, [=] {
        return vbox({
            text("导航菜单") | bold | color(Color::Cyan),
            separator(),
            menu->Render(),
        });
    });

    return menuRenderer;
}

} // namespace stock_tui