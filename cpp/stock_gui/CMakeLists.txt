# stock_gui/CMakeLists.txt
cmake_minimum_required(VERSION 3.16)

# ========== Qt 图形界面客户端 ==========

# 查找 Qt6 包
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Charts
)

# 设置 Qt 自动生成工具
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# GUI 可执行文件
add_executable(stock_gui
    main.cpp
    ui/MainWindow.cpp
    ui/AnalysisWidget.cpp
    ui/ConfigWidget.cpp
    ui/LogWidget.cpp
    ui/StatusWidget.cpp
    adapters/ProgressAdapter.cpp
    adapters/LogAdapter.cpp
    adapters/StockListModel.cpp
)

# 包含目录
target_include_directories(stock_gui PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/stock_core
    ${CMAKE_BINARY_DIR}  # 添加 build 目录以包含生成的 version.h
)

# 链接库
target_link_libraries(stock_gui PRIVATE
    stock_core
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Charts
)

# GUI 客户端配置完成
message(STATUS "GUI 客户端配置完成:")
message(STATUS "  - 目标名称: stock_gui")
message(STATUS "  - 功能: Qt 图形界面")
message(STATUS "  - 链接库: stock_core + Qt6")