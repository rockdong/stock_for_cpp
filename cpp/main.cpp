#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <atomic>
#include <chrono>

// 版本信息
#include "version.h"

// 配置和日志
#include "Config.h"
#include "Logger.h"

// 数据层
#include "Connection.h"
#include "StockDAO.h"
#include "AnalysisResultDAO.h"

// 网络层
#include "DataSourceFactory.h"

// 核心业务
#include "Core.h"

// 调度器
#include "Scheduler.h"

// 工具类
#include "ThreadPool.h"

#include <csignal>

// 全局调度器指针（用于信号处理）
scheduler::Scheduler* g_scheduler = nullptr;

// 信号处理函数
void signalHandler(int signal) {
    LOG_INFO("收到退出信号，正在优雅关闭...");
    if (g_scheduler) {
        g_scheduler->stop();
    }
}

struct ProgramOptions {
    bool onceMode = false;
    std::string executeTime = "20:00";
    bool help = false;
};

ProgramOptions parseArgs(int argc, char* argv[]) {
    ProgramOptions options;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--once" || arg == "-o") {
            options.onceMode = true;
        } else if (arg == "--help" || arg == "-h") {
            options.help = true;
        } else if (arg == "--time" || arg == "-t") {
            if (i + 1 < argc) {
                options.executeTime = argv[++i];
            }
        }
    }
    
    return options;
}

void printHelp(const char* programName) {
    std::cout << "用法: " << programName << " [选项]" << std::endl;
    std::cout << std::endl;
    std::cout << "选项:" << std::endl;
    std::cout << "  --once, -o       单次执行模式（默认：定时模式）" << std::endl;
    std::cout << "  --time, -t TIME  设置执行时间，格式 HH:MM（默认：20:00）" << std::endl;
    std::cout << "  --help, -h       显示帮助信息" << std::endl;
    std::cout << std::endl;
    std::cout << "示例:" << std::endl;
    std::cout << "  " << programName << "           # 定时模式，每天 20:00 执行" << std::endl;
    std::cout << "  " << programName << " --once   # 单次执行模式" << std::endl;
    std::cout << "  " << programName << " -t 09:30 # 每天 09:30 执行" << std::endl;
}

/**
 * @brief 打印版本信息
 */
void printVersion() {
    std::cout << "========================================" << std::endl;
    std::cout << "📊 Stock Analysis System" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "版本号: " << VERSION_STRING << "." << VERSION_BUILD << std::endl;
    std::cout << "构建日期: " << BUILD_DATE << std::endl;
    std::cout << "Git 提交: " << GIT_COMMIT << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
}

/**
 * @brief 初始化系统配置和日志
 * @return 是否成功
 */
bool initializeSystem() {
    // 初始化配置系统
    auto& config = config::Config::getInstance();
    if (!config.initialize(".env")) {
        std::cerr << "配置初始化失败" << std::endl;
        return false;
    }
    
    // 初始化日志系统
    logger::init();
    
    LOG_INFO("========================================");
    LOG_INFO("应用名称: " + config.getAppName());
    LOG_INFO("应用版本: " + std::string(VERSION_STRING) + "." + std::to_string(VERSION_BUILD));
    LOG_INFO("运行环境: " + config.getAppEnv());
    LOG_INFO("构建日期: " + std::string(BUILD_DATE));
    LOG_INFO("Git 提交: " + std::string(GIT_COMMIT));
    LOG_INFO("========================================");
    
    return true;
}

/**
 * @brief 打印配置信息
 */
void printConfiguration() {
    auto& config = config::Config::getInstance();
    
    LOG_INFO("数据库配置:");
    LOG_INFO("  主机: " + config.getDbHost());
    LOG_INFO("  端口: " + std::to_string(config.getDbPort()));
    LOG_INFO("  数据库: " + config.getDbName());
    
    LOG_INFO("数据源配置:");
    LOG_INFO("  URL: " + config.getDataSourceUrl());
    LOG_INFO("  超时: " + std::to_string(config.getDataSourceTimeout()) + "秒");
    
    LOG_INFO("策略配置:");
    LOG_INFO("  策略列表: " + config.getStrategies());
    
    LOG_INFO("========================================");
}

