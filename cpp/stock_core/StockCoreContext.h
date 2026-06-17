#pragma once

#include <memory>
#include <string>
#include <vector>
#include <functional>

// 引入各模块头文件
#include "../config/Config.h"
#include "../log/Logger.h"
#include "../data/database/Connection.h"
#include "../data/database/ConnectionFactory.h"
#include "../data/database/StockDAO.h"
#include "../data/database/AnalysisResultDAO.h"
#include "../data/database/ChartDataDAO.h"
#include "../data/database/AnalysisProcessRecordDAO.h"
#include "../network/DataSourceFactory.h"
#include "../core/StrategyManager.h"
#include "../scheduler/Scheduler.h"
#include "../eventbus/EventBusManager.h"
#include "../utils/ThreadPool.h"
#include "../utils/TimeUtil.h"

namespace stock_core {

// 引入 ProgressCallback 类型定义
using ProgressCallback = std::function<void(int total, int completed, int failed, const std::string& status)>;

/**
 * @brief 核心库上下文 - 持有所有模块实例
 * 
 * 单例模式,全局唯一实例
 * 负责管理所有模块的生命周期和依赖关系
 */
class StockCoreContext {
public:
    /**
     * @brief 获取单例实例
     * @return 上下文实例引用
     */
    static StockCoreContext& getInstance();

    /**
     * @brief 初始化核心库
     * @param configPath 配置文件路径
     * @return 是否成功
     */
    bool initialize(const std::string& configPath);

    /**
     * @brief 清理资源
     */
    void shutdown();

    /**
     * @brief 检查是否已初始化
     * @return 是否已初始化
     */
    bool isInitialized() const { return initialized_; }

    // ========== 模块访问接口 ==========

    /**
     * @brief 获取配置管理器
     */
    config::Config& getConfig();

    /**
     * @brief 获取数据库连接
     */
    data::Connection& getDatabase();

    /**
     * @brief 获取数据源
     */
    std::shared_ptr<network::IDataSource> getDataSource();

    /**
     * @brief 获取策略管理器
     */
    core::StrategyManager& getStrategyManager();

    /**
     * @brief 获取调度器
     */
    scheduler::Scheduler& getScheduler();

    // ========== DAO 访问接口 ==========

    /**
     * @brief 获取股票 DAO
     */
    data::StockDAO& getStockDAO();

    /**
     * @brief 获取分析结果 DAO
     */
    data::AnalysisResultDAO& getAnalysisResultDAO();

    /**
     * @brief 获取图表数据 DAO
     */
    data::ChartDataDAO& getChartDataDAO();

    /**
     * @brief 获取过程记录 DAO
     */
    data::AnalysisProcessRecordDAO& getProcessRecordDAO();

    // ========== 业务流程接口 ==========

    /**
     * @brief 加载股票列表
     */
    std::vector<core::Stock> loadStockList();

    /**
     * @brief 执行每日分析
     */
    void performDailyAnalysis(const std::vector<core::Stock>& stocks);

    /**
     * @brief 运行调度器
     */
    void runScheduler(const std::string& executeTime);

    /**
     * @brief 设置进度回调
     */
    void setProgressCallback(ProgressCallback callback);

    /**
     * @brief 从 CSV 文件导入股票列表
     * @param csvFilePath CSV 文件路径
     * @return 成功导入的股票数量，-1 表示失败
     */
    int importStocksFromCSV(const std::string& csvFilePath);

private:
    // 私有构造函数 (单例)
    StockCoreContext() = default;
    ~StockCoreContext() = default;
    StockCoreContext(const StockCoreContext&) = delete;
    StockCoreContext& operator=(const StockCoreContext&) = delete;

    // ========== 初始化辅助函数 ==========

    /**
     * @brief 初始化配置系统
     */
    bool initializeConfig(const std::string& configPath);

    /**
     * @brief 初始化日志系统
     */
    bool initializeLogger();

    /**
     * @brief 初始化 EventBus
     */
    bool initializeEventBus();

    /**
     * @brief 初始化数据库
     */
    bool initializeDatabase();

    /**
     * @brief 初始化策略管理器
     */
    bool initializeStrategies();

    /**
     * @brief 初始化数据源
     */
    bool initializeDataSource();

    /**
     * @brief 初始化 DAO
     */
    bool initializeDAOs();

    /**
     * @brief 打印系统信息
     */
    void printSystemInfo();
    
    /**
     * @brief 计算分析日期
     * 考虑切换时间：当前时间 < 切换时间时，分析日期为昨天
     * @return 分析日期（格式：YYYYMMDD）
     */
    std::string calculateAnalysisDate();
    
    /**
     * @brief 分析单只股票
     * @param stock 股票信息
     */
    void analyzeStock(const core::Stock& stock);
    
    /**
     * @brief 执行批量分析（多线程版本）
     * @param stockList 股票列表
     */
    void performBatchAnalysis(const std::vector<core::Stock>& stockList);
    
    /**
     * @brief 执行进度更新 SQL
     * @param sql SQL 语句
     * @return 是否成功
     */
    bool executeProgressUpdate(const std::string& sql);
    
    /**
     * @brief 更新 Phase2 进度
     * @param total 总数
     * @param completed 已完成数
     * @param failed 失败数
     * @param status 状态
     */
    void updatePhase2Progress(int total, int completed, int failed, const std::string& status);

    // ========== 模块实例 ==========

    // 配置（单例引用）
    config::Config* config_ = nullptr;  // 使用 getInstance() 获取

    // 数据库连接（单例引用）
    data::Connection* database_ = nullptr;  // 使用 getInstance() 获取
#ifdef HAS_MYSQL
    data::MySQLConnection* mysqlDatabase_ = nullptr;
#endif

    // 数据源
    std::shared_ptr<network::IDataSource> dataSource_;

    // 策略管理器（单例引用）
    core::StrategyManager* strategyManager_ = nullptr;  // 使用 getInstance() 获取

    // 调度器
    scheduler::Scheduler* scheduler_;

    // DAO 实例
    data::StockDAO* stockDao_;
    data::AnalysisResultDAO* analysisResultDao_;
    data::ChartDataDAO* chartDataDao_;
    data::AnalysisProcessRecordDAO* processRecordDao_;

    // 进度回调
    ProgressCallback progressCallback_;

    // 初始化状态
    bool initialized_;

    // 调度器回调函数
    std::function<void()> schedulerCallback_;
};

} // namespace stock_core