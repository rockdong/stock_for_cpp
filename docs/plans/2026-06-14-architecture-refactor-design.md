# Stock for C++ - 架构重构设计文档

**日期**: 2026-06-14  
**版本**: 2.0  
**状态**: 设计阶段  

---

## 📋 背景

当前项目采用单一可执行文件架构,所有模块库静态链接到 `stock_for_cpp` 可执行文件。这导致:

1. **无法复用**: 业务逻辑与 main.cpp 紧耦合,无法在其他客户端(GUI/Web)中使用
2. **部署冗余**: 如果要同时支持 TUI 和 GUI,需要维护两套完整代码
3. **测试困难**: 核心业务逻辑无法独立测试,必须通过 main.cpp 间接验证

---

## 🎯 重构目标

### 目标架构

**统一核心库 + 三种客户端架构**

```
stock_core (核心库)
    ├── stock_tui (FTXUI 终端 UI 客户端)    - 交互式终端图形界面
    ├── stock_gui (Qt GUI 客户端)           - 图形用户界面
    └── stock_exe (命令行客户端)            - 简单命令行执行
```

### 核心特性

1. **单一核心库**: 所有业务逻辑封装在 `libstock_core.a` 中
2. **三种客户端**:
   - **TUI**: 使用 FTXUI 库的现代终端图形界面
   - **GUI**: Qt 图形用户界面
   - **exe**: 简单命令行执行（无 UI）
3. **清晰接口**: 核心库提供统一 API,三种客户端共享同一业务逻辑
4. **独立构建**: 支持单独构建核心库、TUI、GUI、exe
5. **灵活部署**: 根据环境选择合适的客户端（服务器用 exe/TUI，桌面用 GUI）

---

## 🏗️ 架构设计

### 1. 核心库 (stock_core)

#### 文件结构

```
cpp/
├── stock_core/
│   ├── CMakeLists.txt         # 核心库构建配置
│   ├── stock_core.h           # 统一接口头文件
│   ├── stock_core.cpp         # 核心库初始化实现
│   ├── StockCoreContext.h     # 核心上下文(持有所有模块)
│   ├── StockCoreContext.cpp
│   └── api/
│       ├── ConfigApi.h        # 配置 API
│       ├── DatabaseApi.h      # 数据库 API
│       ├── DataSourceApi.h    # 数据源 API
│       ├── StrategyApi.h      # 策略 API
│       ├── SchedulerApi.h     # 调度器 API
│       └── AnalysisApi.h      # 分析 API
```

#### 统一接口

```cpp
// stock_core.h
#pragma once

#include <string>
#include <vector>

namespace stock_core {
    
/**
 * @brief 核心库上下文 - 持有所有模块实例
 */
class StockCoreContext;
    
/**
 * @brief 初始化核心库
 * @param configPath 配置文件路径 (.env)
 * @return 是否成功
 */
bool initialize(const std::string& configPath = ".env");
    
/**
 * @brief 清理核心库资源
 */
void shutdown();
    
/**
 * @brief 获取核心上下文
 * @return 上下文指针(初始化后有效)
 */
StockCoreContext* getContext();
    
/**
 * @brief 业务逻辑 API
 */
namespace api {
    
    // 配置管理
    config::Config& getConfig();
    
    // 数据库访问
    data::Connection& getDatabase();
    
    // 数据源
    std::shared_ptr<network::IDataSource> getDataSource();
    
    // 策略管理器
    core::StrategyManager& getStrategyManager();
    
    // 调度器
    scheduler::Scheduler& getScheduler();
    
    // 业务流程
    std::vector<core::Stock> loadStockList();
    void performDailyAnalysis(const std::vector<core::Stock>& stocks);
    void runScheduler(const std::string& executeTime);
    
} // namespace api
    
} // namespace stock_core
```

#### 核心上下文

