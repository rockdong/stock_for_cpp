#ifndef I_DATA_SOURCE_H
#define I_DATA_SOURCE_H

#include <string>
#include <vector>
#include <memory>

namespace network {

/**
 * @brief 股票数据结构
 */
struct StockData {
    std::string ts_code;        // 股票代码
    std::string trade_date;     // 交易日期
    double open;                // 开盘价
    double high;                // 最高价
    double low;                 // 最低价
    double close;               // 收盘价
    double pre_close;           // 昨收价
    double change;              // 涨跌额
    double pct_chg;             // 涨跌幅
    long volume;                // 成交量（手）
    double amount;              // 成交额（千元）
};

/**
 * @brief 股票基本信息
 */
struct StockBasic {
    std::string ts_code;        // TS代码（如 000001.SZ）
    std::string symbol;         // 股票代码（如 000001）
    std::string name;           // 股票名称
    std::string area;           // 地域
    std::string industry;       // 所属行业
    std::string fullname;       // 股票全称
    std::string enname;         // 英文全称
    std::string cnspell;        // 拼音缩写
    std::string market;         // 市场类型（主板/创业板/科创板等）
    std::string exchange;       // 交易所代码（SSE上交所 SZSE深交所）
    std::string curr_type;      // 交易货币
    std::string list_status;    // 上市状态（L上市 D退市 P暂停上市）
    std::string list_date;      // 上市日期
    std::string delist_date;    // 退市日期
    std::string is_hs;          // 是否沪深港通标的（N否 H沪股通 S深股通）
};

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
    virtual std::vector<StockBasic> getStockList() = 0;
    
    /**
     * @brief 获取股票列表（带参数）
     * @param list_status 上市状态（L上市 D退市 P暂停上市，默认L）
     * @param exchange 交易所（SSE上交所 SZSE深交所，默认为空表示全部）
     * @return 股票基本信息列表
     */
    virtual std::vector<StockBasic> getStockList(
        const std::string& list_status,
        const std::string& exchange
    ) = 0;
    
    /**
     * @brief 根据股票代码获取股票信息
     * @param ts_code 股票代码（如 000001.SZ）
     * @return 股票基本信息
     */
    virtual StockBasic getStockInfo(const std::string& ts_code) = 0;

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
};

using DataSourcePtr = std::shared_ptr<IDataSource>;

} // namespace network

#endif // I_DATA_SOURCE_H

