cmake_minimum_required(VERSION 3.16)
project(stock_ui)

# ========== Qt6 配置 ==========
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 查找 Qt6 包
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Charts)

# ========== 核心库依赖配置 ==========
# 设置父项目路径
set(PARENT_PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)

# 预编译的核心库路径（从父项目的 build 目录链接）
set(PARENT_BUILD_DIR ${PARENT_PROJECT_DIR}/build)

# 添加父项目的 include 目录（解决 sqlpp11 等头文件找不到问题）
include_directories(
    ${PARENT_PROJECT_DIR}/thirdparty/sqlpp11/include
    ${PARENT_PROJECT_DIR}/thirdparty/json/include
    ${PARENT_BUILD_DIR}/_deps/date-src/include  # date 库（sqlpp11 依赖）
    ${PARENT_PROJECT_DIR}/core
    ${PARENT_PROJECT_DIR}/data
    ${PARENT_PROJECT_DIR}/data/database
    ${PARENT_PROJECT_DIR}/config
    ${PARENT_PROJECT_DIR}/log
    ${PARENT_PROJECT_DIR}/utils
)

# ========== 构建选项 ==========
option(ENABLE_QT_UI "Build Qt UI executable" ON)

if(NOT ENABLE_QT_UI)
    message(STATUS "Qt UI disabled (ENABLE_QT_UI=OFF)")
    return()
endif()

message(STATUS "Qt6 版本: ${Qt6_VERSION}")
message(STATUS "Qt6 路径: ${Qt6_DIR}")
message(STATUS "父项目路径: ${PARENT_PROJECT_DIR}")
message(STATUS "核心库路径: ${PARENT_BUILD_DIR}")

# ========== ui_lib 静态库（适配层 - 重构后） ==========
qt_add_library(ui_lib STATIC
    # ApplicationManager (界面协调器)
    src/ApplicationManager.cpp
    src/ApplicationManager.h

    # MainWindow (简化版入口窗口)
    src/MainWindow.cpp
    src/MainWindow.h

    # 独立窗口类 (符合单一职责)
    src/windows/TaskWindow.cpp
    src/windows/TaskWindow.h
    src/windows/ConfigWindow.cpp
    src/windows/ConfigWindow.h
    src/windows/QueryWindow.cpp
    src/windows/QueryWindow.h
    src/windows/SyncWindow.cpp
    src/windows/SyncWindow.h
    src/windows/ChartWindow.cpp
    src/windows/ChartWindow.h

    # 适配层组件
    src/adapters/StockListModel.cpp
    src/adapters/StockListModel.h
    src/adapters/ChartDataAdapter.cpp
    src/adapters/ChartDataAdapter.h

    # 图表组件
    src/widgets/CandlestickWidget.cpp
    src/widgets/CandlestickWidget.h
)

target_include_directories(ui_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/adapters
    ${CMAKE_CURRENT_SOURCE_DIR}/src/windows
)

target_link_libraries(ui_lib PUBLIC
    Qt6::Core
    Qt6::Widgets
    Qt6::Charts
    # 链接父项目的核心库（已编译的静态库）
    ${PARENT_BUILD_DIR}/libdata_lib.a
    ${PARENT_BUILD_DIR}/libcore_lib.a
    ${PARENT_BUILD_DIR}/libconfig_lib.a
    ${PARENT_BUILD_DIR}/liblog_lib.a
)

# ========== stock_ui 可执行文件 ==========
qt_add_executable(stock_ui
    src/main.cpp
    # 资源文件
    resources/icons.qrc
)

target_link_libraries(stock_ui PRIVATE
    ui_lib
    Qt6::Core
    Qt6::Widgets
    Qt6::Charts
    # 链接核心库和第三方依赖
    ${PARENT_BUILD_DIR}/libdata_lib.a
    ${PARENT_BUILD_DIR}/libcore_lib.a
    ${PARENT_BUILD_DIR}/libconfig_lib.a
    ${PARENT_BUILD_DIR}/liblog_lib.a
    ${PARENT_BUILD_DIR}/libnetwork_lib.a
    ${PARENT_BUILD_DIR}/libutils_lib.a
    ${PARENT_BUILD_DIR}/thirdparty/spdlog/libspdlog.a
    sqlite3
)

# ========== Windows 特定配置 ==========
if(WIN32)
    # 自动部署 Qt DLL（开发时使用）
    # 生产环境使用 windeployqt
    set_target_properties(stock_ui PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

# ========== macOS 特定配置 ==========
if(APPLE)
    set_target_properties(stock_ui PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_STRING "Stock for C++ Qt UI"
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.stocklens.stock_ui"
        MACOSX_BUNDLE_LONG_VERSION_STRING "2.0.0"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "2.0"
    )
endif()

# ========== 安装规则 ==========
install(TARGETS stock_ui
    BUNDLE DESTINATION . COMPONENT Runtime
    RUNTIME DESTINATION bin COMPONENT Runtime
)

# ========== 部署脚本（可选） ==========
# macOS: macdeployqt stock_ui.app -dmg
# Windows: windeployqt stock_ui.exe

message(STATUS "Qt UI 模块配置完成 (重构后 - 符合单一职责)")