```cpp
// StockCoreContext.h
#pragma once

#include <memory>
#include "Config.h"
#include "Connection.h"
#include "DataSourceFactory.h"
#include "StrategyManager.h"
#include "Scheduler.h"

namespace stock_core {

/**
 * @brief 核心库上下文 - 持有所有模块实例
 */
class StockCoreContext {
public:
    StockCoreContext();
    ~StockCoreContext();
    
    // 初始化
    bool initialize(const std::string& configPath);
    void shutdown();
    
    // 获取模块实例
    config::Config& getConfig();
    data::Connection& getDatabase();
    std::shared_ptr<network::IDataSource> getDataSource();
    core::StrategyManager& getStrategyManager();
    scheduler::Scheduler& getScheduler();
    
    // 业务流程
    std::vector<core::Stock> loadStockList();
    void performDailyAnalysis(const std::vector<core::Stock>& stocks);
    
private:
    // 模块实例
    std::unique_ptr<config::Config> config_;
    std::unique_ptr<data::Connection> database_;
    std::shared_ptr<network::IDataSource> dataSource_;
    std::unique_ptr<core::StrategyManager> strategyManager_;
    std::unique_ptr<scheduler::Scheduler> scheduler_;
    
    // DAO 实例
    std::unique_ptr<data::StockDAO> stockDao_;
    std::unique_ptr<data::AnalysisResultDAO> analysisResultDao_;
    std::unique_ptr<data::ChartDataDAO> chartDataDao_;
    std::unique_ptr<data::AnalysisProcessRecordDAO> processRecordDao_;
    
    bool initialized_;
};

} // namespace stock_core
```

#### CMakeLists.txt

```cmake
# stock_core/CMakeLists.txt
cmake_minimum_required(VERSION 3.10)

# ========== 核心库构建配置 ==========

# 定义核心库
add_library(stock_core STATIC
    stock_core.cpp
    StockCoreContext.cpp
    api/ConfigApi.cpp
    api/DatabaseApi.cpp
    api/DataSourceApi.cpp
    api/StrategyApi.cpp
    api/SchedulerApi.cpp
    api/AnalysisApi.cpp
)

# 链接所有模块库
target_link_libraries(stock_core PUBLIC
    # 内部模块
    config_lib
    scheduler_lib
    core_lib
    log_lib
    network_lib
    data_lib
    analysis_lib
    utils_lib
    eventbus_lib
    
    # 第三方库
    spdlog::spdlog
    nlohmann_json::nlohmann_json
    httplib::httplib
    dotenv
    sqlpp11::sqlpp11
    sqlite3
    ta-lib-static
)

# 包含头文件目录
target_include_directories(stock_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/config
    ${CMAKE_SOURCE_DIR}/core
    ${CMAKE_SOURCE_DIR}/log
    ${CMAKE_SOURCE_DIR}/network
    ${CMAKE_SOURCE_DIR}/data
    ${CMAKE_SOURCE_DIR}/analysis
    ${CMAKE_SOURCE_DIR}/utils
    ${CMAKE_SOURCE_DIR}/scheduler
    ${CMAKE_SOURCE_DIR}/eventbus
)

# 导出头文件
install(FILES stock_core.h DESTINATION include)
install(TARGETS stock_core DESTINATION lib)
```

---

### 2. TUI 客户端 (stock_tui) - FTXUI 终端图形界面

#### 为什么选择 FTXUI?

**FTXUI** 是现代 C++ 终端 UI 库,具有以下优势:
- ✅ 纯 C++ 实现,无依赖
- ✅ 响应式布局系统（类似 React）
- ✅ 支持组件化开发
- ✅ 跨平台（Linux/macOS/Windows）
- ✅ 支持键盘导航、鼠标交互
- ✅ 现代化样式（颜色、边框、阴影）

对比传统 ncurses:
- FTXUI 更现代,C++17 支持
- 组件化设计,易于维护
- 不需要学习复杂的 ncurses API

#### 文件结构

```
cpp/
├── stock_tui/
│   ├── CMakeLists.txt         # TUI 构建配置
│   ├── main.cpp               # FTXUI 主入口
│   ├── components/
│   │   ├── MainScreen.h       # 主界面组件
│   │   ├── MainScreen.cpp
│   │   ├── AnalysisScreen.h   # 分析界面
│   │   ├── AnalysisScreen.cpp
│   │   ├── ConfigScreen.h     # 配置界面
│   │   ├── ConfigScreen.cpp
│   │   ├── LogScreen.h        # 日志查看界面
│   │   ├── LogScreen.cpp
│   │   ├── StatusScreen.h     # 状态监控界面
│   │   ├── StatusScreen.cpp
│   │   └── MenuComponent.h    # 导航菜单
│   │   └── MenuComponent.cpp
│   └── adapters/
│       ├── ProgressAdapter.h  # 进度适配器
│       ├── ProgressAdapter.cpp
│       ├── LogAdapter.h       # 日志适配器
│       └── LogAdapter.cpp
│   └── thirdparty/
│       └── ftxui/             # FTXUI 库
│           ├── CMakeLists.txt
│           └── include/
```

