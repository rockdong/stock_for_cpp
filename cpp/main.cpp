#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <atomic>
#include <chrono>
#include <set>
#include <unordered_map>

// 版本信息
#include "version.h"

// 配置和日志
#include "Config.h"
#include "Logger.h"

// EventBus
#include "EventBus.h"

// 数据层
#include "Connection.h"
#include "StockDAO.h"
#include "AnalysisResultDAO.h"
#include "ChartDataDAO.h"
#include "AnalysisProcessRecordDAO.h"

// 网络层
#include "DataSourceFactory.h"
#include "network/TushareDataSource.h"

// 核心业务
#include "Core.h"
#include "core/FundamentalFilter.h"
#include "indicators/EMA.h"

// JSON
#include "../thirdparty/json/include/nlohmann/json.hpp"

// 调度器
#include "Scheduler.h"

// 工具类
#include "ThreadPool.h"
#include "utils/TimeUtil.h"
#include "utils/RateLimiter.h"

#include <csignal>

using json = nlohmann::json;

// 前置声明
void updatePhase1Progress(int total, int completed, int qualified, const std::string& status);
void updatePhase2Progress(int total, int completed, int failed, const std::string& status);

// 全局调度器指针（用于信号处理）
scheduler::Scheduler* g_scheduler = nullptr;

void signalHandler(int signal) {
    LOG_INFO("收到退出信号，正在优雅关闭...");
    RateLimiter::getInstance().stop();
    if (g_scheduler) {
        g_scheduler->stop();
    }
}
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

bool checkStartupAnalysisNeeded(scheduler::Scheduler& sched, data::AnalysisProcessRecordDAO& dao) {
    auto startCheck = std::chrono::high_resolution_clock::now();
    
    std::string analysisDate = utils::TimeUtil::today();
    
    try {
        if (sched.hasRunToday()) {
            LOG_INFO("文件记录显示今天已完成分析，验证数据库进度...");
            
            auto progress = dao.getProgress();
            
            bool phase1Completed = progress.phase1.status == "completed";
            bool phase2Completed = progress.phase2.status == "completed" &&
                                   progress.phase2.completed == progress.phase2.total &&
                                   progress.phase2.total > 0;
            
            if (phase1Completed && phase2Completed) {
                auto endCheck = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endCheck - startCheck);
                
                LOG_INFO("今天已完成分析，跳过首次执行，进入调度等待");
                LOG_INFO("分析日期: " + analysisDate + 
                         ", Phase1: " + progress.phase1.status + " (" + std::to_string(progress.phase1.qualified) + " 只优质股)" +
                         ", Phase2: " + std::to_string(progress.phase2.completed) + "/" + std::to_string(progress.phase2.total) +
                         ", 检查耗时: " + std::to_string(duration.count()) + "ms");
                
                return false;
            } else {
                LOG_WARN("文件记录与数据库状态不一致，将重新执行分析");
                LOG_WARN("Phase1: " + progress.phase1.status + 
                         ", Phase2: " + progress.phase2.status + 
                         ", 完成: " + std::to_string(progress.phase2.completed) + 
                         ", 总数: " + std::to_string(progress.phase2.total));
                return true;
            }
        }
    } catch (const std::exception& e) {
        LOG_ERROR("启动检查异常: " + std::string(e.what()) + "，默认执行分析");
        return true;
    }
    
    LOG_INFO("首次启动或今天未完成，立即执行分析...");
    return true;
}

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
    
    // 初始化 EventBus
    if (!eventbus::initialize()) {
        LOG_ERROR("EventBus 初始化失败");
        return false;
    }
    
    LOG_INFO("EventBus 初始化成功");
    
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
 * @brief 执行基本面筛选
 * 获取全市场财务指标，计算评分，筛选优质股票池
 * @param dataSource 数据源
 * @return 优质股票代码列表（如果失败返回空列表）
 */
