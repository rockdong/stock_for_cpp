#ifndef TUSHARE_DATA_SOURCE_H
#define TUSHARE_DATA_SOURCE_H

#include "IDataSource.h"
#include "http/TushareClient.h"
#include "../core/FundamentalData.h"
#include <memory>

namespace network {

/**
 * @brief Tushare 数据源实现
 * 
 * 实现 IDataSource 接口，提供 Tushare Pro 数据访问
 * 遵循依赖倒置原则（DIP）和里氏替换原则（LSP）
 * 
 * 从配置模块自动获取 URL 和 Token
 */
class TushareDataSource : public IDataSource {
public:
    /**
     * @brief 默认构造函数（从配置模块获取参数）
     */
    TushareDataSource();
    
    /**
     * @brief 构造函数（手动指定 Token）
     * @param api_token Tushare API Token
     */
    explicit TushareDataSource(const std::string& api_token);
    
    ~TushareDataSource() override = default;

    /**
     * @brief 获取股票列表（默认获取所有上市股票）
     * @return 股票基本信息列表
     */
    std::vector<Stock> getStockList() override;
    
    /**
     * @brief 获取股票列表（带参数）
     * @param list_status 上市状态（L上市 D退市 P暂停上市，默认L）
     * @param exchange 交易所（SSE上交所 SZSE深交所，默认为空表示全部）
     * @param market 市场（主板A股 创业板C股 科创板K股 北交所BB股，默认为空表示全部）
     * @return 股票基本信息列表
     */
    std::vector<Stock> getStockList(
        const std::string& list_status,
        const std::string& exchange,
        const std::string& market
    ) override;
    
    /**
     * @brief 根据股票代码获取股票信息
     * @param ts_code 股票代码（如 000001.SZ）
     * @return 股票基本信息
     */
    Stock getStockInfo(const std::string& ts_code) override;

    /**
     * @brief 获取股票日线数据
     * @param ts_code 股票代码
     * @param start_date 开始日期（YYYYMMDD）
     * @param end_date 结束日期（YYYYMMDD）
     * @return 股票数据列表
     */
    std::vector<StockData> getDailyData(
        const std::string& ts_code,
        const std::string& start_date,
        const std::string& end_date
    ) override;

    /**
     * @brief 获取股票行情数据（支持日线、周线、月线）
     * @param ts_code 股票代码
     * @param start_date 开始日期（YYYYMMDD）
     * @param end_date 结束日期（YYYYMMDD）
     * @param freq 频率（d=日线, w=周线, m=月线）
     * @return 股票数据列表
     */
    std::vector<StockData> getQuoteData(
        const std::string& ts_code,
        const std::string& start_date,
        const std::string& end_date,
        const std::string& freq = "d"
    ) override;

    /**
     * @brief 获取最新行情
     * @param ts_code 股票代码
     * @return 股票数据
     */
    StockData getLatestQuote(const std::string& ts_code) override;

    /**
     * @brief 测试连接
     * @return 是否连接成功
     */
    bool testConnection() override;

    /**
     * @brief 获取 Tushare 客户端（用于高级操作）
     * @return Tushare 客户端指针
     */
    TushareClient* getClient() { return client_.get(); }

    /**
     * @brief 获取财务指标数据
     * @param ts_code 股票代码（可选，为空表示获取全部）
     * @param period 报告期（可选，如 20231231）
     * @return 财务指标列表
     */
    std::vector<core::FinancialIndicator> getFinancialIndicators(
        const std::string& ts_code = "",
        const std::string& period = ""
    ) override;

    /**
     * @brief 获取资金流向数据
     * @param ts_code 股票代码（可选）
     * @param trade_date 交易日期（可选）
     * @param start_date 开始日期（可选）
     * @param end_date 结束日期（可选）
     * @return 资金流向列表
     */
    std::vector<MoneyFlow> getMoneyFlow(
        const std::string& ts_code = "",
        const std::string& trade_date = "",
        const std::string& start_date = "",
        const std::string& end_date = ""
    ) override;

