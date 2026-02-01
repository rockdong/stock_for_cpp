cmake_minimum_required(VERSION 3.1)
project(stock_for_cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
    ${CMAKE_CURRENT_SOURCE_DIR}/log
    ${CMAKE_CURRENT_SOURCE_DIR}/config
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/cpp-httplib
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/json/include
)

target_link_libraries(network_lib PUBLIC
    httplib::httplib
    nlohmann_json::nlohmann_json
    log_lib
    config_lib
)

# 创建可执行文件
add_executable(stock_for_cpp main.cpp)

# 链接第三方库
target_link_libraries(stock_for_cpp PRIVATE
    config_lib
    log_lib
    network_lib
    spdlog::spdlog
    sqlpp11::sqlpp11
    nlohmann_json::nlohmann_json
    httplib::httplib
    dotenv
)

# 包含第三方库头文件目录
target_include_directories(stock_for_cpp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/config
    ${CMAKE_CURRENT_SOURCE_DIR}/log
    ${CMAKE_CURRENT_SOURCE_DIR}/network
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/spdlog/include
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/sqlpp11/include
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/json/include
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/cpp-httplib
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/dotenv-cpp/include
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/ta-lib/include
)
