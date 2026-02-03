#include "StockDAO.h"
#include "Connection.h"
#include "Logger.h"
#include <sstream>

namespace data {

// 辅助函数：从数据库行构建 Stock 对象
namespace {
    template<typename Row>
    Stock buildStockFromRow(const Row& row) {
        Stock stock;
        // 只填充业务字段，不包含数据库元信息（id, created_at, updated_at）
        stock.ts_code = row.tsCode;
        stock.symbol = row.symbol.is_null() ? "" : row.symbol.value();
        stock.name = row.name.is_null() ? "" : row.name.value();
        stock.area = row.area.is_null() ? "" : row.area.value();
        stock.industry = row.industry.is_null() ? "" : row.industry.value();
        stock.fullname = row.fullname.is_null() ? "" : row.fullname.value();
        stock.enname = row.enname.is_null() ? "" : row.enname.value();
        stock.cnspell = row.cnspell.is_null() ? "" : row.cnspell.value();
        stock.market = row.market.is_null() ? "" : row.market.value();
        stock.exchange = row.exchange.is_null() ? "" : row.exchange.value();
        stock.curr_type = row.currType.is_null() ? "" : row.currType.value();
        stock.list_status = row.listStatus.is_null() ? "" : row.listStatus.value();
        stock.list_date = row.listDate.is_null() ? "" : row.listDate.value();
        stock.delist_date = row.delistDate.is_null() ? "" : row.delistDate.value();
        stock.is_hs = row.isHs.is_null() ? "" : row.isHs.value();
        return stock;
    }
}

bool StockDAO::insert(const Stock& stock) {
    auto& conn = Connection::getInstance();
    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return false;
    }

    try {
        StockTable stocks;
        auto db = conn.getDb();
        
        (*db)(sqlpp::insert_into(stocks).set(
            stocks.tsCode = stock.ts_code,
            stocks.symbol = stock.symbol,
            stocks.name = stock.name,
            stocks.area = stock.area,
            stocks.industry = stock.industry,
            stocks.fullname = stock.fullname,
            stocks.enname = stock.enname,
            stocks.cnspell = stock.cnspell,
            stocks.market = stock.market,
            stocks.exchange = stock.exchange,
            stocks.currType = stock.curr_type,
            stocks.listStatus = stock.list_status,
            stocks.listDate = stock.list_date,
            stocks.delistDate = stock.delist_date,
            stocks.isHs = stock.is_hs
        ));
        
        LOG_DEBUG("插入股票成功: " + stock.ts_code);
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("插入股票失败: " + std::string(e.what()));
        return false;
    }
}

int StockDAO::batchInsert(const std::vector<Stock>& stocks) {
    auto& conn = Connection::getInstance();
    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return 0;
    }

    conn.beginTransaction();
    int successCount = 0;

    for (const auto& stock : stocks) {
        if (insert(stock)) {
            successCount++;
        }
    }

    conn.commit();
    LOG_INFO("批量插入股票完成: " + std::to_string(successCount) + "/" + std::to_string(stocks.size()));
    return successCount;
}

std::optional<Stock> StockDAO::findByTsCode(const std::string& ts_code) {
    auto& conn = Connection::getInstance();
    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return std::nullopt;
    }

    try {
        StockTable stocks;
        auto db = conn.getDb();
        
        for (const auto& row : (*db)(sqlpp::select(all_of(stocks))
                                     .from(stocks)
                                     .where(stocks.tsCode == ts_code))) {
            return buildStockFromRow(row);
        }
        
        return std::nullopt;
    } catch (const std::exception& e) {
        LOG_ERROR("查询股票失败: " + std::string(e.what()));
        return std::nullopt;
    }
}

std::optional<Stock> StockDAO::findById(int id) {
    auto& conn = Connection::getInstance();
    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return std::nullopt;
    }

    try {
        StockTable stocks;
        auto db = conn.getDb();
        
        for (const auto& row : (*db)(sqlpp::select(all_of(stocks))
                                     .from(stocks)
                                     .where(stocks.id == id))) {
            return buildStockFromRow(row);
        }
        
        return std::nullopt;
    } catch (const std::exception& e) {
        LOG_ERROR("查询股票失败: " + std::string(e.what()));
        return std::nullopt;
    }
}

