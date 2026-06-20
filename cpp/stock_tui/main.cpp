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
            "分析结果",
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

        // 组合布局容器（包含菜单和配置编辑容器）
        auto layout = Container::Horizontal({
            menu,
            main_screen->GetConfigInputsContainer(),  // 添加配置编辑容器，支持Tab键焦点管理
        });

        // 添加键盘事件处理器
        auto eventHandler = layout | CatchEvent([&](Event event) {
            // Tab键处理：在配置编辑模式下，让Container处理Tab键切换焦点
            if (event == Event::Tab) {
                if (main_screen->IsEditMode()) {
                    // 在编辑模式下，让Container处理Tab键（焦点切换）
                    return false;  // 传递给Container处理
                } else {
                    // 在非编辑模式下，拦截Tab键（不切换焦点）
                    return true;
                }
            }
            
            // Shift+Tab反向切换焦点
            if (event == Event::TabReverse) {
                if (main_screen->IsEditMode()) {
                    return false;  // 传递给Container处理
                } else {
                    return true;
                }
            }
            // === 三种快捷键方案（用户可选择）===
            // 方案1：Ctrl组合键（macOS/Linux/Windows通用，推荐）
            // 方案2：智能字母键（编辑模式自动判断，无需修饰键）
            // 方案3：Alt组合键（备用方案，某些终端支持）
            
            // === 方案1：Ctrl组合键（推荐，macOS用户按Ctrl键）===
            
            // Ctrl+E：切换配置编辑模式
            if (event == Event::CtrlE) {
                main_screen->ToggleEditMode();
                std::cerr << "[DEBUG] Ctrl+E：切换编辑模式" << std::endl;
                return true;
            }
            
            // Ctrl+S：保存配置（仅编辑模式下）
            if (event == Event::CtrlS) {
                if (main_screen->IsEditMode()) {
                    std::cerr << "[DEBUG] Ctrl+S：保存配置..." << std::endl;
                    bool success = main_screen->SaveConfig();
                    if (success) {
                        std::cerr << "[DEBUG] 配置保存成功" << std::endl;
                        main_screen->RefreshData();
                    } else {
                        std::cerr << "[DEBUG] 配置保存失败" << std::endl;
                    }
                }
                return true;
            }
            
            // Ctrl+A：开始分析
            if (event == Event::CtrlA) {
                selected_menu = 1;  // 切换到分析视图
                main_screen->SetCurrentMenu(1);
                main_screen->RefreshData();
                return true;
            }
            
            // Ctrl+R：刷新数据
            if (event == Event::CtrlR) {
                main_screen->RefreshData();
                return true;
            }
            
            // Ctrl+L：查看日志
            if (event == Event::CtrlL) {
                selected_menu = 3;
                main_screen->SetCurrentMenu(3);
                return true;
            }
            
            // Ctrl+I：导入CSV数据
            if (event == Event::CtrlI) {
                try {
                    std::cerr << "[DEBUG] Ctrl+I：开始导入 CSV 数据..." << std::endl;
                    int imported = stock_core::api::importStocksFromCSV("data/sample_stocks.csv");
                    if (imported > 0) {
                        std::cerr << "[DEBUG] 成功导入 " << imported << " 只股票" << std::endl;
                        main_screen->RefreshData();
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
            
            // Ctrl+Q：退出系统
            if (event == Event::CtrlQ) {
                screen.Exit();
                return true;
            }
            
            // Ctrl+P：开始/停止分析（智能切换）
            if (event == Event::CtrlP) {
                if (main_screen->IsAnalyzing()) {
                    // 正在分析：停止分析
                    main_screen->StopAnalysis();
                    std::cerr << "[DEBUG] Ctrl+P：停止分析" << std::endl;
                } else {
                    // 未分析：开始分析
                    selected_menu = 1;  // 切换到分析视图
                    main_screen->SetCurrentMenu(1);
                    main_screen->StartAnalysis();
                    std::cerr << "[DEBUG] Ctrl+P：开始分析" << std::endl;
                }
                return true;
            }
            
            // === 方案2：智能字母键（编辑模式与非编辑模式自动判断）===
            // 编辑模式下：所有字母键完全不拦截（允许正常输入）
            // 非编辑模式下：字母键作为快捷键（拦截）
            
            // 统一处理：编辑模式下不拦截任何字母键
            // 如果当前处于编辑模式，直接跳过所有字母键的快捷键处理
            if (main_screen->IsEditMode()) {
                // 编辑模式：不拦截任何字母键，让Input组件正常接收
                return false;  // 传递给Input组件
            }
            
            // === 非编辑模式下的字母键快捷键处理 ===
            
            // E键：切换配置编辑模式
            if (event == Event::Character('e') || event == Event::Character('E')) {
                main_screen->ToggleEditMode();
                std::cerr << "[DEBUG] E键：进入编辑模式" << std::endl;
                return true;  // 拦截
            }
            
            // P键：开始/停止分析（智能切换）
            if (event == Event::Character('p') || event == Event::Character('P')) {
                if (main_screen->IsAnalyzing()) {
                    // 正在分析：停止分析
                    main_screen->StopAnalysis();
                    std::cerr << "[DEBUG] P键：停止分析" << std::endl;
                } else {
                    // 未分析：开始分析
                    selected_menu = 1;  // 切换到分析视图
                    main_screen->SetCurrentMenu(1);
                    main_screen->StartAnalysis();
                    std::cerr << "[DEBUG] P键：开始分析" << std::endl;
                }
                return true;  // 拦截
            }
            
            // S键：暂无功能（未来可扩展）
            if (event == Event::Character('s') || event == Event::Character('S')) {
                // 非编辑模式下暂无功能
                return false;  // 不拦截
            }
            
            // A键：切换到分析视图
            if (event == Event::Character('a') || event == Event::Character('A')) {
                selected_menu = 1;  // 切换到分析视图
                main_screen->SetCurrentMenu(1);
                main_screen->RefreshData();
                return true;  // 拦截
            }
            
            // R键：刷新数据
            if (event == Event::Character('r') || event == Event::Character('R')) {
                main_screen->RefreshData();
                return true;  // 拦截
            }
            
            // L键：查看日志
            if (event == Event::Character('l') || event == Event::Character('L')) {
                selected_menu = 3;
                main_screen->SetCurrentMenu(3);
                return true;  // 拦截
            }
            
            // I键：导入CSV
            if (event == Event::Character('i') || event == Event::Character('I')) {
                try {
                    std::cerr << "[DEBUG] I键：开始导入 CSV 数据..." << std::endl;
                    int imported = stock_core::api::importStocksFromCSV("data/sample_stocks.csv");
                    if (imported > 0) {
                        std::cerr << "[DEBUG] 成功导入 " << imported << " 只股票" << std::endl;
                        main_screen->RefreshData();
                    } else if (imported == 0) {
                        std::cerr << "[DEBUG] CSV 文件为空或已导入" << std::endl;
                    } else {
                        std::cerr << "[DEBUG] CSV 导入失败" << std::endl;
                    }
                } catch (const std::exception& e) {
                    std::cerr << "[ERROR] CSV 导入异常: " << e.what() << std::endl;
                }
                return true;  // 拦截
            }
            
            // Q键：退出系统
            if (event == Event::Character('q') || event == Event::Character('Q')) {
                screen.Exit();
                return true;  // 拦截
            }
            
            // === 方案3：Alt组合键（备用方案）===
            // 如果用户终端支持Alt键，也可以使用Alt组合键
            
            // Alt+E：切换编辑模式
            if (event == Event::AltE) {
                main_screen->ToggleEditMode();
                std::cerr << "[DEBUG] Alt+E：切换编辑模式" << std::endl;
                return true;
            }
            
            // Alt+S：保存配置
            if (event == Event::AltS) {
                if (main_screen->IsEditMode()) {
                    std::cerr << "[DEBUG] Alt+S：保存配置..." << std::endl;
                    bool success = main_screen->SaveConfig();
                    if (success) {
                        std::cerr << "[DEBUG] 配置保存成功" << std::endl;
                        main_screen->RefreshData();
                    } else {
                        std::cerr << "[DEBUG] 配置保存失败" << std::endl;
                    }
                }
                return true;
            }
            
            // Escape键处理：编辑模式下退出编辑模式，非编辑模式下退出系统
            if (event == Event::Escape) {
                if (main_screen->IsEditMode()) {
                    // 在编辑模式下，Escape键退出编辑模式
                    main_screen->ToggleEditMode();
                    std::cerr << "[DEBUG] Escape键：退出编辑模式" << std::endl;
                    return true;
                } else {
                    // 在非编辑模式下，Escape键退出系统
                    screen.Exit();
                    return true;
                }
            }
            
            // 箭头键处理：编辑模式下切换Input焦点，非编辑模式下导航菜单
            if (event == Event::ArrowUp || event == Event::ArrowDown) {
                if (main_screen->IsEditMode()) {
                    // 编辑模式：传递给configInputsContainer_处理Input焦点切换
                    return false;  // 让Container处理
                } else {
                    // 非编辑模式：同步Menu状态并传递给Menu组件
                    main_screen->SetCurrentMenu(selected_menu);
                    return false;  // 让Menu组件处理
                }
            }
            
            if (event == Event::Return) {
                // Enter 键：执行当前菜单项的动作
                main_screen->SetCurrentMenu(selected_menu);
                
                // 如果在"退出"菜单，直接退出
                if (selected_menu == 6) {
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
                text("快捷键: [Ctrl+E] 配置编辑  [Ctrl+S] 保存  [Ctrl+P] 运行/停止分析  [Ctrl+A] 分析视图  [Ctrl+R] 刷新数据  [Ctrl+I] 导入CSV  [Ctrl+Q] 退出") | color(Color::GrayLight),
        text("或者: [E] [P] [A] [R] [I] [Q]（非编辑模式下）") | color(Color::GrayDark),
        text("提示: [P] 键智能切换分析运行状态，[R] 键刷新股票列表和分析结果，编辑模式下字母键用于输入") | color(Color::Yellow),
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