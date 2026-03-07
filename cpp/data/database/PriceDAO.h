#ifndef DATA_PRICE_DAO_H
#define DATA_PRICE_DAO_H

#include <string>
#include <vector>
#include <optional>
#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/sqlite3/sqlite3.h>
#include "PriceTable.h"

namespace data {

/**
 * @brief 价格实体类
 */
struct Price {
    int id = 0;
    int stock_id = 0;
    std::string trade_date;   // 交易日期
    double open = 0.0;        // 开盘价
    double high = 0.0;        // 最高价
    double low = 0.0;         // 最低价
    double close = 0.0;       // 收盘价
    double pre_close = 0.0;   // 前收盘价
    double change_pct = 0.0;  // 涨跌幅
    double volume = 0.0;      // 成交量
    double amount = 0.0;      // 成交额
};

/**
 * @brief 价格数据访问对象
 * 职责：提供价格数据的 CRUD 操作
 */
class PriceDAO {
public:
    PriceDAO() = default;
    ~PriceDAO() = default;

    /**
     * @brief 插入价格数据
     * @param price 价格对象
     * @return 是否成功
     */
    bool insert(const Price& price);

    /**
     * @brief 批量插入价格数据
     * @param prices 价格列表
     * @return 成功插入的数量
     */
    int batchInsert(const std::vector<Price>& prices);

    /**
     * @brief 根据股票和日期查找
     * @param stock_id 股票 ID
     * @param trade_date 交易日期
     * @return 价格对象（可选）
     */
    std::optional<Price> findByStockAndDate(int stock_id, const std::string& trade_date);

    /**
     * @brief 根据股票查找所有价格
     * @param stock_id 股票 ID
     * @return 价格列表
     */
    std::vector<Price> findByStock(int stock_id);

    /**
     * @brief 根据股票和日期范围查找
     * @param stock_id 股票 ID
     * @param start_date 开始日期
     * @param end_date 结束日期
     * @return 价格列表
     */
    std::vector<Price> findByStockAndDateRange(int stock_id, const std::string& start_date, const std::string& end_date);

    /**
     * @brief 查找股票最新价格
     * @param stock_id 股票 ID
     * @return 价格对象（可选）
     */
    std::optional<Price> findLatestByStock(int stock_id);

    /**
     * @brief 更新价格数据
     * @param price 价格对象
     * @return 是否成功
     */
    bool update(const Price& price);

    /**
     * @brief 删除价格数据
     * @param stock_id 股票 ID
     * @param trade_date 交易日期
     * @return 是否成功
     */
    bool remove(int stock_id, const std::string& trade_date);

    /**
     * @brief 删除股票的所有价格数据
     * @param stock_id 股票 ID
     * @return 删除的数量
     */
    int removeByStock(int stock_id);

    /**
     * @brief 统计股票的价格数据数量
     * @param stock_id 股票 ID
     * @return 数量
     */
    int countByStock(int stock_id);
};

} // namespace data

#endif // DATA_PRICE_DAO_H

