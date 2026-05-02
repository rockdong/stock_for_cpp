#ifndef DATA_MONEY_FLOW_DAO_H
#define DATA_MONEY_FLOW_DAO_H

#include <string>
#include <vector>
#include <optional>
#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/sqlite3/sqlite3.h>
#include "MoneyFlowTable.h"
#include "../../core/FundamentalData.h"

namespace data {

using MoneyFlow = core::MoneyFlow;

class MoneyFlowDAO {
public:
    MoneyFlowDAO() = default;
    ~MoneyFlowDAO() = default;

    bool insert(const MoneyFlow& flow);

    int batchInsert(const std::vector<MoneyFlow>& flows);

    std::vector<MoneyFlow> findByTsCode(
        const std::string& ts_code,
        const std::string& start_date = "",
        const std::string& end_date = ""
    );

    std::vector<MoneyFlow> findByTradeDate(const std::string& trade_date);

    std::optional<MoneyFlow> findByTsCodeAndDate(
        const std::string& ts_code,
        const std::string& trade_date
    );

    bool exists(const std::string& ts_code, const std::string& trade_date);

    int count();

    int countByTsCode(const std::string& ts_code);
};

} // namespace data

#endif // DATA_MONEY_FLOW_DAO_H