#### FTXUI 主界面示例

```cpp
// stock_tui/main.cpp
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include "stock_core.h"
#include "components/MainScreen.h"
#include "components/AnalysisScreen.h"
#include "components/ConfigScreen.h"
#include "components/LogScreen.h"

using namespace ftxui;

int main(int argc, char* argv[]) {
    try {
        // 1. 初始化核心库
        if (!stock_core::initialize(".env")) {
            std::cerr << "核心库初始化失败" << std::endl;
            return 1;
        }
        
        // 2. 创建终端界面
        auto screen = ScreenInteractive::TerminalOutput();
        
        // 3. 创建菜单导航
        int selected_tab = 0;
        std::vector<std::string> tab_titles = {
            "分析", "配置", "日志", "状态"
        };
        
        auto tab_menu = Menu(&tab_titles, &selected_tab, MenuOption::Horizontal());
        
        // 4. 创建各界面组件
        auto analysis_screen = AnalysisScreen::create();
        auto config_screen = ConfigScreen::create();
        auto log_screen = LogScreen::create();
        auto status_screen = StatusScreen::create();
        
        // 5. 组合界面
        auto content_container = Container::Vertical({
            analysis_screen,
            config_screen,
            log_screen,
            status_screen
        });
        
        // 6. 主布局
        auto main_layout = Renderer(content_container, [&](Element content) {
            return vbox({
                // 标题栏
                hbox({
                    text("Stock for C++ v2.0"),
                    separator(),
                    gauge(0.5) | color(Color::Green),
                    separator(),
                    text("运行状态: 就绪")
                }) | border,
                
                // 菜单栏
                tab_menu->Render() | border,
                
                // 内容区
                content | border | flex,
                
                // 状态栏
                hbox({
                    text("核心库: 已连接"),
                    separator(),
                    text("策略数: 5"),
                    separator(),
                    text("数据库: SQLite"),
                    separator(),
                    text("按 [Esc] 退出")
                }) | border
            });
        });
        
        // 7. 运行循环
        screen.Loop(main_layout);
        
        // 8. 清理
        stock_core::shutdown();
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "程序异常: " << e.what() << std::endl;
        stock_core::shutdown();
        return 1;
    }
}
```

#### 分析界面组件

```cpp
// components/AnalysisScreen.cpp
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include "stock_core.h"

using namespace ftxui;

Component AnalysisScreen::create() {
    // 状态变量
    static std::string execute_time = "20:00";
    static bool running = false;
    static int progress = 0;
    static int total_stocks = 0;
    static int completed = 0;
    static int failed = 0;
    
    // 输入组件
    auto time_input = Input(&execute_time, "执行时间 (HH:MM)");
    
    // 开始按钮
    auto start_button = Button("开始分析", [&]{
        running = true;
        auto stocks = stock_core::api::loadStockList();
        total_stocks = stocks.size();
        
        // 执行分析
        stock_core::api::performDailyAnalysis(stocks);
        running = false;
    });
    
    // 定时模式按钮
    auto schedule_button = Button("定时模式", [&]{
        stock_core::api::runScheduler(execute_time);
    });
    
    // 停止按钮
    auto stop_button = Button("停止", [&]{
        running = false;
        // TODO: 实现停止逻辑
    }, ButtonOption::Animated(Color::Red));
    
    // 布局
    return Renderer(Container::Vertical({
        time_input,
        start_button,
        schedule_button,
        stop_button
    }), [&]{
        return vbox({
            // 标题
            text("分析配置") | bold | color(Color::Blue),
            separator(),
            
            // 时间输入
            hbox({
                text("执行时间: "),
                time_input->Render()
            }),
            
            // 进度显示
            vbox({
                text("进度统计"),
                hbox({
                    text("总数: "),
                    text(std::to_string(total_stocks)),
                    separator(),
                    text("完成: "),
                    text(std::to_string(completed)) | color(Color::Green),
                    separator(),
                    text("失败: "),
                    text(std::to_string(failed)) | color(Color::Red)
                }),
                gauge(progress / 100.0) | color(Color::Green)
            }) | border,
            
            // 控制按钮
            hbox({
                start_button->Render(),
                separator(),
                schedule_button->Render(),
                separator(),
                stop_button->Render()
            }) | border,
            
            // 日志显示区域
            vbox({
                text("执行日志"),
                text("等待开始...")
            }) | border | flex
        });
    });
}
```

#### CMakeLists.txt

