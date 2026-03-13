cmake_minimum_required(VERSION 3.10)
project(stock_for_cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
add_subdirectory(thirdparty/sqlpp11)
add_subdirectory(thirdparty/json)
add_subdirectory(thirdparty/cpp-httplib)
add_subdirectory(thirdparty/dotenv-cpp)

# TA-Lib 通常需要特殊处理，因为它是 C 库
# 如果 thirdparty/ta-lib 有 CMakeLists.txt，可以取消下面的注释
add_subdirectory(thirdparty/ta-lib)

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
)

target_link_libraries(scheduler_lib PUBLIC
    log_lib
    utils_lib
    core_lib
)

# 添加核心业务模块库（策略系统）
add_library(core_lib STATIC
    core/AnalysisResult.cpp
    core/Strategy.cpp
    core/StrategyFactory.cpp
    core/StrategyManager.cpp
    core/strategies/MACrossStrategy.cpp
    core/strategies/MACDStrategy.cpp
    core/strategies/RSIStrategy.cpp
    core/strategies/BOLLStrategy.cpp
    core/strategies/GridStrategy.cpp
    core/strategies/EMA17TO25Strategy.cpp
    core/strategies/EMA17BreakoutStrategy.cpp
    core/strategies/EMAConvergenceStrategy.cpp
    core/strategies/EMA25Greater17PriceMatchStrategy.cpp
)

target_include_directories(core_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/core/strategies
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/analysis
    ${CMAKE_CURRENT_SOURCE_DIR}/analysis/indicators
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/ta-lib/include
)

target_link_libraries(core_lib PUBLIC
    utils_lib
    analysis_lib
)

# 添加日志库
add_library(log_lib STATIC
    log/LogConfig.cpp
    log/LoggerFactory.cpp
    log/LoggerManager.cpp
    log/SpdlogLogger.cpp
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
add_library(data_lib STATIC
    data/Data.cpp
    data/database/Connection.cpp
    data/database/StockDAO.cpp
    data/database/PriceDAO.cpp
    data/database/AnalysisResultDAO.cpp
    data/database/ChartDataDAO.cpp
    data/cache/MemoryCache.cpp
    data/file/CSVReader.cpp
    data/file/CSVWriter.cpp
)

target_include_directories(data_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/data
    ${CMAKE_CURRENT_SOURCE_DIR}/data/database
    ${CMAKE_CURRENT_SOURCE_DIR}/data/cache
    ${CMAKE_CURRENT_SOURCE_DIR}/data/file
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/log
    ${CMAKE_CURRENT_SOURCE_DIR}/config
    ${CMAKE_CURRENT_SOURCE_DIR}/network
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

# 添加工具类库
find_package(OpenSSL REQUIRED)

add_library(utils_lib STATIC
    utils/TimeUtil.cpp
    utils/StringUtil.cpp
    utils/MathUtil.cpp
    utils/Encryptor.cpp
    utils/ThreadPool.cpp
    utils/RateLimiter.cpp
)

target_include_directories(utils_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
)

target_link_libraries(utils_lib PUBLIC
    OpenSSL::SSL
    OpenSSL::Crypto
)

# 创建可执行文件
add_executable(stock_for_cpp main.cpp)

# 链接第三方库
target_link_libraries(stock_for_cpp PRIVATE
    config_lib
    scheduler_lib
    core_lib
    log_lib
    network_lib
    data_lib
    analysis_lib
    utils_lib
    spdlog::spdlog
    sqlpp11::sqlpp11
    sqlite3
    nlohmann_json::nlohmann_json
    httplib::httplib
    dotenv
    ta-lib-static
)

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