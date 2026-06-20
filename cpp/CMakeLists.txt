cmake_minimum_required(VERSION 3.10)
project(stock_for_cpp VERSION 2.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ========== 构建选项 ==========

option(BUILD_CORE "构建核心库" ON)
option(BUILD_TUI "构建 TUI 客户端 (FTXUI终端界面)" ON)
option(BUILD_GUI "构建 GUI 客户端 (Qt图形界面)" ON)
option(BUILD_EXE "构建 exe 客户端 (命令行执行)" ON)
option(BUILD_TESTS "构建测试" OFF)

# 默认: 如果没有指定任何客户端,构建 exe
if(NOT BUILD_TUI AND NOT BUILD_GUI AND NOT BUILD_EXE)
    set(BUILD_EXE ON)
endif()

# ========== 版本信息 ==========
# 读取 VERSION 文件
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" VERSION_STRING)
string(STRIP "${VERSION_STRING}" VERSION_STRING)
string(REPLACE "." ";" VERSION_LIST ${VERSION_STRING})
list(GET VERSION_LIST 0 VERSION_MAJOR)
list(GET VERSION_LIST 1 VERSION_MINOR)
list(GET VERSION_LIST 2 VERSION_PATCH)

# 构建号（从环境变量获取，默认为 0）
if(DEFINED ENV{BUILD_NUMBER})
    set(VERSION_BUILD $ENV{BUILD_NUMBER})
else()
    set(VERSION_BUILD 0)
endif()

# 构建日期
string(TIMESTAMP BUILD_DATE "%Y-%m-%d %H:%M:%S")

# Git 提交哈希（如果可用）
execute_process(
    COMMAND git rev-parse --short HEAD
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)
if(NOT GIT_COMMIT)
    set(GIT_COMMIT "unknown")
endif()

# 生成版本头文件
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/version.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/version.h"
    @ONLY
)

message(STATUS "版本信息: ${VERSION_STRING}.${VERSION_BUILD}")
message(STATUS "构建日期: ${BUILD_DATE}")
message(STATUS "Git 提交: ${GIT_COMMIT}")

# ========== 禁用 cpp-httplib 的 SSL 支持 ==========
# 必须在 add_subdirectory(thirdparty/cpp-httplib) 之前设置
set(HTTPLIB_USE_OPENSSL_IF_AVAILABLE OFF CACHE BOOL "Disable OpenSSL support" FORCE)
set(HTTPLIB_REQUIRE_OPENSSL OFF CACHE BOOL "Do not require OpenSSL" FORCE)
set(HTTPLIB_USE_ZLIB_IF_AVAILABLE OFF CACHE BOOL "Disable ZLIB support" FORCE)
set(HTTPLIB_USE_BROTLI_IF_AVAILABLE OFF CACHE BOOL "Disable Brotli support" FORCE)
set(HTTPLIB_USE_ZSTD_IF_AVAILABLE OFF CACHE BOOL "Disable ZSTD support" FORCE)

# 强制禁用 OpenSSL（防止自动检测）
set(HTTPLIB_IS_USING_OPENSSL OFF CACHE BOOL "Force disable OpenSSL" FORCE)

# 添加第三方库子目录
add_subdirectory(thirdparty/spdlog)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/thirdparty/sqlpp11/cmake/modules")
find_package(MySQL QUIET)

if(MySQL_FOUND)
    set(BUILD_MYSQL_CONNECTOR ON CACHE BOOL "Enable MySQL connector")
else()
    set(BUILD_MYSQL_CONNECTOR OFF CACHE BOOL "Disable MySQL connector (not found)")
endif()

add_subdirectory(thirdparty/sqlpp11)

if(TARGET sqlpp11::mysql)
    set(HAS_MYSQL TRUE)
    message(STATUS "MySQL connector 已启用")
else()
    set(HAS_MYSQL FALSE)
    message(STATUS "MySQL connector 未启用（MySQL 库未找到）")
endif()

add_subdirectory(thirdparty/json)
add_subdirectory(thirdparty/cpp-httplib)
add_subdirectory(thirdparty/dotenv-cpp)

# TA-Lib 通常需要特殊处理，因为它是 C 库
# 如果 thirdparty/ta-lib 有 CMakeLists.txt，可以取消下面的注释
add_subdirectory(thirdparty/ta-lib)

# EventBus (header-only library)
add_library(EventBus INTERFACE)
target_include_directories(EventBus INTERFACE
    ${CMAKE_SOURCE_DIR}/thirdparty/EventBus/include
)

# 添加配置库
add_library(config_lib STATIC
    config/Config.cpp
)

target_include_directories(config_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/config
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/dotenv-cpp/include
)

target_link_libraries(config_lib PUBLIC
    dotenv
)

# 添加调度器模块
add_library(scheduler_lib STATIC
    scheduler/Scheduler.cpp
    scheduler/TradeCalendar.cpp
)

target_include_directories(scheduler_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/scheduler
    ${CMAKE_CURRENT_SOURCE_DIR}/log
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/eventbus
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/EventBus/include
)

target_link_libraries(scheduler_lib PUBLIC
    log_lib
    utils_lib
    core_lib
    eventbus_lib
)

