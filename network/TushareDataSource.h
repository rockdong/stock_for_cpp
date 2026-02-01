#ifndef TUSHARE_DATA_SOURCE_H
#define TUSHARE_DATA_SOURCE_H

#include "IDataSource.h"
#include "http/TushareClient.h"
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
    std::vector<StockBasic> getStockList() override;
    
    /**
     * @brief 获取股票列表（带参数）
     * @param list_status 上市状态（L上市 D退市 P暂停上市，默认L）
     * @param exchange 交易所（SSE上交所 SZSE深交所，默认为空表示全部）
     * @return 股票基本信息列表
     */
    std::vector<StockBasic> getStockList(
        const std::string& list_status,
        const std::string& exchange
    ) override;
    
    /**
     * @brief 根据股票代码获取股票信息
     * @param ts_code 股票代码（如 000001.SZ）
     * @return 股票基本信息
     */
    StockBasic getStockInfo(const std::string& ts_code) override;

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

private:
    /**
     * @brief 解析股票基本信息
     * @param response Tushare 响应
     * @return 股票基本信息列表
     */
    std::vector<StockBasic> parseStockBasic(const TushareResponse& response);

    /**
     * @brief 解析股票数据
     * @param response Tushare 响应
     * @return 股票数据列表
     */
    std::vector<StockData> parseStockData(const TushareResponse& response);

    std::unique_ptr<TushareClient> client_;
};

} // namespace network

#endif // TUSHARE_DATA_SOURCE_H

