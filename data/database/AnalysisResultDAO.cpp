#include "AnalysisResultDAO.h"
#include "Connection.h"
#include "Logger.h"

namespace data {

// 辅助函数：从数据库行构建 AnalysisResult 对象
namespace {
    template<typename Row>
    AnalysisResult buildAnalysisResultFromRow(const Row& row) {
        AnalysisResult result;
        result.ts_code = row.tsCode;
        result.strategy_name = row.strategyName;
        result.trade_date = row.tradeDate;
        result.label = row.label;
        return result;
    }
}

bool AnalysisResultDAO::insert(const AnalysisResult& result) {
    auto& conn = Connection::getInstance();
    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return false;
    }

    try {
        AnalysisResultTable results;
        auto db = conn.getDb();
        
        (*db)(sqlpp::insert_into(results).set(
            results.tsCode = result.ts_code,
            results.strategyName = result.strategy_name,
            results.tradeDate = result.trade_date,
            results.label = result.label
        ));
        
        LOG_DEBUG("插入分析结果成功: " + result.ts_code + " - " + result.strategy_name);
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("插入分析结果失败: " + std::string(e.what()));
        return false;
    }
}

int AnalysisResultDAO::batchInsert(const std::vector<AnalysisResult>& results) {
    auto& conn = Connection::getInstance();
    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return 0;
    }

    conn.beginTransaction();
    int successCount = 0;

    for (const auto& result : results) {
        if (insert(result)) {
            successCount++;
        }
    }

    conn.commit();
    LOG_INFO("批量插入分析结果完成: " + std::to_string(successCount) + "/" + std::to_string(results.size()));
    return successCount;
}

std::optional<AnalysisResult> AnalysisResultDAO::findById(int id) {
    auto& conn = Connection::getInstance();
    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return std::nullopt;
    }

    try {
        AnalysisResultTable results;
        auto db = conn.getDb();
        
        for (const auto& row : (*db)(sqlpp::select(all_of(results))
                                     .from(results)
                                     .where(results.id == id))) {
            return buildAnalysisResultFromRow(row);
        }
        
        return std::nullopt;
    } catch (const std::exception& e) {
        LOG_ERROR("查询分析结果失败: " + std::string(e.what()));
        return std::nullopt;
    }
}

std::vector<AnalysisResult> AnalysisResultDAO::findAll() {
    auto& conn = Connection::getInstance();
    std::vector<AnalysisResult> result;

    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return result;
    }

    try {
        AnalysisResultTable results;
        auto db = conn.getDb();
        
        for (const auto& row : (*db)(sqlpp::select(all_of(results))
                                     .from(results)
                                     .unconditionally()
                                     .order_by(results.createdAt.desc()))) {
            result.push_back(buildAnalysisResultFromRow(row));
        }
        
        LOG_DEBUG("查询到 " + std::to_string(result.size()) + " 条分析结果");
        return result;
    } catch (const std::exception& e) {
        LOG_ERROR("查询分析结果失败: " + std::string(e.what()));
        return result;
    }
}

std::vector<AnalysisResult> AnalysisResultDAO::findByTsCode(const std::string& ts_code) {
    auto& conn = Connection::getInstance();
    std::vector<AnalysisResult> result;

    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return result;
    }

    try {
        AnalysisResultTable results;
        auto db = conn.getDb();
        
        for (const auto& row : (*db)(sqlpp::select(all_of(results))
                                     .from(results)
                                     .where(results.tsCode == ts_code)
                                     .order_by(results.tradeDate.desc()))) {
            result.push_back(buildAnalysisResultFromRow(row));
        }
        
        return result;
    } catch (const std::exception& e) {
        LOG_ERROR("查询分析结果失败: " + std::string(e.what()));
        return result;
    }
}

std::vector<AnalysisResult> AnalysisResultDAO::findByStrategy(const std::string& strategy_name) {
    auto& conn = Connection::getInstance();
    std::vector<AnalysisResult> result;

    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return result;
    }

    try {
        AnalysisResultTable results;
        auto db = conn.getDb();
        
        for (const auto& row : (*db)(sqlpp::select(all_of(results))
                                     .from(results)
                                     .where(results.strategyName == strategy_name)
                                     .order_by(results.tradeDate.desc()))) {
            result.push_back(buildAnalysisResultFromRow(row));
        }
        
        return result;
    } catch (const std::exception& e) {
        LOG_ERROR("查询分析结果失败: " + std::string(e.what()));
        return result;
    }
}

std::vector<AnalysisResult> AnalysisResultDAO::findByLabel(const std::string& label) {
    auto& conn = Connection::getInstance();
    std::vector<AnalysisResult> result;

    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return result;
    }

    try {
        AnalysisResultTable results;
        auto db = conn.getDb();
        
        for (const auto& row : (*db)(sqlpp::select(all_of(results))
                                     .from(results)
                                     .where(results.label == label)
                                     .order_by(results.tradeDate.desc()))) {
            result.push_back(buildAnalysisResultFromRow(row));
        }
        
        return result;
    } catch (const std::exception& e) {
        LOG_ERROR("查询分析结果失败: " + std::string(e.what()));
        return result;
    }
}