# 添加核心业务模块库（策略系统）
add_library(core_lib STATIC
    core/AnalysisResult.cpp
    core/Strategy.cpp
    core/StrategyFactory.cpp
    core/StrategyManager.cpp
    core/StrategyEmojiMapper.cpp
    core/FundamentalFilter.cpp
    core/strategies/MACrossStrategy.cpp
    core/strategies/MACDStrategy.cpp
    core/strategies/RSIStrategy.cpp
    core/strategies/BOLLStrategy.cpp
    core/strategies/GridStrategy.cpp
    core/strategies/EMA17TO25Strategy.cpp
    core/strategies/EMA17BreakoutStrategy.cpp
    core/strategies/EMAConvergenceStrategy.cpp
    core/strategies/EMA25Greater17PriceMatchStrategy.cpp
    core/strategies/EMA25CrossoverStrategy.cpp
    core/strategies/SurgeSignalStrategy.cpp
    core/strategies/FundamentalStrategy.cpp
)

target_include_directories(core_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/core/strategies
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/analysis
    ${CMAKE_CURRENT_SOURCE_DIR}/analysis/indicators
    ${CMAKE_CURRENT_SOURCE_DIR}/network
    ${CMAKE_CURRENT_SOURCE_DIR}/network/http
    ${CMAKE_CURRENT_SOURCE_DIR}/config
    ${CMAKE_CURRENT_SOURCE_DIR}/log
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/ta-lib/include
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/json/include
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/cpp-httplib
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/dotenv-cpp/include
)

target_link_libraries(core_lib PUBLIC
    utils_lib
    analysis_lib
    network_lib
    log_lib
    config_lib
    nlohmann_json::nlohmann_json
    httplib::httplib
    dotenv
)

# 添加日志库
add_library(log_lib STATIC
    log/LogConfig.cpp
    log/LoggerFactory.cpp
    log/LoggerManager.cpp
    log/SpdlogLogger.cpp
    log/UILogSink.cpp  # 新增：UI sink 实现
)

target_include_directories(log_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/log
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/spdlog/include
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/dotenv-cpp/include
)

target_link_libraries(log_lib PUBLIC
    spdlog::spdlog
    dotenv
)

# 添加网络库
add_library(network_lib STATIC
    network/http/HttpClient.cpp
    network/http/TushareClient.cpp
    network/TushareDataSource.cpp
    network/DataSourceFactory.cpp
)

# 注意：不要定义 CPPHTTPLIB_OPENSSL_SUPPORT
# httplib.h 使用 #ifdef 检查，定义为 0 也会被认为启用了
# 我们通过 HTTPLIB_USE_OPENSSL_IF_AVAILABLE=OFF 来禁用 SSL

target_include_directories(network_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/network
    ${CMAKE_CURRENT_SOURCE_DIR}/network/http
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/log
    ${CMAKE_CURRENT_SOURCE_DIR}/config
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/cpp-httplib
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/json/include
)

target_link_libraries(network_lib PUBLIC
    httplib::httplib
    nlohmann_json::nlohmann_json
    core_lib
    log_lib
    config_lib
)

# 添加数据层库
set(DATA_LIB_SOURCES
    data/Data.cpp
    data/database/Connection.cpp
    data/database/ConnectionFactory.cpp
    data/database/ConnectionManager.cpp
    data/database/SQLiteAdapter.cpp
    data/database/AdapterFactory.cpp
    data/database/StockDAO.cpp
    data/database/PriceDAO.cpp
    data/database/AnalysisResultDAO.cpp
    data/database/ChartDataDAO.cpp
data/database/MoneyFlowDAO.cpp
    data/database/AnalysisProcessRecord.cpp
    data/database/AnalysisProcessRecordDAO.cpp
    data/cache/MemoryCache.cpp
    data/file/CSVReader.cpp
    data/file/CSVWriter.cpp
)

if(HAS_MYSQL)
    list(APPEND DATA_LIB_SOURCES data/database/MySQLConnection.cpp)
    list(APPEND DATA_LIB_SOURCES data/database/MySQLAdapter.cpp)
endif()

add_library(data_lib STATIC ${DATA_LIB_SOURCES})

# 如果 MySQL 可用，定义 HAS_MYSQL 编译宏
if(HAS_MYSQL)
    target_compile_definitions(data_lib PUBLIC HAS_MYSQL)
endif()

target_include_directories(data_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/data
    ${CMAKE_CURRENT_SOURCE_DIR}/data/database
    ${CMAKE_CURRENT_SOURCE_DIR}/data/cache
    ${CMAKE_CURRENT_SOURCE_DIR}/data/file
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/log
    ${CMAKE_CURRENT_SOURCE_DIR}/config
    ${CMAKE_CURRENT_SOURCE_DIR}/network
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/sqlpp11/include
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/json/include
)

target_link_libraries(data_lib PUBLIC
    sqlpp11::sqlpp11
    sqlite3
    nlohmann_json::nlohmann_json
    core_lib
    log_lib
    config_lib
)

