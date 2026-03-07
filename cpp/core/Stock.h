#ifndef CORE_STOCK_H
#define CORE_STOCK_H

#include <string>

namespace core {

/**
 * @brief 股票基本信息（核心数据结构）
 * 
 * 这是系统中股票信息的统一数据结构，被以下模块使用：
 * - network 模块：从 API 获取数据
 * - data 模块：数据库存储
 * - core 模块：业务逻辑处理
 * 
 * 设计原则：
 * - 单一数据源：避免重复定义
 * - 业务字段：只包含业务相关字段
 * - 数据库无关：不包含数据库特定字段（如 id, created_at）
 */
struct Stock {
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
 * @brief 股票数据库实体（扩展核心结构）
 * 
 * 在核心 Stock 结构基础上，添加数据库特定字段
 * 用于数据持久化
 */
struct StockEntity {
    int id = 0;                 // 数据库主键
    Stock stock;                // 核心股票信息
    std::string created_at;     // 创建时间
    std::string updated_at;     // 更新时间
};

/**
 * @brief 股票行情数据
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

} // namespace core

#endif // CORE_STOCK_H