```cmake
# stock_tui/CMakeLists.txt
cmake_minimum_required(VERSION 3.10)

# ========== FTXUI 配置 ==========

# 引入 FTXUI 库 (作为子模块)
add_subdirectory(thirdparty/ftxui)

# ========== TUI 可执行文件 ==========

add_executable(stock_tui
    main.cpp
    components/MainScreen.cpp
    components/AnalysisScreen.cpp
    components/ConfigScreen.cpp
    components/LogScreen.cpp
    components/StatusScreen.cpp
    components/MenuComponent.cpp
    adapters/ProgressAdapter.cpp
    adapters/LogAdapter.cpp
)

# 链接库
target_link_libraries(stock_tui PRIVATE
    stock_core      # 核心业务库
    ftxui::component  # FTXUI 组件库
    ftxui::dom        # FTXUI DOM 库
)

# 包含目录
target_include_directories(stock_tui PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/components
    ${CMAKE_CURRENT_SOURCE_DIR}/adapters
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/ftxui/include
)

# 安装规则
install(TARGETS stock_tui DESTINATION bin)
```

---

### 3. exe 客户端 (stock_exe) - 简单命令行执行

#### 文件结构

```
cpp/
├── stock_exe/
│   ├── CMakeLists.txt         # exe 构建配置
│   ├── main.cpp               # 命令行入口
│   ├── CliOptions.h           # 命令行参数解析
│   ├── CliOptions.cpp
│   ├── CliProgress.h          # 终端进度显示
│   ├── CliProgress.cpp
│   └── SignalHandler.h        # 信号处理
│   └── SignalHandler.cpp
```

#### 主入口实现

```cpp
// stock_exe/main.cpp
#include "stock_core.h"
#include "CliOptions.h"
#include "CliProgress.h"
#include "SignalHandler.h"

int main(int argc, char* argv[]) {
    try {
        // 1. 解析命令行参数
        auto options = parseCliOptions(argc, argv);
        
        if (options.help) {
            printHelp(argv[0]);
            return 0;
        }
        
        // 2. 注册信号处理
        SignalHandler::registerHandlers();
        
        // 3. 初始化核心库
        if (!stock_core::initialize(".env")) {
            std::cerr << "核心库初始化失败" << std::endl;
            return 1;
        }
        
        printVersion();
        
        // 4. 加载股票列表
        auto stocks = stock_core::api::loadStockList();
        
        // 5. 设置进度回调
        CliProgress progress;
        stock_core::api::setProgressCallback(progress.getCallback());
        
        // 6. 执行分析
        if (options.onceMode) {
            stock_core::api::performDailyAnalysis(stocks);
        } else {
            stock_core::api::runScheduler(options.executeTime);
        }
        
        // 7. 清理
        stock_core::shutdown();
        
        std::cout << "应用程序正常退出" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "程序异常: " << e.what() << std::endl;
        stock_core::shutdown();
        return 1;
    }
}
```

#### CMakeLists.txt

```cmake
# stock_exe/CMakeLists.txt
cmake_minimum_required(VERSION 3.10)

# ========== exe 可执行文件 ==========

add_executable(stock_exe
    main.cpp
    CliOptions.cpp
    CliProgress.cpp
    SignalHandler.cpp
)

# 链接核心库
target_link_libraries(stock_exe PRIVATE
    stock_core
)

# 安装规则
install(TARGETS stock_exe DESTINATION bin)
```

---

### 4. GUI 客户端 (stock_gui)

#### 文件结构

```
cpp/
├── stock_gui/
│   ├── CMakeLists.txt         # GUI 构建配置
│   ├── main.cpp               # Qt 入口
│   ├── ui_lib/                # UI 组件库
│   │   ├── src/
│   │   │   ├── ApplicationManager.cpp
│   │   │   ├── MainWindow.cpp
│   │   │   ├── windows/
│   │   │   └── adapters/
│   │   └── CMakeLists.txt
│   └── resources/
│       └── icons.qrc
```

#### Qt 入口

```cpp
// stock_gui/main.cpp
#include <QApplication>
#include "stock_core.h"
#include "ApplicationManager.h"

int main(int argc, char *argv[]) {
    // 1. 初始化核心库
    if (!stock_core::initialize(".env")) {
        std::cerr << "核心库初始化失败" << std::endl;
        return 1;
    }
    
    // 2. 创建 Qt 应用
    QApplication app(argc, argv);
    
    // 3. 设置应用信息
    app.setApplicationName("Stock for C++");
    app.setApplicationVersion("2.0.0");
    
    // 4. 创建主窗口
    ApplicationManager manager;
    manager.show();
    
    // 5. 运行 Qt 事件循环
    int result = app.exec();
    
    // 6. 清理
    stock_core::shutdown();
    
    return result;
}
```

