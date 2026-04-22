#include "AnalysisProcessRecordDAO.h"
#include "AnalysisProcessRecordTable.h"
#include "AnalysisProgressTable.h"
#include "ConnectionManager.h"
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
    auto& conn = ConnectionManager::getInstance().getConnection();
    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return false;
    }

    try {
        StockProcessRecordTable table;
        auto db = ConnectionManager::getInstance().getDb();
        
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
    auto& conn = ConnectionManager::getInstance().getConnection();
    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return std::nullopt;
    }

    try {
        StockProcessRecordTable table;
        auto db = ConnectionManager::getInstance().getDb();
        
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
    auto& conn = ConnectionManager::getInstance().getConnection();
    std::vector<StockProcessRecord> results;
    
    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return results;
    }

    try {
        StockProcessRecordTable table;
        auto db = ConnectionManager::getInstance().getDb();
        
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
    auto& conn = ConnectionManager::getInstance().getConnection();
    std::vector<StockProcessRecord> results;
    
    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return results;
    }

    try {
        StockProcessRecordTable table;
        auto db = ConnectionManager::getInstance().getDb();
        
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
    auto& conn = ConnectionManager::getInstance().getConnection();
    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return false;
    }

    try {
        StockProcessRecordTable table;
        auto db = ConnectionManager::getInstance().getDb();
        
        (*db)(sqlpp::remove_from(table).where(table.tsCode == ts_code));
        
        LOG_DEBUG("删除过程记录成功: " + ts_code);
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("删除过程记录失败: " + std::string(e.what()));
        return false;
    }
}

int StockProcessRecordDAO::count() {
    auto& conn = ConnectionManager::getInstance().getConnection();
    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return 0;
    }

    try {
        StockProcessRecordTable table;
        auto db = ConnectionManager::getInstance().getDb();
        
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

AnalysisProgress StockProcessRecordDAO::getProgress() const {
    auto& conn = ConnectionManager::getInstance().getConnection();
    AnalysisProgress progress;
    progress.phase1 = {"idle", 0, 0, 0, 0};
    progress.phase2 = {"idle", 0, 0, 0, 0};
    progress.started_at = "";
    progress.phase1_completed_at = "";
    progress.updated_at = "";
    
    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return progress;
    }

    try {
        AnalysisProgressTable table;
        auto db = ConnectionManager::getInstance().getDb();
        
        for (const auto& row : (*db)(sqlpp::select(all_of(table))
                                     .from(table)
                                     .unconditionally()
                                     .limit(1u))) {
            progress.phase1.status = row.phase1Status.is_null() ? "idle" : row.phase1Status.value();
            progress.phase1.total = row.phase1Total.is_null() ? 0 : row.phase1Total.value();
            progress.phase1.completed = row.phase1Completed.is_null() ? 0 : row.phase1Completed.value();
            progress.phase1.qualified = row.phase1Qualified.is_null() ? 0 : row.phase1Qualified.value();
            
            progress.phase2.status = row.phase2Status.is_null() ? "idle" : row.phase2Status.value();
            progress.phase2.total = row.phase2Total.is_null() ? 0 : row.phase2Total.value();
            progress.phase2.completed = row.phase2Completed.is_null() ? 0 : row.phase2Completed.value();
            progress.phase2.failed = row.phase2Failed.is_null() ? 0 : row.phase2Failed.value();
            
            progress.started_at = row.startedAt.is_null() ? "" : row.startedAt.value();
            progress.phase1_completed_at = row.phase1CompletedAt.is_null() ? "" : row.phase1CompletedAt.value();
            progress.updated_at = row.updatedAt.is_null() ? "" : row.updatedAt.value();
            return progress;
        }
        
        return progress;
    } catch (const std::exception& e) {
        LOG_ERROR("查询分析进度失败: " + std::string(e.what()));
        return progress;
    }
}

} // namespace data