#include <iostream>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

// 核心库
#include "../stock_core/stock_core.h"
#include "../core/Stock.h"

// TUI 组件
#include "components/MainScreen.h"
#include "components/MenuComponent.h"

// 版本信息
#include "version.h"

using namespace ftxui;

int main(int argc, char* argv[]) {
    try {
        std::cerr << "[DEBUG] 开始初始化..." << std::endl;

        // 1. 初始化核心库
        if (!stock_core::initialize(".env")) {
            std::cerr << "核心库初始化失败" << std::endl;
            return 1;
        }
        std::cerr << "[DEBUG] 核心库初始化完成" << std::endl;

        // 2. 创建屏幕（全屏模式 - 自动适应终端窗口大小）
        auto screen = ScreenInteractive::Fullscreen();
        std::cerr << "[DEBUG] 屏幕创建完成（Fullscreen 全屏模式）" << std::endl;

        // 3. 创建主界面组件
        std::cerr << "[DEBUG] 创建 MainScreen..." << std::endl;
        auto main_screen = stock_tui::CreateMainScreen();
        std::cerr << "[DEBUG] MainScreen 创建完成" << std::endl;

        // 4. 菜单状态（使用 static 确保生命周期）
        static int selected_menu = 0;
        static std::vector<std::string> menuEntries = {
            "主界面",
            "分析",
            "配置",
            "日志",
            "状态",
            "退出"
        };

        // 5. 组装主界面（交互版本）
        std::cerr << "[DEBUG] 组装主界面..." << std::endl;

        // 创建交互式菜单（直接在 main.cpp 中创建，避免重复定义）
        auto menu = Menu(&menuEntries, &selected_menu);
        
        // 主界面组件（不再使用 GetComponent()，直接用 Render()）
        // auto mainScreenComponent = main_screen->GetComponent();

        // 组合布局容器（只包含菜单）
        auto layout = Container::Horizontal({
            menu,
        });

        // 添加键盘事件处理器
        auto eventHandler = layout | CatchEvent([&](Event event) {
            // 快捷键处理
            if (event == Event::Character('a') || event == Event::Character('A')) {
                // 开始分析
                selected_menu = 1;  // 切换到分析视图
                main_screen->SetCurrentMenu(1);
                main_screen->RefreshData();
                return true;
            }
            
            if (event == Event::Character('r') || event == Event::Character('R')) {
                // 刷新数据
                main_screen->RefreshData();
                return true;
            }
            
            if (event == Event::Character('l') || event == Event::Character('L')) {
                // 查看日志
                selected_menu = 3;
                main_screen->SetCurrentMenu(3);
                return true;
            }
            
            if (event == Event::Character('i') || event == Event::Character('I')) {
                // 导入 CSV 数据
                try {
                    std::cerr << "[DEBUG] 开始导入 CSV 数据..." << std::endl;
                    int imported = stock_core::api::importStocksFromCSV("data/sample_stocks.csv");
                    if (imported > 0) {
                        std::cerr << "[DEBUG] 成功导入 " << imported << " 只股票" << std::endl;
                        main_screen->RefreshData();  // 刷新界面显示导入结果
                    } else if (imported == 0) {
                        std::cerr << "[DEBUG] CSV 文件为空或已导入" << std::endl;
                    } else {
                        std::cerr << "[DEBUG] CSV 导入失败" << std::endl;
                    }
                } catch (const std::exception& e) {
                    std::cerr << "[ERROR] CSV 导入异常: " << e.what() << std::endl;
                }
                return true;
            }
            
            if (event == Event::Character('q') || event == Event::Character('Q') || event == Event::Escape) {
                // 退出系统
                screen.Exit();
                return true;
            }
            
            // 菜单切换事件
            if (event == Event::ArrowUp || event == Event::ArrowDown) {
                // Menu 组件自动处理上下箭头，但我们需要同步状态
                main_screen->SetCurrentMenu(selected_menu);
                return false;  // 让 Menu 组件继续处理
            }
            
            if (event == Event::Return) {
                // Enter 键：执行当前菜单项的动作
                main_screen->SetCurrentMenu(selected_menu);
                
                // 如果在"退出"菜单，直接退出
                if (selected_menu == 5) {
                    screen.Exit();
                    return true;
                }
                
                return false;
            }
            
            return false;  // 其他事件继续传递
        });

        // 最终渲染器
        auto renderer = Renderer(eventHandler, [&] {
            // 关键修复：每次渲染时同步 selected_menu 到 main_screen
            // 这样确保右侧内容始终显示当前选中的菜单对应的内容
            main_screen->SetCurrentMenu(selected_menu);
            
            return vbox({
                text("Stock Analysis System v" + std::string(VERSION_STRING)) | bold | color(Color::Cyan),
                separator(),
                hbox({
                    // 左侧导航菜单
                    vbox({
                        text("导航菜单") | bold | color(Color::Cyan),
                        separator(),
                        menu->Render(),
                    }) | border | size(WIDTH, LESS_THAN, 20),
                    separator(),
                    // 右侧主内容区（根据菜单切换）
                    main_screen->Render() | border | flex,
                }),
                separator(),
                text("快捷键: [A] 分析  [R] 刷新  [I] 导入CSV  [L] 日志  [Q] 退出  [↑↓] 导航") | color(Color::GrayLight),
            });
        });

        std::cerr << "[DEBUG] 主界面组装完成（交互版本）" << std::endl;

        // 6. 启动事件循环
        std::cerr << "[DEBUG] 启动事件循环..." << std::endl;
        screen.Loop(renderer);
        std::cerr << "[DEBUG] 事件循环结束" << std::endl;

        // 7. 清理
        stock_core::shutdown();

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "[EXCEPTION] 类型: " << typeid(e).name() << std::endl;
        std::cerr << "[EXCEPTION] 内容: " << e.what() << std::endl;
        stock_core::shutdown();
        return 1;
    } catch (...) {
        std::cerr << "[EXCEPTION] 未知异常" << std::endl;
        stock_core::shutdown();
        return 1;
    }
}