#### CMakeLists.txt

```cmake
# stock_gui/CMakeLists.txt
cmake_minimum_required(VERSION 3.16)

# ========== Qt6 配置 ==========

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Charts)

# ========== ui_lib 组件库 ==========

qt_add_library(ui_lib STATIC
    src/ApplicationManager.cpp
    src/ApplicationManager.h
    src/MainWindow.cpp
    src/MainWindow.h
    src/windows/TaskWindow.cpp
    src/windows/ConfigWindow.cpp
    src/windows/QueryWindow.cpp
    src/windows/SyncWindow.cpp
    src/windows/ChartWindow.cpp
    src/adapters/StockListModel.cpp
    src/adapters/ChartDataAdapter.cpp
    src/widgets/CandlestickWidget.cpp
)

target_link_libraries(ui_lib PUBLIC
    Qt6::Core
    Qt6::Widgets
    Qt6::Charts
    stock_core  # 链接核心库
)

# ========== stock_gui 可执行文件 ==========

qt_add_executable(stock_gui
    main.cpp
    resources/icons.qrc
)

target_link_libraries(stock_gui PRIVATE
    ui_lib
    Qt6::Core
    Qt6::Widgets
    Qt6::Charts
    stock_core  # 链接核心库
)

# ========== 平台特定配置 ==========

if(WIN32)
    set_target_properties(stock_gui PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

if(APPLE)
    set_target_properties(stock_gui PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.stocklens.stock_gui"
    )
endif()

install(TARGETS stock_gui DESTINATION bin)
```

---

## 🔧 主 CMakeLists.txt 结构

```cmake
# cpp/CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(stock_for_cpp VERSION 2.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ========== 构建选项 ==========

option(BUILD_CORE "构建核心库" ON)
option(BUILD_TUI "构建 TUI 客户端 (FTXUI)" OFF)
option(BUILD_GUI "构建 GUI 客户端 (Qt)" OFF)
option(BUILD_EXE "构建 exe 客户端 (命令行)" ON)
option(BUILD_TESTS "构建测试" OFF)

# 默认: 如果没有指定任何客户端,构建 exe
if(NOT BUILD_TUI AND NOT BUILD_GUI AND NOT BUILD_EXE)
    set(BUILD_EXE ON)
endif()

# ========== 版本信息 ==========

file(READ "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" VERSION_STRING)
string(STRIP "${VERSION_STRING}" VERSION_STRING)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/version.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/version.h"
    @ONLY
)

# ========== 第三方库 ==========

add_subdirectory(thirdparty/spdlog)
add_subdirectory(thirdparty/json)
add_subdirectory(thirdparty/cpp-httplib)
add_subdirectory(thirdparty/dotenv-cpp)
add_subdirectory(thirdparty/sqlpp11)
add_subdirectory(thirdparty/ta-lib)

# ========== 内部模块库 ==========

add_library(config_lib STATIC config/Config.cpp)
add_library(log_lib STATIC log/LogConfig.cpp log/LoggerFactory.cpp)
add_library(network_lib STATIC network/http/HttpClient.cpp)
add_library(data_lib STATIC data/Data.cpp data/database/Connection.cpp)
add_library(analysis_lib STATIC analysis/IndicatorBase.cpp)
add_library(utils_lib STATIC utils/TimeUtil.cpp)
add_library(core_lib STATIC core/Strategy.cpp core/StrategyManager.cpp)
add_library(scheduler_lib STATIC scheduler/Scheduler.cpp)
add_library(eventbus_lib STATIC eventbus/EventBusManager.cpp)

# ========== 核心库 ==========

if(BUILD_CORE)
    add_subdirectory(stock_core)
endif()

# ========== 客户端 ==========

if(BUILD_TUI)
    message(STATUS "构建 TUI 客户端 (FTXUI 终端界面)")
    add_subdirectory(stock_tui)
endif()

if(BUILD_GUI)
    message(STATUS "构建 GUI 客户端 (Qt 图形界面)")
    add_subdirectory(stock_gui)
endif()

if(BUILD_EXE)
    message(STATUS "构建 exe 客户端 (命令行)")
    add_subdirectory(stock_exe)
endif()

# ========== 测试 ==========

if(BUILD_TESTS)
    add_subdirectory(tests)
endif()

# ========== 构建信息 ==========

message(STATUS "========================================")
message(STATUS "Stock for C++ v${VERSION_STRING}")
message(STATUS "========================================")
message(STATUS "构建目标:")
message(STATUS "  核心库: ${BUILD_CORE}")
message(STATUS "  TUI 客户端: ${BUILD_TUI}")
message(STATUS "  GUI 客户端: ${BUILD_GUI}")
message(STATUS "  exe 客户端: ${BUILD_EXE}")
message(STATUS "========================================")
```

