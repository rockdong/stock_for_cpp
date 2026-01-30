#include <iostream>
#include "Logger.h"

int main() {
    // 初始化日志系统（从 .env 文件加载配置）
    logger::init(".env");
    
    // 使用默认日志器
    LOG_INFO("应用程序启动");
    LOG_DEBUG("这是一条调试信息");
    LOG_WARN("这是一条警告信息");
    LOG_ERROR("这是一条错误信息");
    
    // 使用命名日志器
    auto db_logger = logger::getLogger("database");
    db_logger->info("数据库连接成功");
    db_logger->debug("执行查询: SELECT * FROM users");
    
    // 使用宏记录日志
    LOG_INFO_N("network", "网络请求开始");
    LOG_INFO_N("network", "网络请求完成");
    
    // 刷新所有日志
    logger::LoggerManager::getInstance().flushAll();
    
    LOG_INFO("应用程序正常退出");
    
    return 0;
}
