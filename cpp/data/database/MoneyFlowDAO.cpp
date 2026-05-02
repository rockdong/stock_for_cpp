#include "MoneyFlowDAO.h"
#include "ConnectionManager.h"
#include "Logger.h"

namespace data {

namespace {
    template<typename Row>
    MoneyFlow buildMoneyFlowFromRow(const Row& row) {
        MoneyFlow flow;
        flow.ts_code = row.tsCode;
        flow.trade_date = row.tradeDate;
        
        flow.buy_sm_vol = row.buySmVol.is_null() ? 0 : row.buySmVol.value();
        flow.buy_sm_amount = row.buySmAmount.is_null() ? 0.0 : row.buySmAmount.value();
        flow.sell_sm_vol = row.sellSmVol.is_null() ? 0 : row.sellSmVol.value();
        flow.sell_sm_amount = row.sellSmAmount.is_null() ? 0.0 : row.sellSmAmount.value();
        
        flow.buy_md_vol = row.buyMdVol.is_null() ? 0 : row.buyMdVol.value();
        flow.buy_md_amount = row.buyMdAmount.is_null() ? 0.0 : row.buyMdAmount.value();
        flow.sell_md_vol = row.sellMdVol.is_null() ? 0 : row.sellMdVol.value();
        flow.sell_md_amount = row.sellMdAmount.is_null() ? 0.0 : row.sellMdAmount.value();
        
        flow.buy_lg_vol = row.buyLgVol.is_null() ? 0 : row.buyLgVol.value();
        flow.buy_lg_amount = row.buyLgAmount.is_null() ? 0.0 : row.buyLgAmount.value();
        flow.sell_lg_vol = row.sellLgVol.is_null() ? 0 : row.sellLgVol.value();
        flow.sell_lg_amount = row.sellLgAmount.is_null() ? 0.0 : row.sellLgAmount.value();
        
        flow.buy_elg_vol = row.buyElgVol.is_null() ? 0 : row.buyElgVol.value();
        flow.buy_elg_amount = row.buyElgAmount.is_null() ? 0.0 : row.buyElgAmount.value();
        flow.sell_elg_vol = row.sellElgVol.is_null() ? 0 : row.sellElgVol.value();
        flow.sell_elg_amount = row.sellElgAmount.is_null() ? 0.0 : row.sellElgAmount.value();
        
        flow.net_mf_vol = row.netMfVol.is_null() ? 0 : row.netMfVol.value();
        flow.net_mf_amount = row.netMfAmount.is_null() ? 0.0 : row.netMfAmount.value();
        
        return flow;
    }
}

bool MoneyFlowDAO::insert(const MoneyFlow& flow) {
    auto& conn = ConnectionManager::getInstance().getConnection();
    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return false;
    }

    try {
        MoneyFlowTable moneyFlow;
        auto db = ConnectionManager::getInstance().getDb();
        
        (*db)(sqlpp::insert_into(moneyFlow).set(
            moneyFlow.tsCode = flow.ts_code,
            moneyFlow.tradeDate = flow.trade_date,
            moneyFlow.buySmVol = flow.buy_sm_vol,
            moneyFlow.buySmAmount = flow.buy_sm_amount,
            moneyFlow.sellSmVol = flow.sell_sm_vol,
            moneyFlow.sellSmAmount = flow.sell_sm_amount,
            moneyFlow.buyMdVol = flow.buy_md_vol,
            moneyFlow.buyMdAmount = flow.buy_md_amount,
            moneyFlow.sellMdVol = flow.sell_md_vol,
            moneyFlow.sellMdAmount = flow.sell_md_amount,
            moneyFlow.buyLgVol = flow.buy_lg_vol,
            moneyFlow.buyLgAmount = flow.buy_lg_amount,
            moneyFlow.sellLgVol = flow.sell_lg_vol,
            moneyFlow.sellLgAmount = flow.sell_lg_amount,
            moneyFlow.buyElgVol = flow.buy_elg_vol,
            moneyFlow.buyElgAmount = flow.buy_elg_amount,
            moneyFlow.sellElgVol = flow.sell_elg_vol,
            moneyFlow.sellElgAmount = flow.sell_elg_amount,
            moneyFlow.netMfVol = flow.net_mf_vol,
            moneyFlow.netMfAmount = flow.net_mf_amount
        ));
        
        LOG_DEBUG("插入资金流向成功: " + flow.ts_code + " " + flow.trade_date);
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("插入资金流向失败: " + std::string(e.what()));
        return false;
    }
}

int MoneyFlowDAO::batchInsert(const std::vector<MoneyFlow>& flows) {
    auto& conn = ConnectionManager::getInstance().getConnection();
    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return 0;
    }

    conn.beginTransaction();
    int successCount = 0;

    for (const auto& flow : flows) {
        if (insert(flow)) {
            successCount++;
        }
    }

    conn.commit();
    LOG_INFO("批量插入资金流向完成: " + std::to_string(successCount) + "/" + std::to_string(flows.size()));
    return successCount;
}

