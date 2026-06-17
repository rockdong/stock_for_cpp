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

# 包含目录
target_include_directories(stock_exe PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/stock_core
    ${CMAKE_BINARY_DIR}  # 添加 build 目录以包含生成的 version.h
)

# 安装规则
install(TARGETS stock_exe DESTINATION bin)

message(STATUS "exe 客户端配置完成:")
message(STATUS "  - 目标名称: stock_exe")
message(STATUS "  - 功能: 简单命令行执行")
message(STATUS "  - 链接库: stock_core")