std::vector<AnalysisResult> AnalysisResultDAO::findByTsCodeAndStrategy(
    const std::string& ts_code,
    const std::string& strategy_name
) {
    auto& conn = Connection::getInstance();
    std::vector<AnalysisResult> result;

    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return result;
    }

    try {
        AnalysisResultTable results;
        auto db = conn.getDb();
        
        for (const auto& row : (*db)(sqlpp::select(all_of(results))
                                     .from(results)
                                     .where(results.tsCode == ts_code 
                                            and results.strategyName == strategy_name)
                                     .order_by(results.tradeDate.desc()))) {
            result.push_back(buildAnalysisResultFromRow(row));
        }
        
        return result;
    } catch (const std::exception& e) {
        LOG_ERROR("查询分析结果失败: " + std::string(e.what()));
        return result;
    }
}

std::vector<AnalysisResult> AnalysisResultDAO::findByDateRange(
    const std::string& start_date,
    const std::string& end_date
) {
    auto& conn = Connection::getInstance();
    std::vector<AnalysisResult> result;

    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return result;
    }

    try {
        AnalysisResultTable results;
        auto db = conn.getDb();
        
        for (const auto& row : (*db)(sqlpp::select(all_of(results))
                                     .from(results)
                                     .where(results.tradeDate >= start_date 
                                            and results.tradeDate <= end_date)
                                     .order_by(results.tradeDate.desc()))) {
            result.push_back(buildAnalysisResultFromRow(row));
        }
        
        return result;
    } catch (const std::exception& e) {
        LOG_ERROR("查询分析结果失败: " + std::string(e.what()));
        return result;
    }
}

bool AnalysisResultDAO::update(int id, const AnalysisResult& result) {
    auto& conn = Connection::getInstance();
    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return false;
    }

    try {
        AnalysisResultTable results;
        auto db = conn.getDb();
        
        (*db)(sqlpp::update(results)
              .set(
                  results.tsCode = result.ts_code,
                  results.strategyName = result.strategy_name,
                  results.tradeDate = result.trade_date,
                  results.label = result.label
              )
              .where(results.id == id));
        
        LOG_DEBUG("更新分析结果成功: ID=" + std::to_string(id));
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("更新分析结果失败: " + std::string(e.what()));
        return false;
    }
}

bool AnalysisResultDAO::remove(int id) {
    auto& conn = Connection::getInstance();
    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return false;
    }

    try {
        AnalysisResultTable results;
        auto db = conn.getDb();
        
        (*db)(sqlpp::remove_from(results).where(results.id == id));
        
        LOG_DEBUG("删除分析结果成功: ID=" + std::to_string(id));
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("删除分析结果失败: " + std::string(e.what()));
        return false;
    }
}

bool AnalysisResultDAO::removeByTsCode(const std::string& ts_code) {
    auto& conn = Connection::getInstance();
    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return false;
    }

    try {
        AnalysisResultTable results;
        auto db = conn.getDb();
        
        (*db)(sqlpp::remove_from(results).where(results.tsCode == ts_code));
        
        LOG_DEBUG("删除股票分析结果成功: " + ts_code);
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("删除股票分析结果失败: " + std::string(e.what()));
        return false;
    }
}

bool AnalysisResultDAO::removeByStrategy(const std::string& strategy_name) {
    auto& conn = Connection::getInstance();
    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return false;
    }

    try {
        AnalysisResultTable results;
        auto db = conn.getDb();
        
        (*db)(sqlpp::remove_from(results).where(results.strategyName == strategy_name));
        
        LOG_DEBUG("删除策略分析结果成功: " + strategy_name);
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("删除策略分析结果失败: " + std::string(e.what()));
        return false;
    }
}

int AnalysisResultDAO::count() {
    auto& conn = Connection::getInstance();
    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return 0;
    }

    try {
        AnalysisResultTable results;
        auto db = conn.getDb();
        
        for (const auto& row : (*db)(sqlpp::select(sqlpp::count(results.id))
                                     .from(results)
                                     .unconditionally())) {
            return static_cast<int>(row.count);
        }
        
        return 0;
    } catch (const std::exception& e) {
        LOG_ERROR("查询分析结果数量失败: " + std::string(e.what()));
        return 0;
    }
}

int AnalysisResultDAO::countByTsCode(const std::string& ts_code) {
    auto& conn = Connection::getInstance();
    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return 0;
    }

    try {
        AnalysisResultTable results;
        auto db = conn.getDb();
        
        for (const auto& row : (*db)(sqlpp::select(sqlpp::count(results.id))
                                     .from(results)
                                     .where(results.tsCode == ts_code))) {
            return static_cast<int>(row.count);
        }
        
        return 0;
    } catch (const std::exception& e) {
        LOG_ERROR("查询股票分析结果数量失败: " + std::string(e.what()));
        return 0;
    }
}

} // namespace data

