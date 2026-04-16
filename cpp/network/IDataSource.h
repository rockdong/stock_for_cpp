#ifndef I_DATA_SOURCE_H
#define I_DATA_SOURCE_H

#include <string>
#include <vector>
#include <memory>
#include "../core/Stock.h"
#include "../core/FundamentalData.h"

namespace network {

// 使用核心模块的数据结构
using Stock = core::Stock;
using StockData = core::StockData;
using FinancialIndicator = core::FinancialIndicator;

/**
 * @brief 数据源接口（抽象基类）
 * 
 * 定义统一的数据获取接口，支持多种数据源实现
 * 遵循依赖倒置原则（DIP）
 */
class IDataSource {
public:
    virtual ~IDataSource() = default;

    /**
     * @brief 获取股票列表（默认获取所有上市股票）
     * @return 股票基本信息列表
     */
    virtual std::vector<Stock> getStockList() = 0;
    
    /**
     * @brief 获取股票列表（带参数）
     * @param list_status 上市状态（L上市 D退市 P暂停上市，默认L）
     * @param exchange 交易所（SSE上交所 SZSE深交所，默认为空表示全部）
     * @param market 市场（主板A股 创业板C股 科创板K股 北交所BB股，默认为空表示全部）
     * @return 股票基本信息列表
     */
    virtual std::vector<Stock> getStockList(
        const std::string& list_status,
        const std::string& exchange,
        const std::string& market
    ) = 0;
    
    /**
     * @brief 根据股票代码获取股票信息
     * @param ts_code 股票代码（如 000001.SZ）
     * @return 股票基本信息
     */
    virtual Stock getStockInfo(const std::string& ts_code) = 0;

    /**
     * @brief 获取股票日线数据
     * @param ts_code 股票代码
     * @param start_date 开始日期（YYYYMMDD）
     * @param end_date 结束日期（YYYYMMDD）
     * @return 股票数据列表
     */
    virtual std::vector<StockData> getDailyData(
        const std::string& ts_code,
        const std::string& start_date,
        const std::string& end_date
    ) = 0;

    /**
     * @brief 获取股票行情数据（支持日线、周线、月线）
     * @param ts_code 股票代码
     * @param start_date 开始日期（YYYYMMDD）
     * @param end_date 结束日期（YYYYMMDD）
     * @param freq 频率（d=日线, w=周线, m=月线）
     * @return 股票数据列表
     */
    virtual std::vector<StockData> getQuoteData(
        const std::string& ts_code,
        const std::string& start_date,
        const std::string& end_date,
        const std::string& freq = "d"
    ) = 0;

    /**
     * @brief 获取最新行情
     * @param ts_code 股票代码
     * @return 股票数据
     */
    virtual StockData getLatestQuote(const std::string& ts_code) = 0;

    /**
     * @brief 测试连接
     * @return 是否连接成功
     */
    virtual bool testConnection() = 0;

    /**
     * @brief 获取财务指标数据
     * @param ts_code 股票代码（可选，空表示全市场）
     * @param period 报告期（可选）
     * @return 财务指标列表
     */
    virtual std::vector<FinancialIndicator> getFinancialIndicators(
        const std::string& ts_code = "",
        const std::string& period = ""
    ) = 0;
};

using DataSourcePtr = std::shared_ptr<IDataSource>;

} // namespace network

#endif // I_DATA_SOURCE_H