    /**
     * @brief 获取停复牌信息
     * @param ts_code 股票代码（可选，空表示全市场）
     * @param start_date 开始日期（可选）
     * @param end_date 结束日期（可选）
     * @return 停复牌信息列表
     */
    std::vector<SuspendInfo> getSuspendInfo(
        const std::string& ts_code = "",
        const std::string& start_date = "",
        const std::string& end_date = ""
    ) override;

    /**
     * @brief 获取业绩预告
     * @param ts_code 股票代码（可选，空表示全市场）
     * @param period 报告期（可选）
     * @param start_date 开始日期（可选）
     * @param end_date 结束日期（可选）
     * @return 业绩预告列表
     */
    std::vector<Forecast> getForecast(
        const std::string& ts_code = "",
        const std::string& period = "",
        const std::string& start_date = "",
        const std::string& end_date = ""
    ) override;

    /**
     * @brief 获取业绩快报
     * @param ts_code 股票代码（可选，空表示全市场）
     * @param period 报告期（可选）
     * @param start_date 开始日期（可选）
     * @param end_date 结束日期（可选）
     * @return 业绩快报列表
     */
    std::vector<Express> getExpress(
        const std::string& ts_code = "",
        const std::string& period = "",
        const std::string& start_date = "",
        const std::string& end_date = ""
    ) override;

    /**
     * @brief 获取分红送股数据
     * @param ts_code 股票代码（可选，空表示全市场）
     * @param end_date 报告期/年度（可选）
     * @param start_date 开始日期（可选）
     * @param ann_date 公告日期（可选）
     * @return 分红送股列表
     */
    std::vector<Dividend> getDividend(
        const std::string& ts_code = "",
        const std::string& end_date = "",
        const std::string& start_date = "",
        const std::string& ann_date = ""
    ) override;

    /**
     * @brief 获取市场热度数据（涨停板+板块热度）
     * 用于暴涨预警策略
     * @param trade_date 交易日期（YYYYMMDD，可选，默认当日）
     * @param lookback_days 回溯天数（用于统计涨停次数，默认5天）
     * @return 市场热度数据
     */
    MarketHeatData getMarketHeatData(
        const std::string& trade_date = "",
        int lookback_days = 5
    ) override;

private:
    /**
     * @brief 解析股票基本信息
     * @param response Tushare 响应
     * @return 股票基本信息列表
     */
    std::vector<Stock> parseStockBasic(const TushareResponse& response);

    /**
     * @brief 解析股票数据
     * @param response Tushare 响应
     * @return 股票数据列表
     */
    std::vector<StockData> parseStockData(const TushareResponse& response);

    /**
     * @brief 解析财务指标数据
     * @param response Tushare 响应
     * @return 财务指标列表
     */
    std::vector<core::FinancialIndicator> parseFinancialIndicators(const TushareResponse& response);

    /**
     * @brief 解析资金流向数据
     * @param response Tushare 响应
     * @return 资金流向列表
     */
    std::vector<MoneyFlow> parseMoneyFlow(const TushareResponse& response);

    /**
     * @brief 解析停复牌信息
     * @param response Tushare 响应
     * @return 停复牌信息列表
     */
    std::vector<SuspendInfo> parseSuspendInfo(const TushareResponse& response);

    /**
     * @brief 解析业绩预告数据
     * @param response Tushare 响应
     * @return 业绩预告列表
     */
    std::vector<Forecast> parseForecast(const TushareResponse& response);

    /**
     * @brief 解析业绩快报数据
     * @param response Tushare 响应
     * @return 业绩快报列表
     */
    std::vector<Express> parseExpress(const TushareResponse& response);

    /**
     * @brief 解析分红送股数据
     * @param response Tushare 响应
     * @return 分红送股列表
     */
    std::vector<Dividend> parseDividend(const TushareResponse& response);

    /**
     * @brief 解析涨停板数据
     * @param response Tushare 响应
     * @return 涨停股票列表
     */
    std::vector<core::LimitListStock> parseLimitListStock(const TushareResponse& response);

    /**
     * @brief 解析板块热度数据
     * @param response Tushare 响应
     * @return 板块热度列表
     */
    std::vector<core::SectorHeat> parseSectorHeat(const TushareResponse& response);

    std::unique_ptr<TushareClient> client_;
};

} // namespace network

#endif // TUSHARE_DATA_SOURCE_H