std::vector<std::string> performFundamentalScreening(
    std::shared_ptr<network::IDataSource> dataSource,
    const std::vector<core::Stock>& allStocks,
    const std::string& tradeDate
) {
    LOG_INFO("========================================");
    LOG_INFO("开始基本面筛选...");
    LOG_INFO("========================================");
    
    updatePhase1Progress(static_cast<int>(allStocks.size()), 0, 0, "running");
    
    try {
        LOG_INFO("获取每日指标数据 (daily_basic)...");
        
        auto tushareSource = std::dynamic_pointer_cast<network::TushareDataSource>(dataSource);
        if (!tushareSource) {
            LOG_WARN("数据源类型不支持基本面筛选，降级到全市场分析");
            updatePhase1Progress(-1, -1, -1, "completed");
            return {};
        }
        
        auto response = tushareSource->getClient()->getDailyBasic("", tradeDate, "", "");
        
        if (!response.isSuccess()) {
            LOG_ERROR("获取每日指标失败: " + response.msg);
            updatePhase1Progress(-1, -1, -1, "completed");
            return {};
        }
        
        std::vector<std::string> qualifiedCodes;
        int processedCount = 0;
        
        if (response.data.contains("fields") && response.data.contains("items")) {
            auto fields = response.data["fields"].get<std::vector<std::string>>();
            auto items = response.data["items"];
            
            std::unordered_map<std::string, size_t> fieldIndex;
            for (size_t i = 0; i < fields.size(); ++i) {
                fieldIndex[fields[i]] = i;
            }
            
            auto get_double = [&](const json& item, const std::string& fieldName) -> double {
                auto it = fieldIndex.find(fieldName);
                if (it != fieldIndex.end() && it->second < item.size() && !item[it->second].is_null()) {
                    return item[it->second].get<double>();
                }
                return 0.0;
            };
            
            for (const auto& item : items) {
                std::string tsCode = item[fieldIndex["ts_code"]].get<std::string>();
                
                double pe = get_double(item, "pe");
                double peTtm = get_double(item, "pe_ttm");
                double pb = get_double(item, "pb");
                double totalMv = get_double(item, "total_mv");
                double circMv = get_double(item, "circ_mv");
                double turnoverRate = get_double(item, "turnover_rate");
                
                double score = 0.0;
                
                if (pe > 0 && pe < 30) score += 10.0;
                else if (peTtm > 0 && peTtm < 30) score += 10.0;
                
                if (pb > 0 && pb < 5) score += 8.0;
                
                if (totalMv >= 50 && totalMv <= 5000) score += 5.0;
                
                if (turnoverRate >= 1 && turnoverRate <= 10) score += 2.0;
                
                if (score >= 15.0) {
                    qualifiedCodes.push_back(tsCode);
                }
                
                processedCount++;
                if (processedCount % 500 == 0) {
                    updatePhase1Progress(-1, processedCount, static_cast<int>(qualifiedCodes.size()), "");
                }
            }
            
            LOG_INFO("处理了 " + std::to_string(processedCount) + " 只股票的估值数据");
        }
        
        updatePhase1Progress(-1, processedCount, static_cast<int>(qualifiedCodes.size()), "completed");
        
        LOG_INFO("========================================");
        LOG_INFO("基本面筛选完成");
        LOG_INFO("  优质股票数量: " + std::to_string(qualifiedCodes.size()));
        LOG_INFO("========================================");
        
        return qualifiedCodes;
        
    } catch (const std::exception& e) {
        LOG_ERROR("基本面筛选失败: " + std::string(e.what()));
        updatePhase1Progress(-1, -1, -1, "completed");
        return {};
    }
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
    data::ChartDataDAO& chartDataDao,
    data::AnalysisProcessRecordDAO& processRecordDao
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
            
            // 计算并保存图表数据（仅当有买入信号时）
            try {
                // 检查是否有买入信号
                bool hasBuySignal = false;
                for (const auto& pair : strategyResults) {
                    if (pair.second.has_value()) {
                        auto result = *pair.second;
                        if (result.opt == "buy") {
                            hasBuySignal = true;
                            break;
                        }
                    }
                }
                
                if (hasBuySignal && data.size() >= 17) {
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
    
    // 保存分析过程数据（每只股票一条记录，包含所有策略和频率）
    {
        data::StockProcessRecord processRecord;
        processRecord.ts_code = stock.ts_code;
        processRecord.stock_name = stock.name;
        processRecord.trade_date = calculateAnalysisDate();
        
        auto strategies = strategyManager.getStrategies();
        for (const auto& strategy : strategies) {
            data::StrategyData strategyData;
            strategyData.name = strategy->getName();
            
            for (const auto& freq : {"d", "w", "m"}) {
                data::StrategyFreqData freqData;
                freqData.freq = freq;
                freqData.signal = "NONE";
                
                try {
                    auto data = dataSource->getQuoteData(stock.ts_code, "", "", freq);
                    
                    if (!data.empty()) {
                        auto result = strategy->analyze(stock.ts_code, data);
                        if (result.has_value()) {
                            if (result->label == "买入") freqData.signal = "BUY";
                            else if (result->label == "卖出") freqData.signal = "SELL";
                            else freqData.signal = "HOLD";
                        }
                        
                        if (data.size() >= 10) {
                            std::vector<double> closePrices;
                            for (const auto& d : data) closePrices.push_back(d.close);
                            
                            auto ema17Values = analysis::EMA::compute(closePrices, 17);
                            auto ema25Values = analysis::EMA::compute(closePrices, 25);
                            
                            int candleCount;
                            if (std::string(freq) == "m") {
                                candleCount = 20;
                            } else if (std::string(freq) == "w") {
                                candleCount = 100;
                            } else {
                                candleCount = 500;
                            }
                            
                            size_t startIdx = (data.size() > static_cast<size_t>(candleCount)) 
                                ? data.size() - candleCount : 0;
                            
                            for (size_t j = startIdx; j < data.size(); ++j) {
                                data::ProcessDataPoint point;
                                point.time = data[j].trade_date;
                                point.open = data[j].open;
                                point.high = data[j].high;
                                point.low = data[j].low;
                                point.close = data[j].close;
                                point.volume = data[j].volume;
                                
                                size_t emaIdx = j - startIdx;
                                size_t emaOffset = ema17Values.size() > (data.size() - startIdx) 
                                    ? ema17Values.size() - (data.size() - startIdx) : 0;
                                if (emaIdx + emaOffset < ema17Values.size()) {
                                    point.ema17 = ema17Values[emaIdx + emaOffset];
                                }
                                if (emaIdx + emaOffset < ema25Values.size()) {
                                    point.ema25 = ema25Values[emaIdx + emaOffset];
                                }
                                
                                freqData.candles.push_back(point);
                            }
                        }
                    }
                } catch (...) {}
                
                strategyData.freqs.push_back(freqData);
            }
            
            processRecord.strategies.push_back(strategyData);
        }
        
        processRecordDao.upsert(processRecord);
    }
}

/**
 * @brief 更新分析进度
 * @param total 总数（-1 表示不更新）
 * @param completed 已完成（-1 表示不更新）
 * @param failed 失败数（-1 表示不更新）
 * @param status 状态（空表示不更新）
 */
void updatePhase1Progress(int total = -1, int completed = -1, int qualified = -1, const std::string& status = "") {
    auto& conn = data::Connection::getInstance();
    if (!conn.isConnected()) return;
    
    std::string sql = "UPDATE analysis_progress SET updated_at = CURRENT_TIMESTAMP";
    if (total >= 0) sql += ", phase1_total = " + std::to_string(total);
    if (completed >= 0) sql += ", phase1_completed = " + std::to_string(completed);
    if (qualified >= 0) sql += ", phase1_qualified = " + std::to_string(qualified);
    if (!status.empty()) sql += ", phase1_status = '" + status + "'";
    if (status == "running") sql += ", started_at = CURRENT_TIMESTAMP";
    if (status == "completed") sql += ", phase1_completed_at = CURRENT_TIMESTAMP";
    sql += " WHERE id = 1";
    
    conn.execute(sql);
}

void updatePhase2Progress(int total = -1, int completed = -1, int failed = -1, const std::string& status = "") {
    auto& conn = data::Connection::getInstance();
    if (!conn.isConnected()) return;
    
    std::string sql = "UPDATE analysis_progress SET updated_at = CURRENT_TIMESTAMP";
    if (total >= 0) sql += ", phase2_total = " + std::to_string(total);
    if (completed >= 0) sql += ", phase2_completed = " + std::to_string(completed);
    if (failed >= 0) sql += ", phase2_failed = " + std::to_string(failed);
    if (!status.empty()) sql += ", phase2_status = '" + status + "'";
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
    data::ChartDataDAO& chartDataDao,
    data::AnalysisProcessRecordDAO& processRecordDao
) {
    LOG_INFO("========================================");
    LOG_INFO("开始技术面分析，共 " + std::to_string(stockList.size()) + " 只股票");
    LOG_INFO("========================================");
    
    int total = static_cast<int>(stockList.size());
    updatePhase2Progress(total, 0, 0, "running");
    
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
                analyzeStock(stock, localDataSource, strategyManager, analysisResultDao, chartDataDao, processRecordDao);
                int completed = successCount.fetch_add(1) + 1 + failCount.load();
                if (completed - lastReportedProgress.load() >= 10) {
                    updatePhase2Progress(-1, completed, failCount.load());
                    lastReportedProgress = completed;
                }
            } catch (const std::exception& e) {
                LOG_ERROR("分析失败: " + stock.ts_code + " - " + std::string(e.what()));
                int failed = failCount.fetch_add(1) + 1;
                int completed = successCount.load() + failed;
                updatePhase2Progress(-1, completed, failed);
            }
        });
        
        std::this_thread::sleep_for(std::chrono::milliseconds(delay_between_tasks));
    }
    
    pool.wait();
    
    updatePhase2Progress(-1, successCount.load(), failCount.load(), "completed");
    
    LOG_INFO("========================================");
    LOG_INFO("技术面分析完成");
    LOG_INFO("  成功: " + std::to_string(successCount.load()) + " 只");
    LOG_INFO("  失败: " + std::to_string(failCount.load()) + " 只");
    LOG_INFO("========================================");
}

