#include "PriceDAO.h"
#include "ConnectionManager.h"
#include "Logger.h"

namespace data {

bool PriceDAO::insert(const Price& price) {
    auto& conn = ConnectionManager::getInstance().getConnection();
    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return false;
    }

    try {
        PriceTable prices;
        auto db = ConnectionManager::getInstance().getDb();
        
        (*db)(sqlpp::insert_into(prices).set(
            prices.stockId = price.stock_id,
            prices.tradeDate = price.trade_date,
            prices.open = price.open,
            prices.high = price.high,
            prices.low = price.low,
            prices.close = price.close,
            prices.preClose = price.pre_close,
            prices.changePct = price.change_pct,
            prices.volume = price.volume,
            prices.amount = price.amount
        ));
        
        LOG_DEBUG("插入价格数据成功: stock_id=" + std::to_string(price.stock_id) + ", date=" + price.trade_date);
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("插入价格数据失败: " + std::string(e.what()));
        return false;
    }
}

int PriceDAO::batchInsert(const std::vector<Price>& prices_vec) {
    auto& conn = ConnectionManager::getInstance().getConnection();
    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return 0;
    }

    conn.beginTransaction();
    int successCount = 0;

    for (const auto& price : prices_vec) {
        if (insert(price)) {
            successCount++;
        }
    }

    conn.commit();
    LOG_INFO("批量插入价格数据完成: " + std::to_string(successCount) + "/" + std::to_string(prices_vec.size()));
    return successCount;
}

std::optional<Price> PriceDAO::findByStockAndDate(int stock_id, const std::string& trade_date) {
    auto& conn = ConnectionManager::getInstance().getConnection();
    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return std::nullopt;
    }

    try {
        PriceTable prices;
        auto db = ConnectionManager::getInstance().getDb();
        
        for (const auto& row : (*db)(sqlpp::select(all_of(prices))
                                     .from(prices)
                                     .where(prices.stockId == stock_id and prices.tradeDate == trade_date))) {
            Price price;
            price.id = row.id;
            price.stock_id = row.stockId;
            price.trade_date = row.tradeDate;
            price.open = row.open.is_null() ? 0.0 : row.open.value();
            price.high = row.high.is_null() ? 0.0 : row.high.value();
            price.low = row.low.is_null() ? 0.0 : row.low.value();
            price.close = row.close.is_null() ? 0.0 : row.close.value();
            price.pre_close = row.preClose.is_null() ? 0.0 : row.preClose.value();
            price.change_pct = row.changePct.is_null() ? 0.0 : row.changePct.value();
            price.volume = row.volume.is_null() ? 0.0 : row.volume.value();
            price.amount = row.amount.is_null() ? 0.0 : row.amount.value();
            return price;
        }
        
        return std::nullopt;
    } catch (const std::exception& e) {
        LOG_ERROR("查询价格数据失败: " + std::string(e.what()));
        return std::nullopt;
    }
}

std::vector<Price> PriceDAO::findByStock(int stock_id) {
    auto& conn = ConnectionManager::getInstance().getConnection();
    std::vector<Price> result;

    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return result;
    }

    try {
        PriceTable prices;
        auto db = ConnectionManager::getInstance().getDb();
        
        for (const auto& row : (*db)(sqlpp::select(all_of(prices))
                                     .from(prices)
                                     .where(prices.stockId == stock_id)
                                     .order_by(prices.tradeDate.desc()))) {
            Price price;
            price.id = row.id;
            price.stock_id = row.stockId;
            price.trade_date = row.tradeDate;
            price.open = row.open.is_null() ? 0.0 : row.open.value();
            price.high = row.high.is_null() ? 0.0 : row.high.value();
            price.low = row.low.is_null() ? 0.0 : row.low.value();
            price.close = row.close.is_null() ? 0.0 : row.close.value();
            price.pre_close = row.preClose.is_null() ? 0.0 : row.preClose.value();
            price.change_pct = row.changePct.is_null() ? 0.0 : row.changePct.value();
            price.volume = row.volume.is_null() ? 0.0 : row.volume.value();
            price.amount = row.amount.is_null() ? 0.0 : row.amount.value();
            result.push_back(price);
        }
        
        return result;
    } catch (const std::exception& e) {
        LOG_ERROR("查询价格数据失败: " + std::string(e.what()));
        return result;
    }
}