---

## 📊 构建方式

### 1. 只构建核心库

```bash
cmake -DBUILD_CORE=ON -DBUILD_TUI=OFF -DBUILD_GUI=OFF -DBUILD_EXE=OFF ..
make
```

输出: `libstock_core.a`

### 2. 构建核心库 + exe (默认)

```bash
cmake -DBUILD_CORE=ON -DBUILD_EXE=ON ..
make
```

输出: `libstock_core.a`, `stock_exe`

### 3. 构建核心库 + TUI (FTXUI)

```bash
cmake -DBUILD_CORE=ON -DBUILD_TUI=ON ..
make
```

输出: `libstock_core.a`, `stock_tui`

### 4. 构建核心库 + GUI (Qt)

```bash
cmake -DBUILD_CORE=ON -DBUILD_GUI=ON ..
make
```

输出: `libstock_core.a`, `stock_gui`

### 5. 构建全部

```bash
cmake -DBUILD_CORE=ON -DBUILD_TUI=ON -DBUILD_GUI=ON -DBUILD_EXE=ON ..
make
```

输出: `libstock_core.a`, `stock_tui`, `stock_gui`, `stock_exe`

### 6. 快捷构建脚本

```bash
# scripts/build.sh

# 构建 exe
./scripts/build_exe.sh

# 构建 TUI
./scripts/build_tui.sh

# 构建 GUI
./scripts/build_gui.sh

# 构建全部
./scripts/build_all.sh
```

---

## ✅ 架构优势

| 特性 | 当前架构 | 新架构 |
|------|---------|--------|
| 业务逻辑复用 | ❌ 紧耦合在 main.cpp | ✅ 统一核心库 |
| 多客户端支持 | ❌ 需要复制代码 | ✅ 三种客户端共享核心库 |
| 灵活部署 | ❌ 单一可执行文件 | ✅ 根据环境选择客户端 |
| 服务器部署 | ❌ 无专用版本 | ✅ exe/TUI 适合服务器 |
| 桌面使用 | ❌ 无 GUI | ✅ GUI 适合桌面用户 |
| 终端交互 | ❌ 纯命令行 | ✅ TUI 提供图形界面 |
| 易于测试 | ❌ 必须通过 main.cpp | ✅ 核心库独立测试 |
| 易于扩展 | ❌ 添加客户端需重构 | ✅ 直接链接核心库 |
| 代码维护 | ❌ 多处重复代码 | ✅ 单一维护点 |

---

## 🚀 下一步计划

### Phase 1: 核心库实现 (已完成框架)
1. ✅ 创建 stock_core 目录结构
2. ✅ 实现 StockCoreContext
3. ⏳ 完善 performDailyAnalysis 业务逻辑
4. ⏳ 从 main.cpp 迁移现有代码

### Phase 2: exe 客户端 (优先级最高)
1. ⏳ 创建 stock_exe 目录
2. ⏳ 实现命令行参数解析
3. ⏳ 实现进度显示
4. ⏳ 从现有 main.cpp 迁移代码

### Phase 3: TUI 客户端 (优先级中等)
1. ⏳ 集成 FTXUI 库
2. ⏳ 实现主界面组件
3. ⏳ 实现分析界面
4. ⏳ 实现配置界面
5. ⏳ 实现日志查看界面

### Phase 4: GUI 客户端 (优先级低)
1. ⏳ 重构现有 UI 代码
2. ⏳ 调整到新架构
3. ⏳ 测试 Qt 功能

### Phase 5: 构建系统
1. ⏳ 更新主 CMakeLists.txt
2. ⏳ 创建构建脚本
3. ⏳ 测试三种客户端构建

### Phase 6: 文档和测试
1. ⏳ 更新 README
2. ⏳ 编写使用文档
3. ⏳ 测试验证

---

**文档版本**: 2.0  
**维护者**: Senior Developer Team  
**更新时间**: 2026-06-14