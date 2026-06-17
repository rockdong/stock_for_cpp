#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <memory>

namespace stock_tui {

/**
 * @brief 主界面屏幕
 * 
 * 显示系统概览、股票列表、快速操作
 */
class MainScreen {
public:
    MainScreen();
    ~MainScreen();

    ftxui::Component GetComponent();
    ftxui::Element Render();

    void RefreshData();
    void SetCurrentMenu(int menuIndex);  // 新增：设置当前菜单

private:
    // 数据状态
    int totalStocks_ = 0;
    int analyzedStocks_ = 0;
    int failedAnalysis_ = 0;
    std::string lastAnalysisTime_ = "未分析";
    std::string systemStatus_ = "系统运行在降级模式";

    // UI 状态
    int selectedTab_ = 0;
    int currentMenu_ = 0;  // 新增：当前菜单索引
    std::vector<std::string> tabs_ = {"概览", "股票列表", "快速操作"};

    // 组件
    ftxui::Component tabSelector_;
    ftxui::Component content_;

    void BuildUI();
    ftxui::Element RenderOverview();
    ftxui::Element RenderStockList();
    ftxui::Element RenderQuickActions();
    ftxui::Element RenderAnalysis();     // 新增：分析视图
    ftxui::Element RenderConfiguration(); // 新增：配置视图
    ftxui::Element RenderLogs();          // 新增：日志视图
    ftxui::Element RenderStatus();        // 新增：状态视图
};

// 工厂函数
std::unique_ptr<MainScreen> CreateMainScreen();

} // namespace stock_tui