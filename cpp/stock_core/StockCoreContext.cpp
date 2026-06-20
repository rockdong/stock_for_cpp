#include "StockCoreContext.h"
#include "version.h"
#include "../utils/ThreadPool.h"
#include "../core/StrategyEmojiMapper.h"
#include "../indicators/EMA.h"
#include "../util/CSVParser.h"  // CSV 解析器
#include <stdexcept>
#include <thread>
#include <atomic>
#include <chrono>

namespace stock_core {

// ========== 单例实现 ==========

StockCoreContext& StockCoreContext::getInstance() {
    static StockCoreContext instance;
    return instance;
}

// ========== 初始化 ==========

bool StockCoreContext::initialize(const std::string& configPath) {
    if (initialized_) {
        // 注意：logger 可能已初始化，可以安全使用
        LOG_WARN("核心库已经初始化,跳过重复初始化");
        return true;
    }

    // 1. 先初始化配置系统（加载 .env 文件到环境变量）
    // 这样 logger 初始化时才能正确读取 LOG_CONSOLE_ENABLED 等配置
    if (!initializeConfig(configPath)) {
        std::cerr << "配置系统初始化失败" << std::endl;
        return false;
    }

    // 2. 再初始化日志系统（此时环境变量已加载，LogConfig 能正确读取配置）
    if (!initializeLogger()) {
        // logger 初始化失败，只能用 std::cerr 输出
        std::cerr << "日志系统初始化失败" << std::endl;
        return false;
    }

    // 现在可以安全使用 logger 输出初始化信息
    LOG_INFO("========================================");
    LOG_INFO("Stock Core v" << VERSION_STRING << " 初始化开始");
    LOG_INFO("========================================");

    // 3. 初始化 EventBus
    if (!initializeEventBus()) {
        LOG_ERROR("EventBus 初始化失败");
        return false;
    }

    // 打印系统信息
    printSystemInfo();

    // 4. 初始化数据库
    if (!initializeDatabase()) {
        return false;
    }

    // 5. 初始化策略管理器
    if (!initializeStrategies()) {
        return false;
    }

    // 6. 初始化数据源
    if (!initializeDataSource()) {
        return false;
    }

    // 7. 初始化 DAOs
    if (!initializeDAOs()) {
        return false;
    }

    initialized_ = true;

    LOG_INFO("========================================");
    LOG_INFO("核心库初始化完成");
    LOG_INFO("========================================");

    return true;
}

void StockCoreContext::shutdown() {
    if (!initialized_) {
        return;
    }

    LOG_INFO("========================================");
    LOG_INFO("核心库资源清理开始");
    LOG_INFO("========================================");

    // 1. 清理调度器
    if (scheduler_) {
        scheduler_->stop();
        delete scheduler_;
        scheduler_ = nullptr;
        LOG_INFO("调度器已停止");
    }

    // 2. 清理 DAOs
    if (processRecordDao_) {
        delete processRecordDao_;
        processRecordDao_ = nullptr;
    }
    if (chartDataDao_) {
        delete chartDataDao_;
        chartDataDao_ = nullptr;
    }
    if (analysisResultDao_) {
        delete analysisResultDao_;
        analysisResultDao_ = nullptr;
    }
    if (stockDao_) {
        delete stockDao_;
        stockDao_ = nullptr;
    }
    LOG_INFO("DAOs 已清理");

    // 3. 清理策略管理器（单例，不 delete）
    strategyManager_ = nullptr;
    LOG_INFO("策略管理器引用已释放");

    // 4. 清理数据源
    dataSource_.reset();
    LOG_INFO("数据源已清理");

    // 5. 清理数据库连接（单例，只 disconnect，不 delete）
    if (database_) {
        database_->disconnect();
        database_ = nullptr;
        LOG_INFO("数据库连接已关闭");
    }

#ifdef HAS_MYSQL
    if (mysqlDatabase_) {
        mysqlDatabase_->disconnect();
        delete mysqlDatabase_;
        mysqlDatabase_ = nullptr;
    }
#endif

    // 6. 清理 EventBus（单例，不需要手动 shutdown）
    // EventBus 由 EventBusManager::getInstance() 管理
    LOG_INFO("EventBus 引用已释放");

    // 7. 清理日志系统
    logger::LoggerManager::getInstance().shutdown();
    LOG_INFO("日志系统已关闭");

    // 8. 清理配置（单例，不 delete）
    config_ = nullptr;

    initialized_ = false;

    LOG_INFO("========================================");
    LOG_INFO("核心库资源清理完成");
    LOG_INFO("========================================");
}

// ========== 初始化辅助函数 ==========

bool StockCoreContext::initializeConfig(const std::string& configPath) {
    try {
        // 使用单例模式，获取引用而不是 new
        config_ = &config::Config::getInstance();
        if (!config_->initialize(configPath.c_str())) {
            LOG_ERROR("配置系统初始化失败");
            return false;
        }
        LOG_INFO("配置系统初始化成功");
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR(std::string("配置系统初始化异常: ") + e.what());
        return false;
    }
}

bool StockCoreContext::initializeLogger() {
    try {
        logger::init();
        LOG_INFO("日志系统初始化成功");
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR(std::string("日志系统初始化异常: ") + e.what());
        return false;
    }
}

bool StockCoreContext::initializeEventBus() {
    try {
        // EventBus 由 EventBusManager 单例管理，初始化即获取实例
        auto& eventBusManager = eventbus::EventBusManager::getInstance();
        LOG_INFO("EventBus 初始化成功");
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR(std::string("EventBus 初始化异常: ") + e.what());
        return false;
    }
}

bool StockCoreContext::initializeDatabase() {
    try {
        auto& factory = data::ConnectionFactory::createConnection();
        database_ = &data::Connection::getInstance();

#ifdef HAS_MYSQL
        if (config_->getDbType() == "mysql") {
            mysqlDatabase_ = &data::MySQLConnection::getInstance();
            if (!mysqlDatabase_->connect()) {
                LOG_ERROR("MySQL 数据库连接失败");
                return false;
            }
            LOG_INFO("MySQL 数据库连接成功");
        } else {
#endif
            if (!database_->connect()) {
                LOG_ERROR("SQLite 数据库连接失败");
                return false;
            }
            LOG_INFO("SQLite 数据库连接成功");
#ifdef HAS_MYSQL
        }
#endif

        LOG_INFO("数据库表创建完成");
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR(std::string("数据库初始化异常: ") + e.what());
        return false;
    }
}

bool StockCoreContext::initializeStrategies() {
    try {
        // 使用单例模式，获取引用而不是 new
        strategyManager_ = &core::StrategyManager::getInstance();

        // 从 Config 读取策略配置（而不是环境变量）
        std::string strategiesConfig = config_->getStrategies();

        if (strategiesConfig.empty()) {
            LOG_WARN("未配置策略列表，使用默认策略");
            strategiesConfig = "EMA25_GREATER_17_PRICE_MATCH";  // 默认策略
        }

        if (!strategyManager_->initialize(strategiesConfig)) {
            LOG_ERROR("策略管理器初始化失败");
            return false;
        }

        LOG_INFO("策略管理器初始化成功，已注册 "
                 << strategyManager_->count() << " 个策略:");

        for (const auto& name : strategyManager_->getStrategyNames()) {
            LOG_INFO(std::string("  - ") + name);
        }

        return true;
    } catch (const std::exception& e) {
        LOG_ERROR(std::string("策略管理器初始化异常: ") + e.what());
        return false;
    }
}

bool StockCoreContext::initializeDataSource() {
    try {
        // 检查 API 密钥是否配置（从环境变量和配置文件）
        const char* apiKeyEnv = std::getenv("DATA_SOURCE_API_KEY");
        std::string apiKeyConfig = config_->getDataSourceApiKey();
        
        if ((!apiKeyEnv || strlen(apiKeyEnv) == 0) && apiKeyConfig.empty()) {
            LOG_WARN("========================================");
            LOG_WARN("DATA_SOURCE_API_KEY 未配置");
            LOG_WARN("数据源功能将不可用");
            LOG_WARN("请设置环境变量: export DATA_SOURCE_API_KEY=your_key");
            LOG_WARN("或在 .env 文件中配置: DATA_SOURCE_API_KEY=your_key");
            LOG_WARN("获取API密钥: https://tushare.pro");
            LOG_WARN("========================================");
            
            // 设置 dataSource_ 为 nullptr，但允许系统继续初始化
            dataSource_ = nullptr;
            return true;  // 返回 true，不阻止初始化
        }
        
        dataSource_ = network::DataSourceFactory::createFromConfig();
        if (!dataSource_) {
            LOG_ERROR("数据源初始化失败");
            // 允许系统在没有数据源的情况下启动（降级模式）
            dataSource_ = nullptr;
            return true;
        }
        LOG_INFO("数据源初始化成功");
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR(std::string("数据源初始化异常: ") + e.what());
        LOG_WARN("系统将在降级模式下运行（无数据源）");
        dataSource_ = nullptr;
        return true;  // 允许系统继续初始化
    }
}

bool StockCoreContext::initializeDAOs() {
    try {
        stockDao_ = new data::StockDAO();
        analysisResultDao_ = new data::AnalysisResultDAO();
        chartDataDao_ = new data::ChartDataDAO();
        processRecordDao_ = new data::AnalysisProcessRecordDAO();

        LOG_INFO("DAOs 初始化成功");
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR(std::string("DAOs 初始化异常: ") + e.what());
        return false;
    }
}

void StockCoreContext::printSystemInfo() {
    LOG_INFO("========================================");
    LOG_INFO("应用名称: " << config_->getAppName());
    LOG_INFO(std::string("应用版本: ") + VERSION_STRING + "." + std::to_string(VERSION_BUILD));
    LOG_INFO("运行环境: " << config_->getAppEnv());
    LOG_INFO(std::string("构建日期: ") + BUILD_DATE);
    LOG_INFO(std::string("Git 提交: ") + GIT_COMMIT);
    LOG_INFO("========================================");
    
    LOG_INFO("数据库配置:");
    LOG_INFO(std::string("  类型: ") + config_->getDbType());
    if (config_->getDbType() == "mysql") {
        LOG_INFO("  主机: " << config_->getDbHost());
        LOG_INFO("  端口: " << config_->getDbPort());
        LOG_INFO("  数据库: " << config_->getDbName());
        LOG_INFO("  用户: " << config_->getDbUser());
    } else {
        LOG_INFO("  文件: " << config_->getDbName() << ".db");
    }
    
    LOG_INFO("数据源配置:");
    LOG_INFO("  URL: " << config_->getDataSourceUrl());
    LOG_INFO("  超时: " << config_->getDataSourceTimeout() << "秒");
    
    LOG_INFO("策略配置:");
    LOG_INFO("  策略列表: " << config_->getStrategies());
    LOG_INFO("========================================");
}

// ========== 模块访问接口 ==========

config::Config& StockCoreContext::getConfig() {
    if (!initialized_ || !config_) {
        throw std::runtime_error("核心库未初始化或配置模块无效");
    }
    return *config_;
}

data::Connection& StockCoreContext::getDatabase() {
    if (!initialized_ || !database_) {
        throw std::runtime_error("核心库未初始化或数据库模块无效");
    }
    return *database_;
}

std::shared_ptr<network::IDataSource> StockCoreContext::getDataSource() {
    if (!initialized_ || !dataSource_) {
        throw std::runtime_error("核心库未初始化或数据源模块无效");
    }
    return dataSource_;
}

core::StrategyManager& StockCoreContext::getStrategyManager() {
    if (!initialized_ || !strategyManager_) {
        throw std::runtime_error("核心库未初始化或策略管理器无效");
    }
    return *strategyManager_;
}

scheduler::Scheduler& StockCoreContext::getScheduler() {
    if (!initialized_ || !scheduler_) {
        throw std::runtime_error("核心库未初始化或调度器无效");
    }
    return *scheduler_;
}

// ========== DAO 访问接口 ==========

data::StockDAO& StockCoreContext::getStockDAO() {
    if (!initialized_ || !stockDao_) {
        throw std::runtime_error("核心库未初始化或 StockDAO 无效");
    }
    return *stockDao_;
}

data::AnalysisResultDAO& StockCoreContext::getAnalysisResultDAO() {
    if (!initialized_ || !analysisResultDao_) {
        throw std::runtime_error("核心库未初始化或 AnalysisResultDAO 无效");
    }
    return *analysisResultDao_;
}

data::ChartDataDAO& StockCoreContext::getChartDataDAO() {
    if (!initialized_ || !chartDataDao_) {
        throw std::runtime_error("核心库未初始化或 ChartDataDAO 无效");
    }
    return *chartDataDao_;
}

data::AnalysisProcessRecordDAO& StockCoreContext::getProcessRecordDAO() {
    if (!initialized_ || !processRecordDao_) {
        throw std::runtime_error("核心库未初始化或 ProcessRecordDAO 无效");
    }
    return *processRecordDao_;
}

// ========== 业务流程实现 ==========

std::vector<core::Stock> StockCoreContext::loadStockList() {
    if (!initialized_) {
        throw std::runtime_error("核心库未初始化");
    }

    LOG_INFO("加载股票列表...");

    // 尝试从数据库加载
    auto stocks = stockDao_->findAll();

    if (stocks.empty()) {
        // 数据库为空，检查数据源是否可用
        if (!dataSource_) {
            LOG_WARN("数据库为空且数据源不可用（降级模式）");
            LOG_WARN("请配置 DATA_SOURCE_API_KEY 或手动导入股票数据");
            return {};  // 返回空列表，不阻止程序运行
        }

        // 从 API 获取
        LOG_INFO("从数据源获取股票列表...");
        stocks = dataSource_->getStockList();
        LOG_INFO("获取到 " << stocks.size() << " 只股票");
    } else {
        LOG_INFO("从数据库加载 " << stocks.size() << " 只股票");
    }

    return stocks;
}

void StockCoreContext::performDailyAnalysis(const std::vector<core::Stock>& stocks) {
    if (!initialized_) {
        throw std::runtime_error("核心库未初始化");
    }

    LOG_INFO("========================================");
    LOG_INFO("开始每日分析流程，共 " << stocks.size() << " 只股票");
    LOG_INFO("========================================");

    if (stocks.empty()) {
        LOG_WARN("股票列表为空，跳过分析");
        
        if (progressCallback_) {
            progressCallback_(0, 0, 0, "股票列表为空");
        }
        return;
    }
    
    // 执行技术分析（多线程批量处理）
    performBatchAnalysis(stocks);
    
    LOG_INFO("========================================");
    LOG_INFO("每日分析流程完成");
    LOG_INFO("========================================");
    
    if (progressCallback_) {
        progressCallback_(stocks.size(), stocks.size(), 0, "完成");
    }
}

// ========== 辅助方法 ==========

std::string StockCoreContext::calculateAnalysisDate() {
    if (!initialized_) {
        throw std::runtime_error("核心库未初始化");
    }
    
    std::string switchTimeStr = config_->getAnalysisDateSwitchTime();
    
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

// ========== 业务逻辑实现 ==========

bool StockCoreContext::executeProgressUpdate(const std::string& sql) {
    if (!initialized_) {
        return false;
    }
    
    std::string dbType = config_->getDbType();
    
#ifdef HAS_MYSQL
    if (dbType == "mysql" && mysqlDatabase_) {
        if (!mysqlDatabase_->isConnected()) {
            LOG_ERROR("MySQL 未连接，无法更新进度");
            return false;
        }
        return mysqlDatabase_->execute(sql);
    }
#endif
    
    // SQLite 作为默认/备选
    if (!database_->isConnected()) {
        LOG_ERROR("SQLite 未连接，无法更新进度");
        return false;
    }
    return database_->execute(sql);
}

void StockCoreContext::updatePhase2Progress(int total, int completed, int failed, const std::string& status) {
    if (!initialized_) {
        return;
    }
    
    std::string dbType = config_->getDbType();
    std::string timestampFunc = (dbType == "mysql") ? "NOW()" : "CURRENT_TIMESTAMP";
    
    std::string sql = "UPDATE analysis_progress SET updated_at = " + timestampFunc;
    if (total >= 0) sql += ", phase2_total = " + std::to_string(total);
    if (completed >= 0) sql += ", phase2_completed = " + std::to_string(completed);
    if (failed >= 0) sql += ", phase2_failed = " + std::to_string(failed);
    if (!status.empty()) sql += ", phase2_status = '" + status + "'";
    sql += " WHERE id = 1";
    
    executeProgressUpdate(sql);
}

void StockCoreContext::analyzeStock(const core::Stock& stock) {
    if (!initialized_) {
        throw std::runtime_error("核心库未初始化");
    }

    // 检查数据源是否可用
    if (!dataSource_) {
        LOG_WARN("跳过分析 " + stock.ts_code + ": 数据源不可用（降级模式）");
        return;  // 直接返回，不执行分析
    }

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
            auto data = dataSource_->getQuoteData(stock.ts_code, "", "", freq);
            
            if (data.empty()) {
                LOG_WARN("  " + freqName + ": 无数据");
                continue;
            }
            
            // 使用所有策略分析
            auto strategyResults = strategyManager_->analyzeAll(stock.ts_code, data);
            
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
                    
                    chartDataDao_->save(chartData);
                    LOG_DEBUG("  " + freqName + ": 保存图表数据 " + std::to_string(chartData.candles.size()) + " 条");
                }
            } catch (const std::exception& e) {
                LOG_ERROR("  " + freqName + " 图表数据保存失败: " + std::string(e.what()));
            }
            
