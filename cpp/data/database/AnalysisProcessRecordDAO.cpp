#include "AnalysisProcessRecordDAO.h"
#include "AnalysisProcessRecordTable.h"
#include "Connection.h"
#include "Logger.h"
#include <sqlpp11/sqlpp11.h>

namespace data {

namespace {
    template<typename Row>
    AnalysisProcessRecord buildRecordFromRow(const Row& row) {
        AnalysisProcessRecord record;
        record.id = row.id;
        record.ts_code = row.tsCode;
        record.stock_name = row.stockName.is_null() ? "" : row.stockName.value();
        record.strategy_name = row.strategyName;
        record.trade_date = row.tradeDate;
        record.freq = row.freq.is_null() ? "d" : row.freq.value();
        record.signal = row.signal.is_null() ? "NONE" : row.signal.value();
        
        std::string dataJson = row.data;
        if (!dataJson.empty()) {
            record.dataFromJson(dataJson);
        }
        
        record.created_at = row.createdAt.is_null() ? "" : row.createdAt.value();
        record.expires_at = row.expiresAt.is_null() ? "" : row.expiresAt.value();
        
        return record;
    }
}

bool AnalysisProcessRecordDAO::insert(const AnalysisProcessRecord& record) {
    auto& conn = Connection::getInstance();
    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return false;
    }

    try {
        AnalysisProcessRecordTable table;
        auto db = conn.getDb();
        
        (*db)(sqlpp::insert_into(table).set(
            table.tsCode = record.ts_code,
            table.stockName = record.stock_name,
            table.strategyName = record.strategy_name,
            table.tradeDate = record.trade_date,
            table.freq = record.freq,
            table.signal = record.signal,
            table.data = record.dataToJson()
        ));
        
        LOG_DEBUG("插入过程记录: " + record.ts_code + " - " + record.strategy_name);
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("插入过程记录失败: " + std::string(e.what()));
        return false;
    }
}

bool AnalysisProcessRecordDAO::upsert(const AnalysisProcessRecord& record) {
    auto& conn = Connection::getInstance();
    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return false;
    }

    try {
        AnalysisProcessRecordTable table;
        auto db = conn.getDb();
        
        (*db)(sqlpp::sqlite3::insert_or_replace_into(table).set(
            table.tsCode = record.ts_code,
            table.stockName = record.stock_name,
            table.strategyName = record.strategy_name,
            table.tradeDate = record.trade_date,
            table.freq = record.freq,
            table.signal = record.signal,
            table.data = record.dataToJson()
        ));
        
        LOG_DEBUG("Upsert过程记录: " + record.ts_code + " - " + record.strategy_name);
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("Upsert过程记录失败: " + std::string(e.what()));
        return false;
    }
}

std::optional<AnalysisProcessRecord> AnalysisProcessRecordDAO::findById(int id) {
    auto& conn = Connection::getInstance();
    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return std::nullopt;
    }

    try {
        AnalysisProcessRecordTable table;
        auto db = conn.getDb();
        
        for (const auto& row : (*db)(sqlpp::select(all_of(table))
                                     .from(table)
                                     .where(table.id == id))) {
            return buildRecordFromRow(row);
        }
        
        return std::nullopt;
    } catch (const std::exception& e) {
        LOG_ERROR("查询过程记录失败: " + std::string(e.what()));
        return std::nullopt;
    }
}

std::vector<AnalysisProcessRecord> AnalysisProcessRecordDAO::findByTsCode(const std::string& ts_code) {
    auto& conn = Connection::getInstance();
    std::vector<AnalysisProcessRecord> results;
    
    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return results;
    }

    try {
        AnalysisProcessRecordTable table;
        auto db = conn.getDb();
        
        for (const auto& row : (*db)(sqlpp::select(all_of(table))
                                     .from(table)
                                     .where(table.tsCode == ts_code)
                                     .order_by(table.createdAt.desc()))) {
            results.push_back(buildRecordFromRow(row));
        }
        
        return results;
    } catch (const std::exception& e) {
        LOG_ERROR("查询过程记录失败: " + std::string(e.what()));
        return results;
    }
}

std::vector<AnalysisProcessRecord> AnalysisProcessRecordDAO::findByStrategy(const std::string& strategy_name) {
    auto& conn = Connection::getInstance();
    std::vector<AnalysisProcessRecord> results;
    
    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return results;
    }

    try {
        AnalysisProcessRecordTable table;
        auto db = conn.getDb();
        
        for (const auto& row : (*db)(sqlpp::select(all_of(table))
                                     .from(table)
                                     .where(table.strategyName == strategy_name)
                                     .order_by(table.createdAt.desc()))) {
            results.push_back(buildRecordFromRow(row));
        }
        
        return results;
    } catch (const std::exception& e) {
        LOG_ERROR("查询过程记录失败: " + std::string(e.what()));
        return results;
    }
}

std::vector<AnalysisProcessRecord> AnalysisProcessRecordDAO::findBySignal(const std::string& signal) {
    auto& conn = Connection::getInstance();
    std::vector<AnalysisProcessRecord> results;
    
    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return results;
    }

    try {
        AnalysisProcessRecordTable table;
        auto db = conn.getDb();
        
        for (const auto& row : (*db)(sqlpp::select(all_of(table))
                                     .from(table)
                                     .where(table.signal == signal)
                                     .order_by(table.createdAt.desc()))) {
            results.push_back(buildRecordFromRow(row));
        }
        
        return results;
    } catch (const std::exception& e) {
        LOG_ERROR("查询过程记录失败: " + std::string(e.what()));
        return results;
    }
}

std::optional<AnalysisProcessRecord> AnalysisProcessRecordDAO::findLatest(
    const std::string& ts_code,
    const std::string& strategy_name,
    const std::string& freq
) {
    auto& conn = Connection::getInstance();
    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return std::nullopt;
    }

    try {
        AnalysisProcessRecordTable table;
        auto db = conn.getDb();
        
        for (const auto& row : (*db)(sqlpp::select(all_of(table))
                                     .from(table)
                                     .where(table.tsCode == ts_code 
                                            and table.strategyName == strategy_name
                                            and table.freq == freq)
                                     .order_by(table.tradeDate.desc())
                                     .limit(1u))) {
            return buildRecordFromRow(row);
        }
        
        return std::nullopt;
    } catch (const std::exception& e) {
        LOG_ERROR("查询最新过程记录失败: " + std::string(e.what()));
        return std::nullopt;
    }
}

std::vector<AnalysisProcessRecord> AnalysisProcessRecordDAO::findAll(int limit) {
    auto& conn = Connection::getInstance();
    std::vector<AnalysisProcessRecord> results;
    
    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return results;
    }

    try {
        AnalysisProcessRecordTable table;
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

bool AnalysisProcessRecordDAO::remove(int id) {
    auto& conn = Connection::getInstance();
    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return false;
    }

    try {
        AnalysisProcessRecordTable table;
        auto db = conn.getDb();
        
        (*db)(sqlpp::remove_from(table).where(table.id == id));
        
        LOG_DEBUG("删除过程记录成功: ID=" + std::to_string(id));
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("删除过程记录失败: " + std::string(e.what()));
        return false;
    }
}

int AnalysisProcessRecordDAO::count() {
    auto& conn = Connection::getInstance();
    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return 0;
    }

    try {
        AnalysisProcessRecordTable table;
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