/**
 * @brief 执行每日分析流程
 * 先基本面筛选，后技术分析
 * @param stockList 全市场股票列表
 * @param dataSource 数据源
 * @param strategyManager 策略管理器
 * @param analysisResultDao 分析结果 DAO
 * @param chartDataDao 图表数据 DAO
 * @param processRecordDao 过程记录 DAO
 */
void performDailyAnalysis(
    const std::vector<core::Stock>& stockList,
    std::shared_ptr<network::IDataSource> dataSource,
    core::StrategyManager& strategyManager,
    data::AnalysisResultDAO& analysisResultDao,
    data::ChartDataDAO& chartDataDao,
    data::AnalysisProcessRecordDAO& processRecordDao
) {
    LOG_INFO("========================================");
    LOG_INFO("开始每日分析流程");
    LOG_INFO("========================================");
    
    std::string analysisDate = calculateAnalysisDate();
    
    std::vector<core::Stock> analysisStockList;
    
    auto qualifiedCodes = performFundamentalScreening(dataSource, stockList, analysisDate);
    
    if (qualifiedCodes.empty()) {
        LOG_WARN("基本面筛选未返回优质股票，降级到全市场分析");
        analysisStockList = stockList;
    } else {
        LOG_INFO("基本面筛选成功，优质股票: " + std::to_string(qualifiedCodes.size()) + " 只");
        
        std::set<std::string> qualifiedSet(qualifiedCodes.begin(), qualifiedCodes.end());
        
        for (const auto& stock : stockList) {
            if (qualifiedSet.count(stock.ts_code) > 0) {
                analysisStockList.push_back(stock);
            }
        }
        
        LOG_INFO("从股票列表中匹配到 " + std::to_string(analysisStockList.size()) + " 只优质股票");
    }
    
    if (analysisStockList.empty()) {
        LOG_WARN("分析股票列表为空，跳过技术分析");
        return;
    }
    
    LOG_INFO("开始技术分析，股票数量: " + std::to_string(analysisStockList.size()));
    
    performBatchAnalysis(analysisStockList, dataSource, strategyManager, analysisResultDao, chartDataDao, processRecordDao);
    
    LOG_INFO("========================================");
    LOG_INFO("每日分析流程完成");
    LOG_INFO("========================================");
}

/**
 * @brief 清理系统资源
 */
void cleanup() {
    eventbus::shutdown();
    
    auto& conn = data::Connection::getInstance();
    conn.disconnect();
    
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
        data::AnalysisProcessRecordDAO processRecordDao;
        auto dataSource = network::DataSourceFactory::createFromConfig();
        auto& strategyManager = core::StrategyManager::getInstance();
        
        // 6. 加载股票列表
        auto stockList = loadStockList(dataSource);
        
        // 7. 根据模式执行
        if (options.onceMode) {
            // 单次执行模式
            LOG_INFO("执行单次分析...");
            performDailyAnalysis(stockList, dataSource, strategyManager, analysisResultDao, chartDataDao, processRecordDao);
        } else {
            scheduler::Scheduler sched(options.executeTime, [&]() {
                performDailyAnalysis(stockList, dataSource, strategyManager, analysisResultDao, chartDataDao, processRecordDao);
            });
            
            g_scheduler = &sched;
            
            if (checkStartupAnalysisNeeded(sched, processRecordDao)) {
                performDailyAnalysis(stockList, dataSource, strategyManager, analysisResultDao, chartDataDao, processRecordDao);
            }
            
            LOG_INFO("启动定时调度模式，执行时间: " + options.executeTime);
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