std::vector<MoneyFlow> MoneyFlowDAO::findByTsCode(
    const std::string& ts_code,
    const std::string& start_date,
    const std::string& end_date) {
    
    auto& conn = ConnectionManager::getInstance().getConnection();
    std::vector<MoneyFlow> result;

    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return result;
    }

    try {
        MoneyFlowTable moneyFlow;
        auto db = ConnectionManager::getInstance().getDb();
        
        auto baseCondition = moneyFlow.tsCode == ts_code;
        
        if (!start_date.empty() && !end_date.empty()) {
            for (const auto& row : (*db)(sqlpp::select(all_of(moneyFlow))
                                         .from(moneyFlow)
                                         .where(baseCondition and 
                                                moneyFlow.tradeDate >= start_date and
                                                moneyFlow.tradeDate <= end_date)
                                         .order_by(moneyFlow.tradeDate.desc()))) {
                result.push_back(buildMoneyFlowFromRow(row));
            }
        } else if (!start_date.empty()) {
            for (const auto& row : (*db)(sqlpp::select(all_of(moneyFlow))
                                         .from(moneyFlow)
                                         .where(baseCondition and moneyFlow.tradeDate >= start_date)
                                         .order_by(moneyFlow.tradeDate.desc()))) {
                result.push_back(buildMoneyFlowFromRow(row));
            }
        } else if (!end_date.empty()) {
            for (const auto& row : (*db)(sqlpp::select(all_of(moneyFlow))
                                         .from(moneyFlow)
                                         .where(baseCondition and moneyFlow.tradeDate <= end_date)
                                         .order_by(moneyFlow.tradeDate.desc()))) {
                result.push_back(buildMoneyFlowFromRow(row));
            }
        } else {
            for (const auto& row : (*db)(sqlpp::select(all_of(moneyFlow))
                                         .from(moneyFlow)
                                         .where(baseCondition)
                                         .order_by(moneyFlow.tradeDate.desc()))) {
                result.push_back(buildMoneyFlowFromRow(row));
            }
        }
        
        LOG_DEBUG("查询资金流向成功: " + ts_code + " 共 " + std::to_string(result.size()) + " 条");
        return result;
    } catch (const std::exception& e) {
        LOG_ERROR("查询资金流向失败: " + std::string(e.what()));
        return result;
    }
}

std::vector<MoneyFlow> MoneyFlowDAO::findByTradeDate(const std::string& trade_date) {
    auto& conn = ConnectionManager::getInstance().getConnection();
    std::vector<MoneyFlow> result;

    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return result;
    }

    try {
        MoneyFlowTable moneyFlow;
        auto db = ConnectionManager::getInstance().getDb();
        
        for (const auto& row : (*db)(sqlpp::select(all_of(moneyFlow))
                                     .from(moneyFlow)
                                     .where(moneyFlow.tradeDate == trade_date)
                                     .order_by(moneyFlow.tsCode.asc()))) {
            result.push_back(buildMoneyFlowFromRow(row));
        }
        
        LOG_DEBUG("查询资金流向成功: 日期 " + trade_date + " 共 " + std::to_string(result.size()) + " 条");
        return result;
    } catch (const std::exception& e) {
        LOG_ERROR("查询资金流向失败: " + std::string(e.what()));
        return result;
    }
}

std::optional<MoneyFlow> MoneyFlowDAO::findByTsCodeAndDate(
    const std::string& ts_code,
    const std::string& trade_date) {
    
    auto& conn = ConnectionManager::getInstance().getConnection();
    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return std::nullopt;
    }

    try {
        MoneyFlowTable moneyFlow;
        auto db = ConnectionManager::getInstance().getDb();
        
        for (const auto& row : (*db)(sqlpp::select(all_of(moneyFlow))
                                     .from(moneyFlow)
                                     .where(moneyFlow.tsCode == ts_code and 
                                            moneyFlow.tradeDate == trade_date))) {
            return buildMoneyFlowFromRow(row);
        }
        
        return std::nullopt;
    } catch (const std::exception& e) {
        LOG_ERROR("查询资金流向失败: " + std::string(e.what()));
        return std::nullopt;
    }
}

bool MoneyFlowDAO::exists(const std::string& ts_code, const std::string& trade_date) {
    auto& conn = ConnectionManager::getInstance().getConnection();
    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return false;
    }

    try {
        MoneyFlowTable moneyFlow;
        auto db = ConnectionManager::getInstance().getDb();
        
        for (const auto& row : (*db)(sqlpp::select(sqlpp::count(moneyFlow.id))
                                     .from(moneyFlow)
                                     .where(moneyFlow.tsCode == ts_code and 
                                            moneyFlow.tradeDate == trade_date))) {
            return row.count > 0;
        }
        
        return false;
    } catch (const std::exception& e) {
        LOG_ERROR("查询资金流向失败: " + std::string(e.what()));
        return false;
    }
}

int MoneyFlowDAO::count() {
    auto& conn = ConnectionManager::getInstance().getConnection();
    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return 0;
    }

    try {
        MoneyFlowTable moneyFlow;
        auto db = ConnectionManager::getInstance().getDb();
        
        for (const auto& row : (*db)(sqlpp::select(sqlpp::count(moneyFlow.id))
                                     .from(moneyFlow)
                                     .unconditionally())) {
            return static_cast<int>(row.count);
        }
        
        return 0;
    } catch (const std::exception& e) {
        LOG_ERROR("查询资金流向数量失败: " + std::string(e.what()));
        return 0;
    }
}

int MoneyFlowDAO::countByTsCode(const std::string& ts_code) {
    auto& conn = ConnectionManager::getInstance().getConnection();
    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return 0;
    }

    try {
        MoneyFlowTable moneyFlow;
        auto db = ConnectionManager::getInstance().getDb();
        
        for (const auto& row : (*db)(sqlpp::select(sqlpp::count(moneyFlow.id))
                                     .from(moneyFlow)
                                     .where(moneyFlow.tsCode == ts_code))) {
            return static_cast<int>(row.count);
        }
        
        return 0;
    } catch (const std::exception& e) {
        LOG_ERROR("查询资金流向数量失败: " + std::string(e.what()));
        return 0;
    }
}

} // namespace data