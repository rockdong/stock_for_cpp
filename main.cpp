#include <iostream>
#include "Logger.h"
#include "Config.h"
#include "DataSourceFactory.h"
#include "TushareDataSource.h"
#include "Connection.h"
#include "StockDAO.h"

int main() {
    // 初始化配置系统
    auto& config = config::Config::getInstance();
    if (!config.initialize(".env")) {
        std::cerr << "配置初始化失败" << std::endl;
        return 1;
    }
    
    // 初始化日志系统（LogConfig 会从环境变量读取配置）
    logger::init();
    
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

    // 初始化数据库连接
    auto& conn = data::Connection::getInstance();
    if (!conn.initialize("stock.db")) {
        LOG_ERROR("数据库初始化失败");
        return 1;
    }
    
    if (!conn.connect()) {
        LOG_ERROR("数据库连接失败");
        return 1;
    }
    LOG_INFO("数据库连接成功");

    // 创建 StockDAO 实例
    data::StockDAO stockDao;

    // 先从数据库中查询是否有股票列表
    auto stock_list = stockDao.findAll();
    LOG_INFO("数据库中已有 " + std::to_string(stock_list.size()) + " 只股票");
    
    if (!stock_list.empty()) {
        // 只显示前 10 只股票
        int count = 0;
        for (const auto& stock : stock_list) {
            LOG_INFO("股票代码: " + stock.ts_code + ", 股票名称: " + stock.name);
            if (++count >= 10) {
                LOG_INFO("... 还有 " + std::to_string(stock_list.size() - 10) + " 只股票");
                break;
            }
        }
    }

    // 如果没有就通过接口获取并且保存到数据库中
    if (stock_list.empty()) {
        LOG_INFO("数据库为空，从 Tushare API 获取股票列表...");
        auto dataSource = network::DataSourceFactory::createFromConfig();
        stock_list = dataSource->getStockList();
        LOG_INFO("从 API 获取到 " + std::to_string(stock_list.size()) + " 只股票");
        
        // 转换并保存到数据库
        std::vector<data::Stock> stocks_to_save;
        for (const auto& api_stock : stock_list) {
            data::Stock stock;
            stock.ts_code = api_stock.ts_code;
            stock.symbol = api_stock.symbol;
            stock.name = api_stock.name;
            stock.area = api_stock.area;
            stock.industry = api_stock.industry;
            stock.market = api_stock.market;
            stock.list_date = api_stock.list_date;
            stocks_to_save.push_back(stock);
        }
        
        int saved_count = stockDao.batchInsert(stocks_to_save);
        LOG_INFO("成功保存 " + std::to_string(saved_count) + " 只股票到数据库");
    }

    for (const auto& stock : stock_list) {
        LOG_INFO("股票代码: " + stock.ts_code + ", 股票名称: " + stock.name);
        // 循环 日 周 月
        for (const auto& freq : {"d", "w", "m"}) {
            auto data = dataSource->getQuoteData(stock.ts_code, "", "", freq);
            LOG_INFO("股票代码: " + stock.ts_code + ", 股票名称: " + stock.name + ", 频率: " + freq + ", 数据条数: " + std::to_string(data.size()));
        }
    }
    
    // 测试新增的周线和月线接口
    LOG_INFO("========================================");
    LOG_INFO("测试行情数据接口（日线、周线、月线）");
    LOG_INFO("========================================");
    
    if (!stock_list.empty()) {
        // 使用第一只股票进行测试
        std::string test_ts_code = stock_list[0].ts_code;
        LOG_INFO("测试股票: " + test_ts_code + " (" + stock_list[0].name + ")");
        
        try {
            auto dataSource = network::DataSourceFactory::createFromConfig();
            
            // 测试日线数据
            LOG_INFO("获取日线数据...");
            auto daily_data = dataSource->getQuoteData(test_ts_code, "20240101", "20240110", "d");
            LOG_INFO("日线数据条数: " + std::to_string(daily_data.size()));
            
            // 测试周线数据
            LOG_INFO("获取周线数据...");
            auto weekly_data = dataSource->getQuoteData(test_ts_code, "20240101", "20240331", "w");
            LOG_INFO("周线数据条数: " + std::to_string(weekly_data.size()));
            
            // 测试月线数据
            LOG_INFO("获取月线数据...");
            auto monthly_data = dataSource->getQuoteData(test_ts_code, "20230101", "20231231", "m");
            LOG_INFO("月线数据条数: " + std::to_string(monthly_data.size()));
            
            LOG_INFO("行情数据接口测试完成");
        } catch (const std::exception& e) {
            LOG_ERROR("测试失败: " + std::string(e.what()));
        }
    }
    
    LOG_INFO("========================================");
    
    // 断开数据库连接
    conn.disconnect();
    
    LOG_INFO("应用程序退出");
    
    // 显式关闭日志系统，避免析构顺序问题
    logger::LoggerManager::getInstance().shutdown();

    std::cout << "应用程序退出" << std::endl;
    
    return 0;
}
