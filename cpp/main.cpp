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
#include "ChartDataDAO.h"

// 网络层
#include "DataSourceFactory.h"

// 核心业务
#include "Core.h"
#include "indicators/EMA.h"

// 调度器
#include "Scheduler.h"

// 工具类
#include "ThreadPool.h"
#include "utils/TimeUtil.h"
#include "utils/RateLimiter.h"

#include <csignal>

// 全局调度器指针（用于信号处理）
scheduler::Scheduler* g_scheduler = nullptr;

// 信号处理函数
void signalHandler(int signal) {
    LOG_INFO("收到退出信号，正在优雅关闭...");
    RateLimiter::getInstance().stop();
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
 * @brief 计算分析日期
 * 考虑切换时间：当前时间 < 切换时间时，分析日期为昨天
 * @return 分析日期（格式：YYYYMMDD）
 */
std::string calculateAnalysisDate() {
    auto& config = config::Config::getInstance();
    std::string switchTimeStr = config.getAnalysisDateSwitchTime();
    
    // 解析切换时间 (格式: HH:MM)
    int switchHour = 2, switchMinute = 0;
    size_t colonPos = switchTimeStr.find(':');
    if (colonPos != std::string::npos) {
        switchHour = std::stoi(switchTimeStr.substr(0, colonPos));
        switchMinute = std::stoi(switchTimeStr.substr(colonPos + 1));
    }
    
    // 获取当前时间
    auto now = std::chrono::system_clock::now();
    auto nowTime = std::chrono::system_clock::to_time_t(now);
    std::tm* nowTm = std::localtime(&nowTime);
    
    // 判断是否需要使用昨天的日期
    int currentMinutes = nowTm->tm_hour * 60 + nowTm->tm_min;
    int switchMinutes = switchHour * 60 + switchMinute;
    
    if (currentMinutes < switchMinutes) {
        // 使用昨天的日期
        nowTm->tm_mday -= 1;
        std::mktime(nowTm); // 规范化 tm 结构
    }
    
    // 格式化为 YYYYMMDD
    char buffer[9];
    std::strftime(buffer, sizeof(buffer), "%Y%m%d", nowTm);
    return std::string(buffer);
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
    data::AnalysisResultDAO& analysisResultDao,
    data::ChartDataDAO& chartDataDao
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
            
            // 计算分析日期
            std::string analysisDate = calculateAnalysisDate();
            
            // 计算并保存图表数据
            try {
                if (data.size() >= 17) {
                    std::vector<double> closePrices;
                    for (const auto& d : data) {
                        closePrices.push_back(d.close);
                    }
                    
                    auto ema17Values = analysis::EMA::compute(closePrices, 17);
                    auto ema25Values = analysis::EMA::compute(closePrices, 25);
                    
                    data::ChartData chartData;
                    chartData.ts_code = stock.ts_code;
                    chartData.freq = freq;
                    chartData.analysis_date = analysisDate;
                    
                    size_t startIndex = data.size() > 10 ? data.size() - 10 : 0;
                    size_t emaOffset17 = ema17Values.size() - (data.size() - startIndex);
                    size_t emaOffset25 = ema25Values.size() - (data.size() - startIndex);
                    
                    for (size_t j = startIndex; j < data.size(); ++j) {
                        data::ChartCandle candle;
                        candle.trade_date = data[j].trade_date;
                        candle.open = data[j].open;
                        candle.high = data[j].high;
                        candle.low = data[j].low;
                        candle.close = data[j].close;
                        candle.volume = static_cast<double>(data[j].volume);
                        
                        size_t emaIdx17 = j - startIndex + emaOffset17;
                        size_t emaIdx25 = j - startIndex + emaOffset25;
                        
                        if (emaIdx17 < ema17Values.size()) {
                            candle.ema17 = ema17Values[emaIdx17];
                        }
                        if (emaIdx25 < ema25Values.size()) {
                            candle.ema25 = ema25Values[emaIdx25];
                        }
                        
                        chartData.candles.push_back(candle);
                    }
                    
                    chartDataDao.save(chartData);
                    LOG_DEBUG("  " + freqName + ": 保存图表数据 " + std::to_string(chartData.candles.size()) + " 条");
                }
            } catch (const std::exception& e) {
                LOG_ERROR("  " + freqName + " 图表数据保存失败: " + std::string(e.what()));
            }
            
            // 收集有效结果
            for (const auto& pair : strategyResults) {
                if (pair.second.has_value()) {
                    auto result = *pair.second;
                    // 设置分析日期（不是股票交易日期）
                    result.trade_date = analysisDate;
                    // 设置频率
                    result.freq = freq;
                    // opt 使用 freq 值，后续会转为 emoji
                    result.opt = freq;
                    
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
 * @brief 更新分析进度
 * @param total 总数（-1 表示不更新）
 * @param completed 已完成（-1 表示不更新）
 * @param failed 失败数（-1 表示不更新）
 * @param status 状态（空表示不更新）
 */
void updateProgress(int total = -1, int completed = -1, int failed = -1, const std::string& status = "") {
    auto& conn = data::Connection::getInstance();
    if (!conn.isConnected()) return;
    
    std::string sql = "UPDATE analysis_progress SET updated_at = CURRENT_TIMESTAMP";
    if (total >= 0) sql += ", total = " + std::to_string(total);
    if (completed >= 0) sql += ", completed = " + std::to_string(completed);
    if (failed >= 0) sql += ", failed = " + std::to_string(failed);
    if (!status.empty()) sql += ", status = '" + status + "'";
    if (status == "running") sql += ", started_at = CURRENT_TIMESTAMP";
    sql += " WHERE id = 1";
    
    conn.execute(sql);
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
    data::AnalysisResultDAO& analysisResultDao,
    data::ChartDataDAO& chartDataDao
) {
    LOG_INFO("========================================");
    LOG_INFO("开始批量分析，共 " + std::to_string(stockList.size()) + " 只股票");
    LOG_INFO("========================================");
    
    int total = static_cast<int>(stockList.size());
    updateProgress(total, 0, 0, "running");
    
    unsigned int threadCount = std::thread::hardware_concurrency();
    if (threadCount == 0) threadCount = 4;
    LOG_INFO("使用 " + std::to_string(threadCount) + " 个线程并发处理");
    
    ThreadPool pool(threadCount);
    
    std::atomic<int> successCount(0);
    std::atomic<int> failCount(0);
    std::atomic<int> lastReportedProgress(0);
    
    const int delay_between_tasks = 300;
    
    for (const auto& stock : stockList) {
        pool.enqueue([&, stock]() {
            try {
                auto localDataSource = network::DataSourceFactory::createFromConfig();
                analyzeStock(stock, localDataSource, strategyManager, analysisResultDao, chartDataDao);
                int completed = successCount.fetch_add(1) + 1 + failCount.load();
                if (completed - lastReportedProgress.load() >= 10) {
                    updateProgress(-1, completed, failCount.load());
                    lastReportedProgress = completed;
                }
            } catch (const std::exception& e) {
                LOG_ERROR("分析失败: " + stock.ts_code + " - " + std::string(e.what()));
                int failed = failCount.fetch_add(1) + 1;
                int completed = successCount.load() + failed;
                updateProgress(-1, completed, failed);
            }
        });
        
        std::this_thread::sleep_for(std::chrono::milliseconds(delay_between_tasks));
    }
    
    pool.wait();
    
    updateProgress(-1, successCount.load(), failCount.load(), "completed");
    
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
        data::ChartDataDAO chartDataDao;
        auto dataSource = network::DataSourceFactory::createFromConfig();
        auto& strategyManager = core::StrategyManager::getInstance();
        
        // 6. 加载股票列表
        auto stockList = loadStockList(dataSource);
        
        // 7. 根据模式执行
        if (options.onceMode) {
            // 单次执行模式
            LOG_INFO("执行单次分析...");
            performBatchAnalysis(stockList, dataSource, strategyManager, analysisResultDao, chartDataDao);
        } else {
            // 定时执行模式 - 首次启动先执行一次
            LOG_INFO("首次启动，立即执行一次分析...");
            performBatchAnalysis(stockList, dataSource, strategyManager, analysisResultDao, chartDataDao);
            
            LOG_INFO("启动定时调度模式，执行时间: " + options.executeTime);
            
            scheduler::Scheduler sched(options.executeTime, [&]() {
                performBatchAnalysis(stockList, dataSource, strategyManager, analysisResultDao, chartDataDao);
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
