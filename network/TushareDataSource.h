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
     * @brief 获取股票列表
     * @return 股票基本信息列表
     */
    std::vector<StockBasic> getStockList() override;

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

