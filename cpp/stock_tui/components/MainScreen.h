#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include "../core/Stock.h"  // 包含 Stock 定义（核心数据结构）
#include "../core/AnalysisResult.h"  // 新增：包含 AnalysisResult 定义
#include "../log/UILogSink.h"  // 新增：包含 UI log sink（访问 LogBufferManager）

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
    
    // 新增：配置编辑公共方法
    void ToggleEditMode();  // 切换配置编辑模式
    bool SaveConfig();      // 保存配置到.env文件
    bool IsEditMode() const { return configEditMode_; }  // 检查编辑模式
    
    // 新增：分析运行公共方法
    void StartAnalysis();   // 开始批量分析
    void StopAnalysis();    // 停止分析
    bool IsAnalyzing() const { return isAnalyzing_; }  // 检查是否正在分析

private:
    // 数据状态
    int totalStocks_ = 0;
    int analyzedStocks_ = 0;
    int failedAnalysis_ = 0;
    std::string lastAnalysisTime_ = "未分析";
    std::string systemStatus_ = "系统运行在降级模式";

    // 分析结果状态（新增）
    std::vector<core::AnalysisResult> analysisResults_;  // 分析结果数据缓存
    std::vector<std::string> analysisResultEntries_;     // 分析结果显示条目
    int selectedAnalysisResultIndex_ = 0;                // 当前选中的分析结果索引
    ftxui::Component analysisResultMenu_;                // 分析结果菜单组件
    int totalAnalysisResults_ = 0;                       // 分析结果总数

    // 分析运行状态
    std::atomic<bool> isAnalyzing_{false};  // 是否正在分析（原子变量，线程安全）
    std::string currentAnalyzingStock_ = "";  // 当前分析的股票代码
    std::atomic<int> analysisProgressPercent_{0};  // 分析进度百分比（原子变量）
    std::string analysisStartTime_ = "";  // 分析开始时间
    std::string analysisStatus_ = "就绪";  // 分析状态文本
    std::atomic<int> analyzedCount_{0};  // 已分析数量（原子变量）
    std::atomic<int> failedCount_{0};  // 失败数量（原子变量）

    // 后台分析线程
    std::thread analysisThread_;  // 分析线程
    std::mutex statusMutex_;  // 状态文本互斥锁（保护 analysisStatus_ 和 currentAnalyzingStock_）
    
    // 日志订阅机制（性能优化）
    int logSubscriptionId_{-1};  // LogBufferManager订阅ID
    std::atomic<bool> logUpdated_{false};  // 日志更新标志（通知UI刷新）
    
    // UI 状态
    int selectedTab_ = 0;
    int currentMenu_ = 0;  // 新增：当前菜单索引
    std::vector<std::string> tabs_ = {"概览", "股票列表", "快速操作"};

    // 配置编辑状态
    std::string configApiKey_;
    std::string configDbType_;
    std::string configDbName_;
    std::string configStrategies_;
    std::string configLogLevel_;
    bool configEditMode_ = false;
    std::string configMessage_;

    // 股票列表状态
    std::vector<std::string> stockListEntries_;  // 股票列表显示条目
    int selectedStockIndex_ = 0;  // 当前选中的股票索引
    ftxui::Component stockListMenu_;  // 股票列表菜单组件
    std::vector<core::Stock> stocksData_;  // 股票数据缓存（使用 core::Stock）

    // 组件
    ftxui::Component tabSelector_;
    ftxui::Component content_;
    ftxui::Component configApiKeyInput_;
    ftxui::Component configDbTypeInput_;
    ftxui::Component configDbNameInput_;
    ftxui::Component configStrategiesInput_;
    ftxui::Component configLogLevelInput_;
    ftxui::Component configInputsContainer_;  // 新增：组合所有Input组件，管理焦点

    void BuildUI();
    ftxui::Element RenderOverview();
    ftxui::Element RenderStockList();
    ftxui::Element RenderQuickActions();
    ftxui::Element RenderAnalysis();     // 新增：分析视图
    ftxui::Element RenderAnalysisResults(); // 新增：分析结果视图
    ftxui::Element RenderConfiguration(); // 新增：配置视图
    ftxui::Element RenderLogs();          // 新增：日志视图
    ftxui::Element RenderStatus();        // 新增：状态视图
    
    // 新增：配置保存功能（内部使用）
    void LoadCurrentConfig();
    
public:
    // 新增：获取配置编辑容器（用于焦点管理）
    ftxui::Component GetConfigInputsContainer() const { return configInputsContainer_; }
};

// 工厂函数
std::unique_ptr<MainScreen> CreateMainScreen();

} // namespace stock_tui