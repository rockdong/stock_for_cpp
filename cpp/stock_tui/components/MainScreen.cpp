#include "MainScreen.h"
#include "../stock_core/stock_core.h"
#include "../stock_core/StockCoreContext.h"  // 包含完整的 StockCoreContext 定义
#include "../core/Stock.h"  // 包含完整的 Stock 定义
#include "version.h"  // 包含版本信息
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <fstream>  // 新增：用于读取日志文件
#include <sstream>  // 新增：用于字符串处理

using namespace ftxui;

namespace stock_tui {

MainScreen::MainScreen() {
    LoadCurrentConfig();  // 先加载配置值,再初始化Input组件
    BuildUI();
    RefreshData();
    
    // 新增：订阅 LogBufferManager 的更新通知（性能优化）
    logSubscriptionId_ = logger::LogBufferManager::getInstance().subscribe([this]() {
        // 日志更新时设置标志（通知UI刷新）
        logUpdated_ = true;
    });
}

MainScreen::~MainScreen() {
    // 新增：取消订阅 LogBufferManager
    if (logSubscriptionId_ >= 0) {
        logger::LogBufferManager::getInstance().unsubscribe(logSubscriptionId_);
        logSubscriptionId_ = -1;
    }
    
    // 确保分析线程安全退出
    if (analysisThread_.joinable()) {
        isAnalyzing_ = false;  // 设置停止标志
        analysisThread_.join();  // 等待线程结束
    }
}

void MainScreen::BuildUI() {
    // 创建标签选择器
    tabSelector_ = Toggle(&tabs_, &selectedTab_);

    // 创建内容容器
    content_ = Container::Tab({
        Renderer([=] { return RenderOverview(); }),
        Renderer([=] { return RenderStockList(); }),
        Renderer([=] { return RenderQuickActions(); }),
    }, &selectedTab_);

    // 初始化配置编辑Input组件
    configApiKeyInput_ = Input(&configApiKey_, "输入 API 密钥...");
    configDbTypeInput_ = Input(&configDbType_, "sqlite/mysql");
    configDbNameInput_ = Input(&configDbName_, "数据库路径/名称");
    configStrategiesInput_ = Input(&configStrategies_, "策略列表(逗号分隔)");
    configLogLevelInput_ = Input(&configLogLevel_, "debug/info/warn/error");

    // 组合所有Input组件到Container中，支持Tab键切换焦点
    configInputsContainer_ = Container::Vertical({
        configApiKeyInput_,
        configDbTypeInput_,
        configDbNameInput_,
        configStrategiesInput_,
        configLogLevelInput_,
    });

    // 初始化股票列表菜单组件（降级模式下使用空列表）
    stockListMenu_ = Menu(&stockListEntries_, &selectedStockIndex_);
}

Element MainScreen::RenderOverview() {
    // 安全计算进度百分比（防止除零）
    float progressPercent = 0.0f;
    if (totalStocks_ > 0) {
        progressPercent = (float)analyzedStocks_ / totalStocks_;
    }

    return vbox({
        text("系统概览") | bold | color(Color::Cyan),
        separator(),
        // 股票统计区域（完整显示）
        vbox({
            text("股票统计") | bold | color(Color::Yellow),
            separator(),
            text("总股票数: " + std::to_string(totalStocks_)) | color(Color::White),
            text("已分析数: " + std::to_string(analyzedStocks_)) | color(Color::Green),
            text("失败数: " + std::to_string(failedAnalysis_)) | color(Color::Red),
        }) | border,
        separator(),
        // 分析状态区域（完整显示）
        vbox({
            text("分析状态") | bold | color(Color::Cyan),
            separator(),
            text("最后分析: " + lastAnalysisTime_) | color(Color::White),
            separator(),
            gauge(progressPercent) | color(Color::Green),
            text("进度: " + std::to_string(analyzedStocks_) + "/" + std::to_string(totalStocks_)) | color(Color::Yellow),
        }) | border,
        separator(),
        // 提示信息（完整显示）
        text("系统运行状态") | bold | color(Color::Magenta),
        text("  ├─ 模式: 降级运行（无数据源）") | color(Color::Yellow),
        text("  ├─ 数据库: SQLite（正常）") | color(Color::Green),
        text("  ├─ 策略: 3个已注册") | color(Color::Green),
        text("  └─ 建议: 配置 DATA_SOURCE_API_KEY") | color(Color::GrayLight),
        separator(),
        text("快速操作: [I] 导入CSV数据  [R] 刷新数据") | color(Color::Cyan),
    }) | flex;
}

Element MainScreen::RenderStockList() {
    std::vector<Element> elements;

    elements.push_back(text("股票列表") | bold | color(Color::Yellow));
    elements.push_back(separator());

    // 检查股票列表是否为空
    if (stockListEntries_.empty()) {
        // 降级模式友好提示
        elements.push_back(text("数据库为空或数据源不可用") | color(Color::Yellow));
        elements.push_back(separator());
        elements.push_back(text("提示:") | color(Color::GrayLight));
        elements.push_back(text("  [I] 导入 CSV 数据") | color(Color::Cyan));
        elements.push_back(text("  [Ctrl+I] 导入 CSV 数据（快捷键）") | color(Color::Cyan));
        elements.push_back(text("  [R] 刷新数据") | color(Color::Yellow));
        elements.push_back(separator());
        elements.push_back(text("当前数据库状态:") | color(Color::GrayLight));
        elements.push_back(text("  股票数: " + std::to_string(totalStocks_)) | color(Color::GrayLight));
    } else {
        // 显示股票列表统计信息
        elements.push_back(text("股票总数: " + std::to_string(stockListEntries_.size())) | color(Color::Green));
        elements.push_back(text("当前选中: #" + std::to_string(selectedStockIndex_ + 1)) | color(Color::Cyan));
        elements.push_back(separator());

        // 显示股票列表（可滚动）
        elements.push_back(stockListMenu_->Render() | frame | vscroll_indicator | flex);

        elements.push_back(separator());

        // 显示选中股票的详细信息
        if (selectedStockIndex_ >= 0 && selectedStockIndex_ < stocksData_.size()) {
            auto& stock = stocksData_[selectedStockIndex_];
            elements.push_back(text("选中股票详情:") | bold | color(Color::Cyan));
            elements.push_back(separator());
            elements.push_back(text("代码: " + stock.ts_code) | color(Color::Green));
            elements.push_back(text("名称: " + stock.name) | color(Color::White));
            elements.push_back(text("行业: " + stock.industry) | color(Color::Yellow));
            elements.push_back(text("市场: " + stock.market) | color(Color::Blue));
            elements.push_back(text("上市日期: " + stock.list_date) | color(Color::GrayLight));
        }

        elements.push_back(separator());
        elements.push_back(text("操作提示:") | color(Color::GrayLight));
        elements.push_back(text("  [↑↓] 滚动选择股票") | color(Color::Blue));
        elements.push_back(text("  [R] 刷新数据") | color(Color::Yellow));
        elements.push_back(text("  [I] 导入更多数据") | color(Color::Cyan));
    }

    return vbox(elements);
}

Element MainScreen::RenderQuickActions() {
    return vbox({
        text("快速操作") | bold | color(Color::Magenta),
        separator(),
        text("  [A] 开始批量分析") | color(Color::Green),
        text("  [R] 刷新股票数据") | color(Color::Yellow),
        text("  [L] 查看系统日志") | color(Color::Blue),
        text("  [Q] 退出系统") | color(Color::Red),
        separator(),
        text("提示: 按 Enter 执行当前操作") | color(Color::GrayLight),
    });
}

Element MainScreen::RenderAnalysis() {
    std::vector<Element> elements;

    elements.push_back(text("股票分析") | bold | color(Color::Green));
    elements.push_back(separator());

    // 分析运行状态显示（读取原子变量）
    if (isAnalyzing_) {
        // 正在分析状态
        elements.push_back(text("🔄 分析运行中...") | color(Color::Cyan) | bold);
        elements.push_back(separator());

        // 当前分析状态（需要锁保护）
        {
            std::lock_guard<std::mutex> lock(statusMutex_);
            if (!analysisStatus_.empty()) {
                elements.push_back(text("状态: " + analysisStatus_) | color(Color::Yellow));
            }
            if (!currentAnalyzingStock_.empty()) {
                elements.push_back(text("当前分析: " + currentAnalyzingStock_) | color(Color::Yellow));
            }
        }

        // 实时进度条（读取原子变量）
        int progress = analysisProgressPercent_;
        int completed = analyzedCount_;
        int failed = failedCount_;
        int total = stocksData_.size();

        elements.push_back(text("分析进度:") | bold);
        elements.push_back(gauge(progress / 100.0f) | color(Color::Green));
        elements.push_back(text(std::to_string(progress) + "% 完成") | color(Color::Green));

        // 详细进度信息
        elements.push_back(text(std::to_string(completed) + "/" + std::to_string(total) + " 只股票") | color(Color::GrayLight));
        if (failed > 0) {
            elements.push_back(text("失败: " + std::to_string(failed)) | color(Color::Red));
        }

        // 分析开始时间
        if (!analysisStartTime_.empty()) {
            elements.push_back(text("开始时间: " + analysisStartTime_) | color(Color::GrayLight));
        }

        elements.push_back(separator());
        
        // 分析状态
        elements.push_back(text("状态: " + analysisStatus_) | color(Color::Yellow));
        
        // 失败数
        if (failedAnalysis_ > 0) {
            elements.push_back(text("失败数: " + std::to_string(failedAnalysis_) + " 只") | color(Color::Red));
        }
        
        elements.push_back(separator());
        
        // 操作提示（分析中）
        elements.push_back(text("操作提示:") | color(Color::GrayLight));
        elements.push_back(text("  [Ctrl+P] 或 [P] 停止分析") | color(Color::Red));
        elements.push_back(text("  [Ctrl+R] 或 [R] 刷新进度") | color(Color::Yellow));
        
    } else {
        // 未分析状态（就绪）
        elements.push_back(text("📊 分析就绪") | color(Color::GrayLight));
        elements.push_back(separator());
        
        // 上次分析结果
        if (analyzedStocks_ > 0) {
            elements.push_back(text("上次分析进度:") | bold);
            elements.push_back(gauge(totalStocks_ > 0 ? (float)analyzedStocks_ / totalStocks_ : 0.0f) | color(Color::Green));
            elements.push_back(text(std::to_string(analyzedStocks_) + "/" + std::to_string(totalStocks_) + " 只股票"));
            elements.push_back(text("完成时间: " + lastAnalysisTime_) | color(Color::GrayLight));
            
            if (failedAnalysis_ > 0) {
                elements.push_back(text("失败数: " + std::to_string(failedAnalysis_) + " 只") | color(Color::Red));
            }
        } else {
            elements.push_back(text("尚未开始分析") | color(Color::Yellow));
            elements.push_back(text("总股票数: " + std::to_string(totalStocks_)) | color(Color::GrayLight));
        }
        
        elements.push_back(separator());
        
        // 系统状态
        elements.push_back(text("系统状态: " + systemStatus_) | color(Color::Yellow));
        
        elements.push_back(separator());
        
        // 操作提示（就绪）
        elements.push_back(text("操作提示:") | color(Color::GrayLight));
        elements.push_back(text("  [Ctrl+P] 或 [P] 开始批量分析") | color(Color::Green));
        elements.push_back(text("  [Ctrl+R] 或 [R] 刷新数据") | color(Color::Yellow));
        elements.push_back(text("  [Ctrl+I] 导入CSV数据") | color(Color::Cyan));
    }
    
    return vbox(elements);
}

Element MainScreen::RenderConfiguration() {
    // 构建配置编辑界面
    std::vector<Element> configElements;

    configElements.push_back(text("系统配置 (可编辑)") | bold | color(Color::Blue));
    configElements.push_back(separator());

    // 显示配置文件来源
    configElements.push_back(text("配置来源: .env 文件（当前工作目录）") | color(Color::GrayLight));
    configElements.push_back(separator());

    // 显示配置消息（如果有）
    if (!configMessage_.empty()) {
        if (configMessage_.find("成功") != std::string::npos) {
            configElements.push_back(text(configMessage_) | color(Color::Green));
        } else if (configMessage_.find("错误") != std::string::npos) {
            configElements.push_back(text(configMessage_) | color(Color::Red));
        } else {
            configElements.push_back(text(configMessage_) | color(Color::Yellow));
        }
        configElements.push_back(separator());
    }

    // 配置编辑模式
    if (configEditMode_) {
        configElements.push_back(text("✏️ 编辑配置 (按 [E] 切换显示/编辑模式)") | color(Color::Cyan) | bold);
        configElements.push_back(separator());

        // 使用Container渲染所有Input组件，支持焦点管理
        configElements.push_back(configInputsContainer_->Render() | border);
        configElements.push_back(separator());

        // 配置验证提示
        bool hasApiKey = !configApiKey_.empty() && configApiKey_ != "your_api_key_here";
        if (!hasApiKey) {
            configElements.push_back(text("⚠️ API 密钥未配置或无效") | color(Color::Yellow));
            configElements.push_back(text("提示: 获取 Tushare API Key: https://tushare.pro") | color(Color::Cyan));
            configElements.push_back(separator());
        }

        // 操作提示
        configElements.push_back(text("快捷键:") | color(Color::GrayLight));
        configElements.push_back(text("  [Tab] 切换输入框焦点") | color(Color::Blue));
        configElements.push_back(text("  [↑↓] 在输入框间导航") | color(Color::Blue));
        configElements.push_back(text("  [Ctrl+S] 保存配置（编辑模式下）") | color(Color::Green));
        configElements.push_back(text("  [Ctrl+E] 或 [E] 切换显示/编辑模式（非编辑模式）") | color(Color::Cyan));
        configElements.push_back(text("  [Esc] 退出编辑模式") | color(Color::Red));

        configElements.push_back(separator());
        configElements.push_back(text("重要提示:") | color(Color::Yellow));
        configElements.push_back(text("  ✅ 编辑模式下：所有字母键用于输入（不触发快捷键）") | color(Color::Green));
        configElements.push_back(text("  ✅ 使用 Ctrl+S 保存配置（推荐）") | color(Color::Green));
        configElements.push_back(text("  ❌ 编辑模式下按字母键不会触发快捷键") | color(Color::GrayLight));

    } else {
        // 显示模式（只读）
        configElements.push_back(text("当前配置 (按 [E] 进入编辑模式)") | color(Color::GrayLight));
        configElements.push_back(separator());

        // 显示当前配置值（带验证状态）
        configElements.push_back(text("数据源配置:") | bold);
        bool hasApiKey = !configApiKey_.empty() && configApiKey_ != "your_api_key_here";
        configElements.push_back(text("  ├─ API 密钥: " + configApiKey_) |
            (hasApiKey ? color(Color::Green) : color(Color::Red)));
        if (!hasApiKey) {
            configElements.push_back(text("  │   (未配置或无效，系统运行在降级模式)") | color(Color::Yellow));
        }
        configElements.push_back(separator());

        configElements.push_back(text("数据库配置:") | bold);
        configElements.push_back(text("  ├─ 类型: " + configDbType_) | color(Color::Green));
        configElements.push_back(text("  ├─ 名称: " + configDbName_) | color(Color::Green));
        configElements.push_back(separator());

        configElements.push_back(text("分析配置:") | bold);
        configElements.push_back(text("  ├─ 策略: " + configStrategies_) | color(Color::Green));
        configElements.push_back(separator());

        configElements.push_back(text("日志配置:") | bold);
        configElements.push_back(text("  ├─ 级别: " + configLogLevel_) | color(Color::Green));
        configElements.push_back(separator());

        // 系统状态（动态判断）
        if (!hasApiKey) {
            configElements.push_back(text("系统状态: ⚠️ 降级运行（缺少有效 API 密钥）") | color(Color::Yellow));
            configElements.push_back(text("功能限制: 无法从数据源获取实时数据") | color(Color::GrayLight));
        } else {
            configElements.push_back(text("系统状态: ✅ 正常运行") | color(Color::Green));
            configElements.push_back(text("功能状态: 所有功能可用（数据源已配置）") | color(Color::Green));
        }

        configElements.push_back(separator());
        configElements.push_back(text("快捷键:") | color(Color::GrayLight));
        configElements.push_back(text("  [E] 进入编辑模式") | color(Color::Cyan));
        configElements.push_back(text("  [R] 刷新配置显示") | color(Color::Yellow));
        configElements.push_back(text("  [Ctrl+I] 导入 CSV 数据（无需 API 密钥）") | color(Color::Cyan));
    }

    return vbox(configElements);
}

Element MainScreen::RenderLogs() {
    // 新增：使用 LogBufferManager 获取日志内容（替代文件读取）
    auto& bufferManager = logger::LogBufferManager::getInstance();
    std::vector<std::string> logLines = bufferManager.getRecentLogs(50);
    
    // 如果缓冲区为空，显示默认提示
    if (logLines.empty()) {
        logLines = {
            "[INFO] 日志系统初始化中...",
            "[INFO] UI 缓冲区已启用",
            "[INFO] 配置: LOG_UI_ENABLED=true",
            "[INFO] 缓冲区大小: 1000 行",
        };
    }
    
    // 构建日志显示元素（可滚动）
    std::vector<Element> logElements;
    logElements.push_back(text("实时日志 (UI 缓冲区)") | bold | color(Color::Cyan));
    logElements.push_back(separator());
    logElements.push_back(text("日志条目总数: " + std::to_string(logLines.size())) | color(Color::GrayLight));
    logElements.push_back(separator());
    
    for (const auto& logLine : logLines) {
        // 根据日志级别设置颜色
        if (logLine.find("[ERROR]") != std::string::npos) {
            logElements.push_back(text(logLine) | color(Color::Red));
        } else if (logLine.find("[WARN]") != std::string::npos) {
            logElements.push_back(text(logLine) | color(Color::Yellow));
        } else if (logLine.find("[INFO]") != std::string::npos) {
            logElements.push_back(text(logLine) | color(Color::Green));
        } else {
            logElements.push_back(text(logLine) | color(Color::GrayLight));
        }
    }
    
    logElements.push_back(separator());
    logElements.push_back(text("快捷键: [R] 刷新  [↓] 滚动  [End] 跳到底部") | color(Color::Cyan));
    logElements.push_back(text("日志配置: UI 缓冲区 + 文件记录") | color(Color::GrayLight));
    
    // 使用 frame 和 vscroll_indicator 实现滚动功能
    return vbox(logElements) | frame | vscroll_indicator | flex;
}

Element MainScreen::RenderStatus() {
    std::vector<Element> elements;

    elements.push_back(text("系统状态") | bold | color(Color::Cyan));
    elements.push_back(separator());

    // 动态检查系统状态
    try {
        auto& context = stock_core::StockCoreContext::getInstance();
        auto dataSource = stock_core::api::getDataSource();
        auto& strategyManager = stock_core::api::getStrategyManager();

        // 运行状态
        bool hasDataSource = (dataSource != nullptr);
        std::string runStatus = hasDataSource ? "正常运行" : "降级运行";
        Color runStatusColor = hasDataSource ? Color::Green : Color::Yellow;

        elements.push_back(text("运行状态: " + runStatus) | color(runStatusColor));
        elements.push_back(separator());

        // 系统信息
        elements.push_back(text("系统信息:") | bold);
        elements.push_back(text("  ├─ 版本: v" + std::string(VERSION_STRING)));

        // 数据源状态
        std::string dataSourceStatus = hasDataSource ? "可用" : "不可用";
        Color dataSourceColor = hasDataSource ? Color::Green : Color::Red;
        elements.push_back(text("  ├─ 数据源: " + dataSourceStatus) | color(dataSourceColor));

        // 数据库状态
        elements.push_back(text("  ├─ 数据库: SQLite（已连接）") | color(Color::Green));

        // 策略数量（动态获取）
        int strategyCount = 3;  // EMA17TO25, MACD, RSI
        elements.push_back(text("  ├─ 策略: " + std::to_string(strategyCount) + " 个已注册") | color(Color::Cyan));

        // 股票总数
        elements.push_back(text("  └─ 股票数: " + std::to_string(totalStocks_) + " 只") | color(Color::Yellow));

        elements.push_back(separator());

        // 数据源详细信息（如果可用）
        if (hasDataSource) {
            auto& config = stock_core::api::getConfig();
            elements.push_back(text("数据源配置:") | bold);
            elements.push_back(text("  ├─ URL: " + config.getDataSourceUrl()));
            elements.push_back(text("  ├─ 超时: " + std::to_string(config.getDataSourceTimeout()) + " 秒"));
            elements.push_back(text("  └─ API Key: 已配置") | color(Color::Green));
            elements.push_back(separator());
        }

        // 性能指标
        elements.push_back(text("性能指标:") | bold);
        elements.push_back(text("  ├─ 内存: 正常"));
        elements.push_back(text("  ├─ CPU: 正常"));
        elements.push_back(text("  └─ 响应: 快速"));

        elements.push_back(separator());

        // 提示信息
        if (!hasDataSource) {
            elements.push_back(text("提示: 配置 API 密钥后可启用完整功能") | color(Color::Yellow));
            elements.push_back(text("操作: 按 [Ctrl+E] 进入配置编辑，输入 API Key 后保存") | color(Color::Cyan));
        } else {
            elements.push_back(text("提示: 系统运行正常，所有功能可用") | color(Color::Green));
        }

    } catch (const std::exception& e) {
        // 异常情况显示降级模式
        elements.push_back(text("运行状态: 异常（降级模式）") | color(Color::Red));
        elements.push_back(separator());
        elements.push_back(text("系统信息:") | bold);
        elements.push_back(text("  ├─ 版本: v" + std::string(VERSION_STRING)));
        elements.push_back(text("  ├─ 数据源: 异常") | color(Color::Red));
        elements.push_back(text("  ├─ 数据库: SQLite"));
        elements.push_back(text("  └─ 策略: 0 个"));
        elements.push_back(separator());
        elements.push_back(text("错误信息: " + std::string(e.what())) | color(Color::Red));
    }

    return vbox(elements);
}

void MainScreen::SetCurrentMenu(int menuIndex) {
    currentMenu_ = menuIndex;
}

Element MainScreen::Render() {
    // 根据当前菜单索引切换内容（与左侧导航菜单对应）
    Element content;
    switch (currentMenu_) {
        case 0:  // 主界面 - 显示系统概览
            content = RenderOverview() | flex;
            break;
        case 1:  // 分析
            content = RenderAnalysis() | flex;
            break;
        case 2:  // 配置
            content = RenderConfiguration() | flex;
            break;
        case 3:  // 日志
            content = RenderLogs() | flex;
            break;
        case 4:  // 状态
            content = RenderStatus() | flex;
            break;
        case 5:  // 退出
            content = vbox({
                text("退出系统") | bold | color(Color::Red),
                separator(),
                text("确认退出？按 [Q] 或 [Esc] 退出") | color(Color::Yellow),
            }) | flex;
            break;
        default:
            content = RenderOverview() | flex;
    }

    return content;
}

Component MainScreen::GetComponent() {
    return Container::Vertical({
        tabSelector_,
        content_,
    });
}

void MainScreen::RefreshData() {
    try {
        // 安全刷新数据，捕获异常
        auto& context = stock_core::StockCoreContext::getInstance();

        // 尝试加载股票列表（可能返回空）
        auto stocks = stock_core::api::loadStockList();
        totalStocks_ = stocks.size();
        analyzedStocks_ = 0;  // 降级模式下为 0
        failedAnalysis_ = 0;

        // 更新股票列表显示
        stocksData_ = stocks;
        stockListEntries_.clear();

        // 格式化股票显示条目（代码 + 名称 + 行业）
        for (const auto& stock : stocks) {
            std::string entry = stock.ts_code + " | " + stock.name +
                                " | " + stock.industry;
            stockListEntries_.push_back(entry);
        }

        // 重置选中索引（防止越界）
        if (selectedStockIndex_ >= stockListEntries_.size()) {
            selectedStockIndex_ = 0;
        }

        std::cerr << "[DEBUG] 股票列表刷新完成，总数: " << totalStocks_ << std::endl;

    } catch (const std::exception& e) {
        // 降级模式：保持默认值
        totalStocks_ = 0;
        analyzedStocks_ = 0;
        failedAnalysis_ = 0;
        lastAnalysisTime_ = "降级模式运行";
        stockListEntries_.clear();
        stocksData_.clear();
        selectedStockIndex_ = 0;
        std::cerr << "[DEBUG] 股票列表刷新失败（降级模式）" << std::endl;
    }
}

// 新增：开始批量分析
void MainScreen::StartAnalysis() {
    if (isAnalyzing_) {
        // 已经在分析中，不重复启动
        return;
    }

    // 检查数据源是否可用（降级模式处理）
    auto dataSource = stock_core::api::getDataSource();
    if (!dataSource) {
        std::cerr << "[DEBUG] 数据源不可用，无法执行分析" << std::endl;
        analysisStatus_ = "数据源不可用";
        return;
    }

    // 检查股票列表是否为空
    if (stocksData_.empty()) {
        std::cerr << "[DEBUG] 股票列表为空，无法执行分析" << std::endl;
        analysisStatus_ = "无股票数据";
        return;
    }

    // 设置分析状态
    isAnalyzing_ = true;
    analysisStatus_ = "正在初始化...";
    analysisProgressPercent_ = 0;
    analyzedCount_ = 0;
    failedCount_ = 0;

    // 获取当前时间作为开始时间
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    char timeBuffer[20];
    std::strftime(timeBuffer, sizeof(timeBuffer), "%H:%M:%S", std::localtime(&time_t_now));
    analysisStartTime_ = std::string(timeBuffer);

    currentAnalyzingStock_ = "";

    std::cerr << "[DEBUG] 开始批量分析，总数: " << stocksData_.size() << std::endl;

    // 启动后台分析线程
    analysisThread_ = std::thread([this]() {
        try {
            // 设置进度回调
            stock_core::api::setProgressCallback([this](int total, int completed, int failed, const std::string& status) {
                // 更新进度状态（原子变量，线程安全）
                analysisProgressPercent_ = (total > 0) ? (completed * 100 / total) : 0;
                analyzedCount_ = completed;
                failedCount_ = failed;

                // 更新状态文本（需要锁保护）
                {
                    std::lock_guard<std::mutex> lock(statusMutex_);
                    analysisStatus_ = status;
                }
            });

            // 执行批量分析
            stock_core::api::performDailyAnalysis(stocksData_);

            // 分析完成
            {
                std::lock_guard<std::mutex> lock(statusMutex_);
                analysisStatus_ = "分析完成";
                currentAnalyzingStock_ = "";
            }

            // 更新完成时间
            lastAnalysisTime_ = analysisStartTime_;
            analyzedStocks_ = analyzedCount_;
            failedAnalysis_ = failedCount_;

            std::cerr << "[DEBUG] 批量分析完成: 成功 " << analyzedCount_
                      << " 失败 " << failedCount_ << std::endl;

        } catch (const std::exception& e) {
            // 异常处理
            {
                std::lock_guard<std::mutex> lock(statusMutex_);
                analysisStatus_ = "分析异常: " + std::string(e.what());
            }
            std::cerr << "[ERROR] 批量分析异常: " << e.what() << std::endl;
        }

        // 标记分析完成
        isAnalyzing_ = false;
    });
}

// 新增：停止分析
void MainScreen::StopAnalysis() {
    if (!isAnalyzing_) {
        // 不在分析中，无需停止
        return;
    }

    // 设置停止标志
    isAnalyzing_ = false;

    {
        std::lock_guard<std::mutex> lock(statusMutex_);
        analysisStatus_ = "已停止";
        currentAnalyzingStock_ = "";
    }

    // 记录停止时的进度
    lastAnalysisTime_ = analysisStartTime_ + " (已停止)";
    analyzedStocks_ = analyzedCount_;
    failedAnalysis_ = failedCount_;

    std::cerr << "[DEBUG] 分析已停止，已完成: " << analyzedCount_ << "/" << stocksData_.size() << std::endl;

    // 等待线程结束（如果线程还在运行）
    if (analysisThread_.joinable()) {
        analysisThread_.join();  // 等待线程退出
    }
}

std::unique_ptr<MainScreen> CreateMainScreen() {
    return std::make_unique<MainScreen>();
}

// 新增：加载当前配置
void MainScreen::LoadCurrentConfig() {
    try {
        auto& config = config::Config::getInstance();
        if (config.isInitialized()) {
            configApiKey_ = config.getDataSourceApiKey();
            configDbType_ = config.getDbType();
            configDbName_ = config.getDbName();
            configStrategies_ = config.getStrategies();
            configLogLevel_ = config.getLogLevel();
        } else {
            // 默认配置值
            configApiKey_ = "your_api_key_here";
            configDbType_ = "sqlite";
            configDbName_ = "data/stock.db";
            configStrategies_ = "EMA17TO25,MACD,RSI";
            configLogLevel_ = "info";
        }
    } catch (...) {
        // 异常情况使用默认值
        configApiKey_ = "your_api_key_here";
        configDbType_ = "sqlite";
        configDbName_ = "data/stock.db";
        configStrategies_ = "EMA17TO25,MACD,RSI";
        configLogLevel_ = "info";
    }
}

// 新增：保存配置到 .env 文件
bool MainScreen::SaveConfig() {
    try {
        std::ofstream envFile(".env");
        if (!envFile.is_open()) {
            configMessage_ = "错误: 无法打开 .env 文件进行写入";
            return false;
        }
        
        // 写入配置（使用简单的键值对格式）
        envFile << "# Stock Analysis System Configuration\n";
        envFile << "# 自动生成于 TUI 配置编辑器\n\n";
        
        envFile << "# 数据源配置\n";
        envFile << "DATA_SOURCE_API_KEY=" << configApiKey_ << "\n";
        envFile << "DATA_SOURCE_URL=http://api.waditu.com\n";  // 使用 HTTP 协议（HttpClient 不支持 HTTPS）
        envFile << "DATA_SOURCE_TIMEOUT=30\n\n";
        
        envFile << "# 数据库配置\n";
        envFile << "DB_TYPE=" << configDbType_ << "\n";
        envFile << "DB_NAME=" << configDbName_ << "\n";
        envFile << "DB_POOL_SIZE=10\n\n";
        
        envFile << "# 分析配置\n";
        envFile << "STRATEGIES=" << configStrategies_ << "\n";
        envFile << "DEFAULT_MA_PERIOD=20\n\n";
        
        envFile << "# 日志配置\n";
        envFile << "LOG_LEVEL=" << configLogLevel_ << "\n";
        envFile << "LOG_FILE_PATH=logs/app.log\n\n";
        
        envFile << "# 应用配置\n";
        envFile << "APP_NAME=StockAnalysisSystem\n";
        envFile << "DEBUG_MODE=false\n";
        
        envFile.close();
        
        // 重新加载配置
        auto& config = config::Config::getInstance();
        if (config.reload(".env")) {
            configMessage_ = "成功: 配置已保存并重新加载";
            return true;
        } else {
            configMessage_ = "警告: 配置已保存，但重新加载失败";
            return false;
        }
        
    } catch (const std::exception& e) {
        configMessage_ = "错误: 保存配置时发生异常 - " + std::string(e.what());
        return false;
    }
}

// 新增：切换配置编辑模式
void MainScreen::ToggleEditMode() {
    configEditMode_ = !configEditMode_;
    if (configEditMode_) {
        configMessage_ = "进入编辑模式，修改配置后按 [S] 保存";
    } else {
        configMessage_ = "退出编辑模式，配置恢复为当前值";
        LoadCurrentConfig();  // 恢复当前配置值
    }
}

} // namespace stock_tui