std::vector<Price> PriceDAO::findByStockAndDateRange(int stock_id, const std::string& start_date, const std::string& end_date) {
    auto& conn = ConnectionManager::getInstance().getConnection();
    std::vector<Price> result;

    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return result;
    }

    try {
        PriceTable prices;
        auto db = ConnectionManager::getInstance().getDb();
        
        for (const auto& row : (*db)(sqlpp::select(all_of(prices))
                                     .from(prices)
                                     .where(prices.stockId == stock_id 
                                            and prices.tradeDate >= start_date 
                                            and prices.tradeDate <= end_date)
                                     .order_by(prices.tradeDate.asc()))) {
            Price price;
            price.id = row.id;
            price.stock_id = row.stockId;
            price.trade_date = row.tradeDate;
            price.open = row.open.is_null() ? 0.0 : row.open.value();
            price.high = row.high.is_null() ? 0.0 : row.high.value();
            price.low = row.low.is_null() ? 0.0 : row.low.value();
            price.close = row.close.is_null() ? 0.0 : row.close.value();
            price.pre_close = row.preClose.is_null() ? 0.0 : row.preClose.value();
            price.change_pct = row.changePct.is_null() ? 0.0 : row.changePct.value();
            price.volume = row.volume.is_null() ? 0.0 : row.volume.value();
            price.amount = row.amount.is_null() ? 0.0 : row.amount.value();
            result.push_back(price);
        }
        
        LOG_DEBUG("查询到 " + std::to_string(result.size()) + " 条价格数据");
        return result;
    } catch (const std::exception& e) {
        LOG_ERROR("查询价格数据失败: " + std::string(e.what()));
        return result;
    }
}

std::optional<Price> PriceDAO::findLatestByStock(int stock_id) {
    auto& conn = ConnectionManager::getInstance().getConnection();
    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return std::nullopt;
    }

    try {
        PriceTable prices;
        auto db = ConnectionManager::getInstance().getDb();
        
        for (const auto& row : (*db)(sqlpp::select(all_of(prices))
                                     .from(prices)
                                     .where(prices.stockId == stock_id)
                                     .order_by(prices.tradeDate.desc())
                                     .limit(1u))) {
            Price price;
            price.id = row.id;
            price.stock_id = row.stockId;
            price.trade_date = row.tradeDate;
            price.open = row.open.is_null() ? 0.0 : row.open.value();
            price.high = row.high.is_null() ? 0.0 : row.high.value();
            price.low = row.low.is_null() ? 0.0 : row.low.value();
            price.close = row.close.is_null() ? 0.0 : row.close.value();
            price.pre_close = row.preClose.is_null() ? 0.0 : row.preClose.value();
            price.change_pct = row.changePct.is_null() ? 0.0 : row.changePct.value();
            price.volume = row.volume.is_null() ? 0.0 : row.volume.value();
            price.amount = row.amount.is_null() ? 0.0 : row.amount.value();
            return price;
        }
        
        return std::nullopt;
    } catch (const std::exception& e) {
        LOG_ERROR("查询价格数据失败: " + std::string(e.what()));
        return std::nullopt;
    }
}

bool PriceDAO::update(const Price& price) {
    auto& conn = ConnectionManager::getInstance().getConnection();
    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return false;
    }

    try {
        PriceTable prices;
        auto db = ConnectionManager::getInstance().getDb();
        
        (*db)(sqlpp::update(prices)
              .set(
                  prices.open = price.open,
                  prices.high = price.high,
                  prices.low = price.low,
                  prices.close = price.close,
                  prices.preClose = price.pre_close,
                  prices.changePct = price.change_pct,
                  prices.volume = price.volume,
                  prices.amount = price.amount
              )
              .where(prices.stockId == price.stock_id and prices.tradeDate == price.trade_date));
        
        LOG_DEBUG("更新价格数据成功");
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("更新价格数据失败: " + std::string(e.what()));
        return false;
    }
}

bool PriceDAO::remove(int stock_id, const std::string& trade_date) {
    auto& conn = ConnectionManager::getInstance().getConnection();
    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return false;
    }

    try {
        PriceTable prices;
        auto db = ConnectionManager::getInstance().getDb();
        
        (*db)(sqlpp::remove_from(prices)
              .where(prices.stockId == stock_id and prices.tradeDate == trade_date));
        
        LOG_DEBUG("删除价格数据成功");
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("删除价格数据失败: " + std::string(e.what()));
        return false;
    }
}

int PriceDAO::removeByStock(int stock_id) {
    auto& conn = ConnectionManager::getInstance().getConnection();
    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return 0;
    }

    try {
        PriceTable prices;
        auto db = ConnectionManager::getInstance().getDb();
        
        // 先统计数量
        int count = countByStock(stock_id);
        
        // 删除
        (*db)(sqlpp::remove_from(prices).where(prices.stockId == stock_id));
        
        LOG_DEBUG("删除 " + std::to_string(count) + " 条价格数据");
        return count;
    } catch (const std::exception& e) {
        LOG_ERROR("删除价格数据失败: " + std::string(e.what()));
        return 0;
    }
}

int PriceDAO::countByStock(int stock_id) {
    auto& conn = ConnectionManager::getInstance().getConnection();
    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return 0;
    }

    try {
        PriceTable prices;
        auto db = ConnectionManager::getInstance().getDb();
        
        for (const auto& row : (*db)(sqlpp::select(sqlpp::count(prices.id))
                                     .from(prices)
                                     .where(prices.stockId == stock_id))) {
            return static_cast<int>(row.count);
        }
        
        return 0;
    } catch (const std::exception& e) {
        LOG_ERROR("查询价格数据数量失败: " + std::string(e.what()));
        return 0;
    }
}

} // namespace data