std::vector<Stock> StockDAO::findAll() {
    auto& conn = Connection::getInstance();
    std::vector<Stock> result;

    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return result;
    }

    try {
        StockTable stocks;
        auto db = conn.getDb();
        
        for (const auto& row : (*db)(sqlpp::select(all_of(stocks))
                                     .from(stocks)
                                     .unconditionally()
                                     .order_by(stocks.tsCode.asc()))) {
            result.push_back(buildStockFromRow(row));
        }
        
        LOG_DEBUG("查询到 " + std::to_string(result.size()) + " 只股票");
        return result;
    } catch (const std::exception& e) {
        LOG_ERROR("查询股票失败: " + std::string(e.what()));
        return result;
    }
}

std::vector<Stock> StockDAO::findByMarket(const std::string& market) {
    auto& conn = Connection::getInstance();
    std::vector<Stock> result;

    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return result;
    }

    try {
        StockTable stocks;
        auto db = conn.getDb();
        
        for (const auto& row : (*db)(sqlpp::select(all_of(stocks))
                                     .from(stocks)
                                     .where(stocks.market == market)
                                     .order_by(stocks.tsCode.asc()))) {
            result.push_back(buildStockFromRow(row));
        }
        
        return result;
    } catch (const std::exception& e) {
        LOG_ERROR("查询股票失败: " + std::string(e.what()));
        return result;
    }
}

std::vector<Stock> StockDAO::findByIndustry(const std::string& industry) {
    auto& conn = Connection::getInstance();
    std::vector<Stock> result;

    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return result;
    }

    try {
        StockTable stocks;
        auto db = conn.getDb();
        
        for (const auto& row : (*db)(sqlpp::select(all_of(stocks))
                                     .from(stocks)
                                     .where(stocks.industry == industry)
                                     .order_by(stocks.tsCode.asc()))) {
            result.push_back(buildStockFromRow(row));
        }
        
        return result;
    } catch (const std::exception& e) {
        LOG_ERROR("查询股票失败: " + std::string(e.what()));
        return result;
    }
}

bool StockDAO::update(const Stock& stock) {
    auto& conn = Connection::getInstance();
    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return false;
    }

    try {
        StockTable stocks;
        auto db = conn.getDb();
        
        (*db)(sqlpp::update(stocks)
              .set(
                  stocks.symbol = stock.symbol,
                  stocks.name = stock.name,
                  stocks.area = stock.area,
                  stocks.industry = stock.industry,
                  stocks.fullname = stock.fullname,
                  stocks.enname = stock.enname,
                  stocks.cnspell = stock.cnspell,
                  stocks.market = stock.market,
                  stocks.exchange = stock.exchange,
                  stocks.currType = stock.curr_type,
                  stocks.listStatus = stock.list_status,
                  stocks.listDate = stock.list_date,
                  stocks.delistDate = stock.delist_date,
                  stocks.isHs = stock.is_hs
              )
              .where(stocks.tsCode == stock.ts_code));
        
        LOG_DEBUG("更新股票成功: " + stock.ts_code);
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("更新股票失败: " + std::string(e.what()));
        return false;
    }
}

bool StockDAO::remove(const std::string& ts_code) {
    auto& conn = Connection::getInstance();
    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return false;
    }

    try {
        StockTable stocks;
        auto db = conn.getDb();
        
        (*db)(sqlpp::remove_from(stocks).where(stocks.tsCode == ts_code));
        
        LOG_DEBUG("删除股票成功: " + ts_code);
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("删除股票失败: " + std::string(e.what()));
        return false;
    }
}

bool StockDAO::exists(const std::string& ts_code) {
    auto& conn = Connection::getInstance();
    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return false;
    }

    try {
        StockTable stocks;
        auto db = conn.getDb();
        
        for (const auto& row : (*db)(sqlpp::select(sqlpp::count(stocks.id))
                                     .from(stocks)
                                     .where(stocks.tsCode == ts_code))) {
            return row.count > 0;
        }
        
        return false;
    } catch (const std::exception& e) {
        LOG_ERROR("查询股票失败: " + std::string(e.what()));
        return false;
    }
}

int StockDAO::count() {
    auto& conn = Connection::getInstance();
    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return 0;
    }

    try {
        StockTable stocks;
        auto db = conn.getDb();
        
        for (const auto& row : (*db)(sqlpp::select(sqlpp::count(stocks.id))
                                     .from(stocks)
                                     .unconditionally())) {
            return static_cast<int>(row.count);
        }
        
        return 0;
    } catch (const std::exception& e) {
        LOG_ERROR("查询股票数量失败: " + std::string(e.what()));
        return 0;
    }
}

} // namespace data
