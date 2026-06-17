#include <iostream>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

// 核心库
#include "../stock_core/stock_core.h"

// 版本信息
#include "version.h"

using namespace ftxui;

int main(int argc, char* argv[]) {
    try {
        // 1. 初始化核心库
        if (!stock_core::initialize(".env")) {
            std::cerr << "核心库初始化失败" << std::endl;
            return 1;
        }

        std::cout << "✅ 核心库初始化成功" << std::endl;

        // 2. 创建简单的屏幕
        auto screen = ScreenInteractive::Fullscreen();

        std::cout << "✅ 屏幕创建成功" << std::endl;

        // 3. 创建简单组件
        auto component = Renderer([&] {
            return vbox({
                text("Stock Analysis System v" + std::string(VERSION_STRING)) | bold | color(Color::Cyan),
                separator(),
                text("系统已启动（降级模式运行）") | color(Color::Green),
                text("按 Ctrl+C 或 Esc 退出") | color(Color::GrayLight),
            });
        });

        std::cout << "✅ 组件创建成功" << std::endl;

        // 4. 启动事件循环
        std::cout << "启动 FTXUI 事件循环..." << std::endl;
        screen.Loop(component);

        // 5. 清理
        stock_core::shutdown();
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "异常: " << e.what() << std::endl;
        stock_core::shutdown();
        return 1;
    }
}