/**
 * @brief 初始化数据库连接
 * @return 是否成功
 */
bool initializeDatabase() {
    auto& conn = data::Connection::getInstance();
    
    if (!conn.initialize("stock.db")) {
        LOG_ERROR("数据库初始化失败");
        return false;
    }
    
    if (!conn.connect()) {
        LOG_ERROR("数据库连接失败");
        return false;
    }
    
    LOG_INFO("数据库连接成功");
    return true;
}

/**
 * @brief 初始化策略管理器
 * @return 是否成功
 */
bool initializeStrategies() {
    auto& strategyManager = core::StrategyManager::getInstance();
    
    if (!strategyManager.initializeFromEnv("STRATEGIES")) {
        LOG_ERROR("策略管理器初始化失败");
        return false;
    }
    
    LOG_INFO("策略管理器初始化成功，已注册 " + 
             std::to_string(strategyManager.count()) + " 个策略:");
    
    for (const auto& name : strategyManager.getStrategyNames()) {
        LOG_INFO("  - " + name);
    }
    
    return true;
}

/**
 * @brief 加载或获取股票列表
 * @param stockDao 股票 DAO
 * @param dataSource 数据源
 * @return 股票列表
 */
std::vector<core::Stock> loadStockList(
    std::shared_ptr<network::IDataSource> dataSource
) { 
    // 数据库为空，从 API 获取
    LOG_INFO("从 Tushare API 获取股票列表...");
    auto stock_list = dataSource->getStockList();
    LOG_INFO("从 API 获取到 " + std::to_string(stock_list.size()) + " 只股票");
    
    return stock_list;
}

/**
 * @brief 分析单只股票
 * @param stock 股票信息
 * @param dataSource 数据源
 * @param strategyManager 策略管理器
 * @param analysisResultDao 分析结果 DAO
 */
void analyzeStock(
    const core::Stock& stock,
    std::shared_ptr<network::IDataSource> dataSource,
    core::StrategyManager& strategyManager,
    data::AnalysisResultDAO& analysisResultDao
) {
    LOG_INFO("开始分析: " + stock.ts_code + " (" + stock.name + ")");
    
    std::vector<core::AnalysisResult> results;
    
    // 对每个频率进行分析
    const std::vector<std::string> frequencies = {"d", "w", "m"};
    const std::vector<std::string> freqNames = {"日线", "周线", "月线"};
    
    for (size_t i = 0; i < frequencies.size(); ++i) {
        const auto& freq = frequencies[i];
        const auto& freqName = freqNames[i];
        
        try {
            // 获取股票数据
            auto data = dataSource->getQuoteData(stock.ts_code, "", "", freq);
            
            if (data.empty()) {
                LOG_WARN("  " + freqName + ": 无数据");
                continue;
            }
            
            // 使用所有策略分析
            auto strategyResults = strategyManager.analyzeAll(stock.ts_code, data);
            
            // 收集有效结果
            for (const auto& pair : strategyResults) {
                if (pair.second.has_value()) {
                    auto result = *pair.second;
                    // 设置频率
                    result.freq = freq;
                    // 根据 label 设置 opt
                    if (result.label == "买入") {
                        result.opt = "买入";
                    }
                    
                    results.push_back(result);
                    LOG_INFO("  " + freqName + " - " + pair.first + ": " + result.label);
                }
            }
            
        } catch (const std::exception& e) {
            LOG_ERROR("  " + freqName + " 分析失败: " + std::string(e.what()));
        }
    }
    
    // 批量保存结果
    if (!results.empty()) {
        int saved = analysisResultDao.batchInsert(results);
        LOG_INFO("保存 " + std::to_string(saved) + " 条分析结果");
    }
}

/**
 * @brief 执行批量分析（多线程版本）
 * @param stockList 股票列表
 * @param dataSource 数据源
 * @param strategyManager 策略管理器
 * @param analysisResultDao 分析结果 DAO
 */
