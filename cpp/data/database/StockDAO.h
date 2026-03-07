#ifndef DATA_STOCK_DAO_H
#define DATA_STOCK_DAO_H

#include <string>
#include <vector>
#include <optional>
#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/sqlite3/sqlite3.h>
#include "StockTable.h"
#include "../../core/Stock.h"

namespace data {

// 使用核心模块的数据结构
using Stock = core::Stock;
using StockEntity = core::StockEntity;

/**
 * @brief 股票数据访问对象
 * 职责：提供股票数据的 CRUD 操作
 */
class StockDAO {
public:
    StockDAO() = default;
    ~StockDAO() = default;

    /**
     * @brief 插入股票
     * @param stock 股票对象
     * @return 是否成功
     */
    bool insert(const Stock& stock);

    /**
     * @brief 批量插入股票
     * @param stocks 股票列表
     * @return 成功插入的数量
     */
    int batchInsert(const std::vector<Stock>& stocks);

    /**
     * @brief 根据股票代码查找
     * @param ts_code 股票代码
     * @return 股票对象（可选）
     */
    std::optional<Stock> findByTsCode(const std::string& ts_code);

    /**
     * @brief 根据 ID 查找
     * @param id 股票 ID
     * @return 股票对象（可选）
     */
    std::optional<Stock> findById(int id);

    /**
     * @brief 查找所有股票
     * @return 股票列表
     */
    std::vector<Stock> findAll();

    /**
     * @brief 根据市场查找
     * @param market 市场类型
     * @return 股票列表
     */
    std::vector<Stock> findByMarket(const std::string& market);

    /**
     * @brief 根据行业查找
     * @param industry 行业
     * @return 股票列表
     */
    std::vector<Stock> findByIndustry(const std::string& industry);

    /**
     * @brief 更新股票
     * @param stock 股票对象
     * @return 是否成功
     */
    bool update(const Stock& stock);

    /**
     * @brief 删除股票
     * @param ts_code 股票代码
     * @return 是否成功
     */
    bool remove(const std::string& ts_code);

    /**
     * @brief 检查股票是否存在
     * @param ts_code 股票代码
     * @return 是否存在
     */
    bool exists(const std::string& ts_code);

    /**
     * @brief 获取股票总数
     * @return 股票数量
     */
    int count();
};

} // namespace data

#endif // DATA_STOCK_DAO_H