            // 收集有效结果
            for (const auto& pair : strategyResults) {
                if (pair.second.has_value()) {
                    auto result = *pair.second;
                    result.trade_date = analysisDate;
                    result.freq = freq;
                    result.opt = freq;
                    result.strategy_name = core::StrategyEmojiMapper::getEmoji(pair.first);
                    
                    results.push_back(result);
                    LOG_INFO("  " + freqName + " - " + result.strategy_name + ": " + result.label);
                }
            }
            
        } catch (const std::exception& e) {
            LOG_ERROR("  " + freqName + " 分析失败: " + std::string(e.what()));
        }
    }
    
    // 批量保存结果
    if (!results.empty()) {
        int saved = analysisResultDao_->batchInsert(results);
        LOG_INFO("保存 " + std::to_string(saved) + " 条分析结果");
    }
    
    // 保存分析过程数据
    {
        data::StockProcessRecord processRecord;
        processRecord.ts_code = stock.ts_code;
        processRecord.stock_name = stock.name;
        processRecord.trade_date = calculateAnalysisDate();
        
        auto strategies = strategyManager_->getStrategies();
        for (const auto& strategy : strategies) {
            data::StrategyData strategyData;
            strategyData.name = core::StrategyEmojiMapper::getEmoji(strategy->getName());
            
            for (const auto& freq : {"d", "w", "m"}) {
                data::StrategyFreqData freqData;
                freqData.freq = freq;
                freqData.signal = "NONE";
                
                try {
                    auto data = dataSource_->getQuoteData(stock.ts_code, "", "", freq);
                    
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
        
        processRecordDao_->upsert(processRecord);
    }
}

void StockCoreContext::performBatchAnalysis(const std::vector<core::Stock>& stockList) {
    if (!initialized_) {
        throw std::runtime_error("核心库未初始化");
    }

    // 检查数据源是否可用
    if (!dataSource_) {
        LOG_ERROR("数据源不可用（降级模式），无法执行批量分析");
        LOG_WARN("请配置 DATA_SOURCE_API_KEY 后重新启动");
        if (progressCallback_) {
            progressCallback_(stockList.size(), 0, stockList.size(), "数据源不可用");
        }
        return;  // 直接返回，不执行分析
    }

    LOG_INFO("========================================");
    LOG_INFO("开始技术面分析，共 " << stockList.size() << " 只股票");
    LOG_INFO("========================================");

    // 获取市场热度数据（用于暴涨预警）
    std::string analysisDate = calculateAnalysisDate();
    try {
        LOG_INFO("获取市场热度数据...");
        auto heatData = dataSource_->getMarketHeatData(analysisDate, 5);
        strategyManager_->setMarketHeatData(heatData);
        LOG_INFO("市场热度数据已注入: 涨停" << heatData.limit_up_count << "只, 板块" << heatData.hot_sectors.size() << "个");
    } catch (const std::exception& e) {
        LOG_WARN("获取市场热度数据失败: " << e.what() << "，将跳过市场热度检测");
        strategyManager_->clearMarketHeatData();
    }
    
    int total = static_cast<int>(stockList.size());
    updatePhase2Progress(total, 0, 0, "running");
    
    // 多线程处理
    unsigned int threadCount = std::thread::hardware_concurrency();
    if (threadCount == 0) threadCount = 4;
    LOG_INFO("使用 " << threadCount << " 个线程并发处理");
    
    ThreadPool pool(threadCount);
    
    std::atomic<int> successCount(0);
    std::atomic<int> failCount(0);
    std::atomic<int> lastReportedProgress(0);
    
    const int delay_between_tasks = 300;
    
    for (const auto& stock : stockList) {
        pool.enqueue([&, stock]() {
            try {
                // 为每个线程创建独立的数据源实例
                auto localDataSource = network::DataSourceFactory::createFromConfig();
                
                // 临时替换数据源
                auto originalDataSource = dataSource_;
                dataSource_ = localDataSource;
                
                analyzeStock(stock);
                
                // 恢复原数据源
                dataSource_ = originalDataSource;
                
                int completed = successCount.fetch_add(1) + 1 + failCount.load();
                
                // 更新进度
                if (progressCallback_) {
                    progressCallback_(total, completed, failCount.load(), stock.ts_code);
                }
                
                if (completed - lastReportedProgress.load() >= 10) {
                    updatePhase2Progress(-1, completed, failCount.load(), "running");
                    lastReportedProgress = completed;
                }
            } catch (const std::exception& e) {
                LOG_ERROR(std::string("分析失败: ") + stock.ts_code + " - " + e.what());
                int failed = failCount.fetch_add(1) + 1;
                int completed = successCount.load() + failed;
                
                if (progressCallback_) {
                    progressCallback_(total, completed, failed, "失败: " + stock.ts_code);
                }

                updatePhase2Progress(-1, completed, failed, "failed");
            }
        });
        
        std::this_thread::sleep_for(std::chrono::milliseconds(delay_between_tasks));
    }
    
    pool.wait();
    
    // 清除市场热度数据
    strategyManager_->clearMarketHeatData();
    
    updatePhase2Progress(-1, successCount.load(), failCount.load(), "completed");
    
    LOG_INFO("========================================");
    LOG_INFO("技术面分析完成");
    LOG_INFO("  成功: " << successCount.load() << " 只");
    LOG_INFO("  失败: " << failCount.load() << " 只");
    LOG_INFO("========================================");
}

void StockCoreContext::runScheduler(const std::string& executeTime) {
    if (!initialized_) {
        throw std::runtime_error("核心库未初始化");
    }

    if (!scheduler_) {
        scheduler_ = new scheduler::Scheduler(executeTime, schedulerCallback_);
    }

    LOG_INFO(std::string("启动定时调度模式，执行时间: ") + executeTime);
    scheduler_->run();
}

void StockCoreContext::setProgressCallback(ProgressCallback callback) {
    progressCallback_ = callback;
}

int StockCoreContext::importStocksFromCSV(const std::string& csvFilePath) {
    if (!initialized_) {
        throw std::runtime_error("核心库未初始化");
    }

    LOG_INFO("========================================");
    LOG_INFO("从 CSV 导入股票列表");
    LOG_INFO("========================================");
    LOG_INFO("CSV 文件路径: " << csvFilePath);

    try {
        // 1. 使用 CSVParser 解析 CSV 文件
        auto stocks = util::CSVParser::parseStockList(csvFilePath);

        if (stocks.empty()) {
            LOG_WARN("CSV 文件为空或解析失败");
            return 0;
        }

        LOG_INFO("解析到 " << stocks.size() << " 只股票");

        // 2. 使用 StockDAO 批量保存到数据库
        int savedCount = stockDao_->batchInsert(stocks);

        if (savedCount > 0) {
            LOG_INFO("✅ 成功导入 " << savedCount << " 只股票到数据库");
        } else {
            LOG_WARN("导入失败或所有股票已存在（可能重复）");
        }

        return savedCount;
    } catch (const std::exception& e) {
        LOG_ERROR("❌ CSV 导入异常: " << e.what());
        return -1;
    }
}

} // namespace stock_core