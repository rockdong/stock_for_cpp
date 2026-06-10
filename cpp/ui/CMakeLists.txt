cmake_minimum_required(VERSION 3.16)
project(stock_ui)

# ========== Qt6 配置 ==========
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 查找 Qt6 包
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Charts)

# ========== 构建选项 ==========
option(ENABLE_QT_UI "Build Qt UI executable" ON)

if(NOT ENABLE_QT_UI)
    message(STATUS "Qt UI disabled (ENABLE_QT_UI=OFF)")
    return()
endif()

message(STATUS "Qt6 版本: ${Qt6_VERSION}")
message(STATUS "Qt6 路径: ${Qt6_DIR}")

# ========== ui_lib 静态库（适配层） ==========
qt_add_library(ui_lib STATIC
    src/adapters/StockListModel.cpp
    src/adapters/StockListModel.h
)

target_include_directories(ui_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/adapters
)

target_link_libraries(ui_lib PUBLIC
    Qt6::Core
    Qt6::Widgets
    Qt6::Charts
    # 链接现有核心库（如果存在）
    $<TARGET_NAME_IF_EXISTS:core_lib>
    $<TARGET_NAME_IF_EXISTS:analysis_lib>
    $<TARGET_NAME_IF_EXISTS:data_lib>
)

# ========== stock_ui 可执行文件 ==========
qt_add_executable(stock_ui
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.h
    # 资源文件
    resources/icons.qrc
)

target_link_libraries(stock_ui PRIVATE
    ui_lib
    Qt6::Core
    Qt6::Widgets
    Qt6::Charts
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
        MACOSX_BUNDLE_LONG_VERSION_STRING "1.0.0"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0"
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

message(STATUS "Qt UI 模块配置完成")