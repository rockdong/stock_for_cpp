# stock_tui/CMakeLists.txt
cmake_minimum_required(VERSION 3.10)

# ========== FTXUI 终端 UI 客户端 ==========

# 使用本地 thirdparty 中的 FTXUI（指定 binary directory）
add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/ftxui ${CMAKE_BINARY_DIR}/ftxui)

# TUI 可执行文件
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
    stock_core
    ftxui::ftxui
)

# 包含目录
target_include_directories(stock_tui PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/stock_core
    ${CMAKE_BINARY_DIR}  # version.h
)

# 安装规则
install(TARGETS stock_tui DESTINATION bin)

message(STATUS "TUI 客户端配置完成:")
message(STATUS "  - 目标名称: stock_tui")
message(STATUS "  - 功能: FTXUI 终端界面")
message(STATUS "  - 链接库: stock_core + ftxui")