# 如果 MySQL 可用，链接 MySQL 库
if(HAS_MYSQL)
    target_link_libraries(data_lib PUBLIC sqlpp11::mysql)
endif()

# 添加分析层库
add_library(analysis_lib STATIC
    analysis/IndicatorBase.cpp
    analysis/indicators/MA.cpp
    analysis/indicators/EMA.cpp
    analysis/indicators/MACD.cpp
    analysis/indicators/RSI.cpp
    analysis/indicators/KDJ.cpp
    analysis/indicators/BOLL.cpp
    analysis/indicators/ATR.cpp
    analysis/IndicatorFactory.cpp
)

target_include_directories(analysis_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/analysis
    ${CMAKE_CURRENT_SOURCE_DIR}/analysis/indicators
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/ta-lib/include
)

target_link_libraries(analysis_lib PUBLIC
    ta-lib-static
)

add_library(utils_lib STATIC
    utils/TimeUtil.cpp
    utils/StringUtil.cpp
    utils/MathUtil.cpp
    utils/KLineAggregator.cpp
    utils/ThreadPool.cpp
    utils/RateLimiter.cpp
    util/CSVParser.cpp  # CSV 解析器（位于 util 目录）
)

# 添加 util 目录的包含路径
target_include_directories(utils_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/util  # CSVParser 头文件目录
)

add_library(eventbus_lib STATIC
    eventbus/EventBusManager.cpp
    eventbus/EventValidator.cpp
)

target_include_directories(eventbus_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/eventbus
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/EventBus/include
)

target_link_libraries(eventbus_lib PUBLIC
    EventBus
    log_lib
)

# ========== 核心库 ==========

if(BUILD_CORE)
    message(STATUS "构建核心库 (stock_core)")
    add_subdirectory(stock_core)
else()
    message(STATUS "跳过核心库构建")
endif()

# ========== 客户端 ==========

# TUI 客户端 (FTXUI 终端界面)
if(BUILD_TUI)
    message(STATUS "构建 TUI 客户端 (FTXUI 终端界面)")
    add_subdirectory(stock_tui)
endif()

# GUI 客户端 (Qt 图形界面)
if(BUILD_GUI)
    message(STATUS "构建 GUI 客户端 (Qt 图形界面)")
    add_subdirectory(stock_gui)
endif()

# exe 客户端 (命令行执行)
if(BUILD_EXE)
    message(STATUS "构建 exe 客户端 (命令行执行)")
    add_subdirectory(stock_exe)
endif()

# ========== 测试 ==========

if(BUILD_TESTS)
    add_subdirectory(tests)
endif()

# ========== 构建信息 ==========

message(STATUS "========================================")
message(STATUS "Stock for C++ v${VERSION_STRING}.${VERSION_BUILD}")
message(STATUS "========================================")
message(STATUS "构建目标:")
message(STATUS "  核心库: ${BUILD_CORE}")
message(STATUS "  TUI 客户端: ${BUILD_TUI}")
message(STATUS "  GUI 客户端: ${BUILD_GUI}")
message(STATUS "  exe 客户端: ${BUILD_EXE}")
message(STATUS "========================================")

# ========== 旧版兼容 (临时保留) ==========

# 为了兼容性,保留旧的 stock_for_cpp 可执行文件
# 当所有客户端实现完成后,将删除此部分

option(BUILD_LEGACY "构建旧版 stock_for_cpp (兼容模式)" OFF)

if(BUILD_LEGACY)
    message(STATUS "构建旧版 stock_for_cpp (兼容模式)")
    
    add_executable(stock_for_cpp main.cpp)
    
    # 链接第三方库
    set(STOCK_LINK_LIBS
        config_lib
        scheduler_lib
        core_lib
        log_lib
        network_lib
        data_lib
        analysis_lib
        utils_lib
        eventbus_lib
        spdlog::spdlog
        sqlpp11::sqlpp11
        sqlite3
        nlohmann_json::nlohmann_json
        httplib::httplib
        dotenv
        ta-lib-static
    )
    
    # 如果 MySQL 可用，链接 MySQL 库
    if(HAS_MYSQL)
        list(APPEND STOCK_LINK_LIBS sqlpp11::mysql)
    endif()
    
    target_link_libraries(stock_for_cpp PRIVATE ${STOCK_LINK_LIBS})
    
    # 包含第三方库头文件目录
    target_include_directories(stock_for_cpp PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/config
        ${CMAKE_CURRENT_SOURCE_DIR}/core
        ${CMAKE_CURRENT_SOURCE_DIR}/log
        ${CMAKE_CURRENT_SOURCE_DIR}/network
        ${CMAKE_CURRENT_SOURCE_DIR}/data
    ${CMAKE_CURRENT_SOURCE_DIR}/analysis
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/spdlog/include
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/sqlpp11/include
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/json/include
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/cpp-httplib
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/dotenv-cpp/include
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/ta-lib/include
    ${CMAKE_CURRENT_BINARY_DIR}
)

# 把 .env 拷贝到可执行文件所在目录
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/.env DESTINATION ${CMAKE_CURRENT_SOURCE_DIR})

endif() # BUILD_LEGACY