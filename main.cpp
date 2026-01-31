#include <iostream>
#include "Logger.h"
#include "Config.h"

int main() {
    // 初始化配置系统
    auto& config = config::Config::getInstance();
    if (!config.initialize(".env")) {
        std::cerr << "配置初始化失败" << std::endl;
        return 1;
    }
    
    // 初始化日志系统（从 .env 文件加载配置）
    logger::init(".env");
    
    // 输出应用信息
    LOG_INFO("========================================");
    LOG_INFO("应用名称: " + config.getAppName());
    LOG_INFO("应用版本: " + config.getAppVersion());
    LOG_INFO("运行环境: " + config.getAppEnv());
    LOG_INFO("========================================");
    
    // 输出配置信息
    LOG_INFO("数据库配置:");
    LOG_INFO("  主机: " + config.getDbHost());
    LOG_INFO("  端口: " + std::to_string(config.getDbPort()));
    LOG_INFO("  数据库: " + config.getDbName());
    LOG_INFO("  连接池大小: " + std::to_string(config.getDbPoolSize()));
    
    LOG_INFO("数据源配置:");
    LOG_INFO("  URL: " + config.getDataSourceUrl());
    LOG_INFO("  超时: " + std::to_string(config.getDataSourceTimeout()) + "秒");
    LOG_INFO("  重试次数: " + std::to_string(config.getDataSourceRetryTimes()));
    
    LOG_INFO("缓存配置:");
    LOG_INFO("  启用: " + std::string(config.isCacheEnabled() ? "是" : "否"));
    LOG_INFO("  大小: " + std::to_string(config.getCacheSize()));
    LOG_INFO("  TTL: " + std::to_string(config.getCacheTtl()) + "秒");
    
    LOG_INFO("输出配置:");
    LOG_INFO("  输出目录: " + config.getOutputDir());
    LOG_INFO("  导出格式: " + config.getExportFormat());
    LOG_INFO("  图表: " + std::string(config.isChartEnabled() ? "启用" : "禁用"));
    
    LOG_INFO("分析配置:");
    LOG_INFO("  MA周期: " + std::to_string(config.getDefaultMaPeriod()));
    LOG_INFO("  MACD快线: " + std::to_string(config.getDefaultMacdFast()));
    LOG_INFO("  MACD慢线: " + std::to_string(config.getDefaultMacdSlow()));
    LOG_INFO("  RSI周期: " + std::to_string(config.getDefaultRsiPeriod()));
    
    LOG_INFO("========================================");
    
    // 使用命名日志器
    auto db_logger = logger::getLogger("database");
    db_logger->info("数据库模块初始化完成");
    
    auto analysis_logger = logger::getLogger("analysis");
    analysis_logger->info("分析模块初始化完成");
    
    // 刷新所有日志
    logger::LoggerManager::getInstance().flushAll();
    
    LOG_INFO("应用程序正常退出");
    
    return 0;
}
