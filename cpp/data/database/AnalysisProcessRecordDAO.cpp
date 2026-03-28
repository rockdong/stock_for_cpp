#include "AnalysisProcessRecordDAO.h"
#include "AnalysisProcessRecordTable.h"
#include "Connection.h"
#include "Logger.h"
#include <sqlpp11/sqlpp11.h>

namespace data {

namespace {
    template<typename Row>
    StockProcessRecord buildRecordFromRow(const Row& row) {
        StockProcessRecord record;
        record.id = row.id;
        record.ts_code = row.tsCode;
        record.stock_name = row.stockName.is_null() ? "" : row.stockName.value();
        record.trade_date = row.tradeDate;
        
        std::string dataJson = row.data;
        if (!dataJson.empty()) {
            record.dataFromJson(dataJson);
        }
        
        record.created_at = row.createdAt.is_null() ? "" : row.createdAt.value();
        record.expires_at = row.expiresAt.is_null() ? "" : row.expiresAt.value();
        
        return record;
    }
}

bool StockProcessRecordDAO::upsert(const StockProcessRecord& record) {
    auto& conn = Connection::getInstance();
    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return false;
    }

    try {
        StockProcessRecordTable table;
        auto db = conn.getDb();
        
        (*db)(sqlpp::sqlite3::insert_or_replace_into(table).set(
            table.tsCode = record.ts_code,
            table.stockName = record.stock_name,
            table.tradeDate = record.trade_date,
            table.data = record.dataToJson()
        ));
        
        LOG_DEBUG("Upsert过程记录: " + record.ts_code);
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("Upsert过程记录失败: " + std::string(e.what()));
        return false;
    }
}

std::optional<StockProcessRecord> StockProcessRecordDAO::findByTsCode(
    const std::string& ts_code,
    const std::string& trade_date
) {
    auto& conn = Connection::getInstance();
    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return std::nullopt;
    }

    try {
        StockProcessRecordTable table;
        auto db = conn.getDb();
        
        if (!trade_date.empty()) {
            for (const auto& row : (*db)(sqlpp::select(all_of(table))
                                         .from(table)
                                         .where(table.tsCode == ts_code and table.tradeDate == trade_date)
                                         .limit(1u))) {
                return buildRecordFromRow(row);
            }
        } else {
            for (const auto& row : (*db)(sqlpp::select(all_of(table))
                                         .from(table)
                                         .where(table.tsCode == ts_code)
                                         .order_by(table.tradeDate.desc())
                                         .limit(1u))) {
                return buildRecordFromRow(row);
            }
        }
        
        return std::nullopt;
    } catch (const std::exception& e) {
        LOG_ERROR("查询过程记录失败: " + std::string(e.what()));
        return std::nullopt;
    }
}

std::vector<StockProcessRecord> StockProcessRecordDAO::findAll(int limit) {
    auto& conn = Connection::getInstance();
    std::vector<StockProcessRecord> results;
    
    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return results;
    }

    try {
        StockProcessRecordTable table;
        auto db = conn.getDb();
        
        for (const auto& row : (*db)(sqlpp::select(all_of(table))
                                     .from(table)
                                     .unconditionally()
                                     .order_by(table.createdAt.desc())
                                     .limit(static_cast<size_t>(limit)))) {
            results.push_back(buildRecordFromRow(row));
        }
        
        return results;
    } catch (const std::exception& e) {
        LOG_ERROR("查询过程记录失败: " + std::string(e.what()));
        return results;
    }
}

std::vector<StockProcessRecord> StockProcessRecordDAO::findByDateRange(
    const std::string& start_date,
    const std::string& end_date
) {
    auto& conn = Connection::getInstance();
    std::vector<StockProcessRecord> results;
    
    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return results;
    }

    try {
        StockProcessRecordTable table;
        auto db = conn.getDb();
        
        for (const auto& row : (*db)(sqlpp::select(all_of(table))
                                     .from(table)
                                     .where(table.tradeDate >= start_date 
                                            and table.tradeDate <= end_date)
                                     .order_by(table.tradeDate.desc()))) {
            results.push_back(buildRecordFromRow(row));
        }
        
        return results;
    } catch (const std::exception& e) {
        LOG_ERROR("查询过程记录失败: " + std::string(e.what()));
        return results;
    }
}

bool StockProcessRecordDAO::remove(const std::string& ts_code) {
    auto& conn = Connection::getInstance();
    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return false;
    }

    try {
        StockProcessRecordTable table;
        auto db = conn.getDb();
        
        (*db)(sqlpp::remove_from(table).where(table.tsCode == ts_code));
        
        LOG_DEBUG("删除过程记录成功: " + ts_code);
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("删除过程记录失败: " + std::string(e.what()));
        return false;
    }
}

int StockProcessRecordDAO::count() {
    auto& conn = Connection::getInstance();
    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return 0;
    }

    try {
        StockProcessRecordTable table;
        auto db = conn.getDb();
        
        for (const auto& row : (*db)(sqlpp::select(sqlpp::count(table.id))
                                     .from(table)
                                     .unconditionally())) {
            return static_cast<int>(row.count);
        }
        
        return 0;
    } catch (const std::exception& e) {
        LOG_ERROR("查询过程记录数量失败: " + std::string(e.what()));
        return 0;
    }
}

} // namespace data