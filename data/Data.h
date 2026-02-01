#ifndef DATA_H
#define DATA_H

/**
 * @file Data.h
 * @brief 数据层统一头文件
 * 
 * 数据层提供数据持久化和访问功能，包括：
 * - 数据库访问（SQLite）
 * - 内存缓存（LRU 策略）
 * - 文件存储（CSV 读写）
 * 
 * 命名空间：data::
 * 
 * 使用示例：
 * @code
 * #include "Data.h"
 * 
 * // 数据库操作
 * auto& conn = data::Connection::getInstance();
 * conn.initialize("stock.db");
 * conn.connect();
 * 
 * data::StockDAO stockDao;
 * auto stocks = stockDao.findAll();
 * 
 * // 缓存操作
 * data::MemoryCache cache(1000);
 * cache.set("key", std::string("value"), 300);
 * auto value = cache.get("key");
 * 
 * // CSV 操作
 * data::CSVReader reader;
 * reader.open("data.csv");
 * auto rows = reader.readAll();
 * 
 * data::CSVWriter writer;
 * writer.open("output.csv");
 * writer.writeHeader({"列1", "列2", "列3"});
 * writer.writeLine({"值1", "值2", "值3"});
 * @endcode
 */

// 数据库访问
#include "database/Connection.h"
#include "database/StockDAO.h"
#include "database/PriceDAO.h"

// 缓存管理
#include "cache/ICache.h"
#include "cache/MemoryCache.h"

// 文件存储
#include "file/CSVReader.h"
#include "file/CSVWriter.h"

// 前向声明
namespace network {
    struct StockBasic;
}

namespace data {

/**
 * @brief 将 network::StockBasic 转换为 data::Stock
 * @param stockBasic 网络层的股票基本信息
 * @return 数据层的股票对象
 */
Stock convertFromStockBasic(const network::StockBasic& stockBasic);

/**
 * @brief 批量转换 network::StockBasic 为 data::Stock
 * @param stockBasics 网络层的股票基本信息列表
 * @return 数据层的股票对象列表
 */
std::vector<Stock> convertFromStockBasics(const std::vector<network::StockBasic>& stockBasics);

/**
 * @brief 初始化数据层
 * @param dbPath 数据库文件路径
 * @param cacheSize 缓存大小
 * @return 是否成功
 */
inline bool initialize(const std::string& dbPath, size_t cacheSize = 1000) {
    auto& conn = Connection::getInstance();
    if (!conn.initialize(dbPath)) {
        return false;
    }
    return conn.connect();
}

/**
 * @brief 关闭数据层
 */
inline void shutdown() {
    auto& conn = Connection::getInstance();
    conn.disconnect();
}

} // namespace data

#endif // DATA_H