void performBatchAnalysis(
    const std::vector<core::Stock>& stockList,
    std::shared_ptr<network::IDataSource> dataSource,
    core::StrategyManager& strategyManager,
    data::AnalysisResultDAO& analysisResultDao
) {
    LOG_INFO("========================================");
    LOG_INFO("开始批量分析，共 " + std::to_string(stockList.size()) + " 只股票");
    LOG_INFO("========================================");
    
    unsigned int threadCount = std::thread::hardware_concurrency();
    if (threadCount == 0) threadCount = 4;
    LOG_INFO("使用 " + std::to_string(threadCount) + " 个线程并发处理");
    
    ThreadPool pool(threadCount);
    
    std::atomic<int> successCount(0);
    std::atomic<int> failCount(0);
    
    // 为了遵守 Tushare API 每分钟 500 次的调用限制，计算线程间的延迟
    // 假设每只股票需要调用 3 次 API（日线、周线、月线），500 次/分钟 = 8.33 次/秒
    // 安全起见，每秒最多调用 2 次，即每秒分析不超过 1 只股票
    const int delay_between_tasks = 500; // 500ms，确保每秒不超过 2 次调用
    
    for (const auto& stock : stockList) {
        pool.enqueue([&]() {
            try {
                auto localDataSource = network::DataSourceFactory::createFromConfig();
                analyzeStock(stock, localDataSource, strategyManager, analysisResultDao);
                successCount++;
            } catch (const std::exception& e) {
                LOG_ERROR("分析失败: " + stock.ts_code + " - " + std::string(e.what()));
                failCount++;
            }
        });
        
        // 在任务间添加延迟以控制 API 调用频率
        std::this_thread::sleep_for(std::chrono::milliseconds(delay_between_tasks));
    }
    
    pool.wait();
    
    LOG_INFO("========================================");
    LOG_INFO("批量分析完成");
    LOG_INFO("  成功: " + std::to_string(successCount.load()) + " 只");
    LOG_INFO("  失败: " + std::to_string(failCount.load()) + " 只");
    LOG_INFO("========================================");
}

/**
 * @brief 清理系统资源
 */
void cleanup() {
    // 断开数据库连接
    auto& conn = data::Connection::getInstance();
    conn.disconnect();
    
    // 关闭日志系统
    logger::LoggerManager::getInstance().shutdown();
    
    LOG_INFO("应用程序退出");
}

/**
 * @brief 主函数
 */
int main(int argc, char* argv[]) {
    try {
        // 0. 解析命令行参数
        ProgramOptions options = parseArgs(argc, argv);
        
        if (options.help) {
            printHelp(argv[0]);
            return 0;
        }
        
        // 注册信号处理
        std::signal(SIGINT, signalHandler);
        std::signal(SIGTERM, signalHandler);
        
        // 0. 打印版本信息
        printVersion();
        
        // 1. 初始化系统
        if (!initializeSystem()) {
            return 1;
        }
        
        // 2. 打印配置信息
        printConfiguration();
        
        // 3. 初始化数据库
        if (!initializeDatabase()) {
            return 1;
        }
        
        // 4. 初始化策略管理器
        if (!initializeStrategies()) {
            return 1;
        }
        
        // 5. 创建 DAO 和数据源
        data::StockDAO stockDao;
        data::AnalysisResultDAO analysisResultDao;
        auto dataSource = network::DataSourceFactory::createFromConfig();
        auto& strategyManager = core::StrategyManager::getInstance();
        
        // 6. 加载股票列表
        auto stockList = loadStockList(dataSource);
        
        // 7. 根据模式执行
        if (options.onceMode) {
            // 单次执行模式
            LOG_INFO("执行单次分析...");
            performBatchAnalysis(stockList, dataSource, strategyManager, analysisResultDao);
        } else {
            // 定时执行模式
            LOG_INFO("启动定时调度模式，执行时间: " + options.executeTime);
            
            scheduler::Scheduler sched(options.executeTime, [&]() {
                performBatchAnalysis(stockList, dataSource, strategyManager, analysisResultDao);
            });
            
            g_scheduler = &sched;
            sched.run();
        }
        
        // 8. 清理资源
        cleanup();
        
        std::cout << "应用程序正常退出" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "程序异常: " << e.what() << std::endl;
        LOG_ERROR("程序异常: " + std::string(e.what()));
        cleanup();
        return 1;
    }
}
