cmake_minimum_required(VERSION 3.1)
project(stock_for_cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加第三方库子目录
add_subdirectory(thirdparty/spdlog)
add_subdirectory(thirdparty/sqlpp11)
add_subdirectory(thirdparty/json)
add_subdirectory(thirdparty/cpp-httplib)
add_subdirectory(thirdparty/dotenv-cpp)

# TA-Lib 通常需要特殊处理，因为它是 C 库
# 如果 thirdparty/ta-lib 有 CMakeLists.txt，可以取消下面的注释
add_subdirectory(thirdparty/ta-lib)

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

# 创建可执行文件
add_executable(stock_for_cpp main.cpp)

# 链接第三方库
target_link_libraries(stock_for_cpp PRIVATE
    log_lib
    spdlog::spdlog
    sqlpp11::sqlpp11
    nlohmann_json::nlohmann_json
    httplib::httplib
    dotenv
)

# 包含第三方库头文件目录
target_include_directories(stock_for_cpp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/log
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/spdlog/include
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/sqlpp11/include
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/json/include
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/cpp-httplib
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/dotenv-cpp/include
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/ta-lib/include
)
