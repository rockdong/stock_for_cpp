#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>

// 前置声明
namespace config { class Config; }
namespace logger { class LoggerManager; }
namespace data { class Connection; }
namespace network { class IDataSource; }
namespace core { 
    class Stock;
    class StrategyManager;
    class AnalysisResult;
}
namespace scheduler { class Scheduler; }

namespace stock_core {

/**
 * @brief 核心库版本信息
 */
constexpr const char* VERSION = "2.0.0";
constexpr const char* LIBRARY_NAME = "stock_core";

/**
 * @brief 进度回调函数类型
 * @param total 总数
 * @param completed 已完成数
 * @param failed 失败数
 * @param status 状态描述
 */
using ProgressCallback = std::function<void(int total, int completed, int failed, const std::string& status)>;

/**
 * @brief 核心库上下文 - 持有所有模块实例
 * 
 * 内部实现类,不暴露给外部
 */
class StockCoreContext;

/**
 * @brief 初始化核心库
 * 
 * 初始化所有内部模块:
 * - 配置系统 (Config)
 * - 日志系统 (Logger)
 * - 数据库连接 (Connection)
 * - 数据源 (DataSource)
 * - 策略管理器 (StrategyManager)
 * - EventBus
 * 
 * @param configPath 配置文件路径 (默认: ".env")
 * @return 是否成功初始化
 * 
 * @example
 * ```cpp
 * if (!stock_core::initialize(".env")) {
 *     std::cerr << "核心库初始化失败" << std::endl;
 *     return 1;
 * }
 * ```
 */
bool initialize(const std::string& configPath = ".env");

/**
 * @brief 清理核心库资源
 * 
 * 按顺序清理所有模块:
 * - EventBus
 * - 数据库连接
 * - 日志系统
 * 
 * @example
 * ```cpp
 * stock_core::shutdown();
 * ```
 */
void shutdown();

/**
 * @brief 获取核心上下文
 * 
 * 获取内部上下文指针,用于访问各个模块
 * 
 * @return 上下文指针 (初始化后有效,否则为 nullptr)
 * 
 * @warning 必须先调用 initialize() 才能获取有效上下文
 */
StockCoreContext* getContext();

/**
 * @brief 业务逻辑 API
 * 
 * 提供统一的业务接口,供客户端调用
 */
namespace api {

    /**
     * @brief 获取配置管理器
     * @return 配置管理器引用
     */
    config::Config& getConfig();

    /**
     * @brief 获取数据库连接
     * @return 数据库连接引用
     */
    data::Connection& getDatabase();

    /**
     * @brief 获取数据源
     * @return 数据源智能指针
     */
    std::shared_ptr<network::IDataSource> getDataSource();

    /**
     * @brief 获取策略管理器
     * @return 策略管理器引用
     */
    core::StrategyManager& getStrategyManager();

    /**
     * @brief 获取调度器
     * @return 调度器引用
     */
    scheduler::Scheduler& getScheduler();

    /**
     * @brief 加载股票列表
     * 
     * 从数据源或数据库加载所有股票基本信息
     * 
     * @return 股票列表
     */
    std::vector<core::Stock> loadStockList();

    /**
     * @brief 从 CSV 文件导入股票列表
     * 
     * 从 CSV 文件读取股票数据并保存到数据库
     * CSV 格式：ts_code,name,industry,list_date,market
     * 
     * @param csvFilePath CSV 文件路径
     * @return 导入的股票数量
     * @throws std::runtime_error 如果文件不存在或格式错误
     * 
     * @example
     * ```cpp
     * int imported = stock_core::api::importStocksFromCSV("data/sample_stocks.csv");
     * std::cout << "成功导入 " << imported << " 只股票" << std::endl;
     * ```
     */
    int importStocksFromCSV(const std::string& csvFilePath);

    /**
     * @brief 执行每日分析流程
     * 
     * 对给定的股票列表执行完整的分析流程:
     * - 技术分析 (所有策略、所有频率)
     * - 结果保存 (数据库)
     * 
     * @param stocks 股票列表
     */
    void performDailyAnalysis(const std::vector<core::Stock>& stocks);

    /**
     * @brief 运行定时调度器
     * 
     * 启动定时调度模式,每天在指定时间执行分析
     * 
     * @param executeTime 执行时间 (格式: "HH:MM", 例如 "20:00")
     */
    void runScheduler(const std::string& executeTime);

    /**
     * @brief 设置进度回调
     * 
     * 设置进度回调函数,用于监控分析进度
     * 
     * @param callback 回调函数
     */
    void setProgressCallback(ProgressCallback callback);

} // namespace api

} // namespace stock_core