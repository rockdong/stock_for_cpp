# stock_core/CMakeLists.txt
cmake_minimum_required(VERSION 3.10)

# ========== 核心库构建配置 ==========

# 定义核心库
add_library(stock_core STATIC
    stock_core.cpp
    StockCoreContext.cpp
)

# 设置包含目录
target_include_directories(stock_core PUBLIC
    # 构建目录 (version.h) - 最优先
    ${CMAKE_BINARY_DIR}
    
    # 当前目录
    ${CMAKE_CURRENT_SOURCE_DIR}
    
    # 各模块目录
    ${CMAKE_SOURCE_DIR}/config
    ${CMAKE_SOURCE_DIR}/core
    ${CMAKE_SOURCE_DIR}/log
    ${CMAKE_SOURCE_DIR}/network
    ${CMAKE_SOURCE_DIR}/data
    ${CMAKE_SOURCE_DIR}/analysis
    ${CMAKE_SOURCE_DIR}/utils
    ${CMAKE_SOURCE_DIR}/util  # CSVParser 头文件目录
    ${CMAKE_SOURCE_DIR}/scheduler
    ${CMAKE_SOURCE_DIR}/eventbus
    
    # 第三方库头文件
    ${CMAKE_SOURCE_DIR}/thirdparty/spdlog/include
    ${CMAKE_SOURCE_DIR}/thirdparty/json/include
    ${CMAKE_SOURCE_DIR}/thirdparty/cpp-httplib
    ${CMAKE_SOURCE_DIR}/thirdparty/dotenv-cpp/include
    ${CMAKE_SOURCE_DIR}/thirdparty/sqlpp11/include
    ${CMAKE_SOURCE_DIR}/thirdparty/ta-lib/include
    ${CMAKE_SOURCE_DIR}/thirdparty/EventBus/include
)

# 链接所有模块库
target_link_libraries(stock_core PUBLIC
    # 内部模块库
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

# 如果 MySQL 可用,链接 MySQL 库
if(HAS_MYSQL)
    target_link_libraries(stock_core PUBLIC sqlpp11::mysql)
    target_compile_definitions(stock_core PUBLIC HAS_MYSQL)
endif()

# 设置编译选项
target_compile_features(stock_core PUBLIC cxx_std_17)

# 安装规则
install(TARGETS stock_core DESTINATION lib)
install(FILES stock_core.h DESTINATION include)

message(STATUS "核心库配置完成:")
message(STATUS "  - 库名称: stock_core")
message(STATUS "  - 库类型: STATIC")
message(STATUS "  - 输出文件: libstock_core.a")