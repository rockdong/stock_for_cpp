# MoneyFlow API Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Add Tushare moneyflow API (个股资金流向) for strategy analysis, with database storage support.

**Architecture:** Follow existing SOLID pattern - add data structure, HTTP method, interface method, implementation, and DAO for database storage.

**Tech Stack:** C++17, cpp-httplib (HTTP), nlohmann/json, MySQL

---

## Task 1: Add MoneyFlow Data Structure

**Files:**
- Modify: `cpp/core/FundamentalData.h`

**Step 1: Add MoneyFlow struct**

Add to `cpp/core/FundamentalData.h` after `FinancialIndicator` struct:

```cpp
/**
 * @brief 资金流向数据
 * 
 * 来自 Tushare moneyflow 接口
 * 小单：<5万；中单：5万~20万；大单：20万~100万；特大单：>=100万
 */
struct MoneyFlow {
    std::string ts_code;           // TS股票代码
    std::string trade_date;        // 交易日期
    
    // 小单（<5万）
    int buy_sm_vol = 0;            // 小单买入量（手）
    double buy_sm_amount = 0.0;    // 小单买入金额（万元）
    int sell_sm_vol = 0;           // 小单卖出量（手）
    double sell_sm_amount = 0.0;   // 小单卖出金额（万元）
    
    // 中单（5万~20万）
    int buy_md_vol = 0;            // 中单买入量（手）
    double buy_md_amount = 0.0;    // 中单买入金额（万元）
    int sell_md_vol = 0;           // 中单卖出量（手）
    double sell_md_amount = 0.0;   // 中单卖出金额（万元）
    
    // 大单（20万~100万）
    int buy_lg_vol = 0;            // 大单买入量（手）
    double buy_lg_amount = 0.0;    // 大单买入金额（万元）
    int sell_lg_vol = 0;           // 大单卖出量（手）
    double sell_lg_amount = 0.0;   // 大单卖出金额（万元）
    
    // 特大单（>=100万）
    int buy_elg_vol = 0;           // 特大单买入量（手）
    double buy_elg_amount = 0.0;   // 特大单买入金额（万元）
    int sell_elg_vol = 0;          // 特大单卖出量（手）
    double sell_elg_amount = 0.0;  // 特大单卖出金额（万元）
    
    // 净流入
    int net_mf_vol = 0;            // 净流入量（手）
    double net_mf_amount = 0.0;    // 净流入额（万元）
};
```

**Step 2: Update Network.h type alias**

Modify `cpp/network/IDataSource.h` line 15, add:

```cpp
using MoneyFlow = core::MoneyFlow;
```

**Step 3: Verify syntax**

Run: `cd cpp/build && cmake .. && make -j4 2>&1 | head -50`

Expected: Compilation may fail at next steps, but struct should be valid.

**Step 4: Commit**

```bash
git add cpp/core/FundamentalData.h cpp/network/IDataSource.h
git commit -m "feat(core): add MoneyFlow data structure for moneyflow API"
```

---

## Task 2: Add HTTP Method in TushareClient

**Files:**
- Modify: `cpp/network/http/TushareClient.h`
- Modify: `cpp/network/http/TushareClient.cpp`

**Step 1: Add declaration in TushareClient.h**

Add after `getHsConst()` method (around line 203):

```cpp
    // ========== 资金流向数据 ==========
    
    /**
     * @brief 获取个股资金流向
     * @param ts_code 股票代码（如 000001.SZ）
     * @param trade_date 交易日期（YYYYMMDD）
     * @param start_date 开始日期（YYYYMMDD）
     * @param end_date 结束日期（YYYYMMDD）
     * @return Tushare 响应
     */
    TushareResponse getMoneyFlow(const std::string& ts_code = "",
                                  const std::string& trade_date = "",
                                  const std::string& start_date = "",
                                  const std::string& end_date = "");
```

**Step 2: Add implementation in TushareClient.cpp**

Add to `cpp/network/http/TushareClient.cpp`:

```cpp
TushareResponse TushareClient::getMoneyFlow(
    const std::string& ts_code,
    const std::string& trade_date,
    const std::string& start_date,
    const std::string& end_date) {
    
    std::map<std::string, std::string> params;
    if (!ts_code.empty()) params["ts_code"] = ts_code;
    if (!trade_date.empty()) params["trade_date"] = trade_date;
    if (!start_date.empty()) params["start_date"] = start_date;
    if (!end_date.empty()) params["end_date"] = end_date;
    
    return query("moneyflow", params);
}
```

**Step 3: Compile and verify**

Run: `cd cpp/build && cmake .. && make -j4`

Expected: Compilation success.

**Step 4: Commit**

```bash
git add cpp/network/http/TushareClient.h cpp/network/http/TushareClient.cpp
git commit -m "feat(network): add getMoneyFlow method to TushareClient"
```

---

## Task 3: Add Interface Method in IDataSource

**Files:**
- Modify: `cpp/network/IDataSource.h`

**Step 1: Add virtual method**

Add to `cpp/network/IDataSource.h` after `getFinancialIndicators()` (around line 103):

```cpp
    /**
     * @brief 获取资金流向数据
     * @param ts_code 股票代码（可选，空表示全市场）
     * @param trade_date 交易日期（可选）
     * @param start_date 开始日期（可选）
     * @param end_date 结束日期（可选）
     * @return 资金流向列表
     */
    virtual std::vector<MoneyFlow> getMoneyFlow(
        const std::string& ts_code = "",
        const std::string& trade_date = "",
        const std::string& start_date = "",
        const std::string& end_date = ""
    ) = 0;
```

**Step 2: Compile**

Run: `cd cpp/build && cmake .. && make -j4`

Expected: Compilation will fail (TushareDataSource doesn't implement it yet).

**Step 3: Commit**

```bash
git add cpp/network/IDataSource.h
git commit -m "feat(network): add getMoneyFlow to IDataSource interface"
```

---

## Task 4: Implement in TushareDataSource

**Files:**
- Modify: `cpp/network/TushareDataSource.h`
- Modify: `cpp/network/TushareDataSource.cpp`

**Step 1: Add declaration in TushareDataSource.h**

Add after `getFinancialIndicators()` declaration (around line 116):

```cpp
    /**
     * @brief 获取资金流向数据
     * @param ts_code 股票代码（可选）
     * @param trade_date 交易日期（可选）
     * @param start_date 开始日期（可选）
     * @param end_date 结束日期（可选）
     * @return 资金流向列表
     */
    std::vector<MoneyFlow> getMoneyFlow(
        const std::string& ts_code = "",
        const std::string& trade_date = "",
        const std::string& start_date = "",
        const std::string& end_date = ""
    ) override;

private:
    // ... existing parse methods ...
    
    /**
     * @brief 解析资金流向数据
     * @param response Tushare 响应
     * @return 资金流向列表
     */
    std::vector<MoneyFlow> parseMoneyFlow(const TushareResponse& response);
```

**Step 2: Add implementation in TushareDataSource.cpp**

Add to `cpp/network/TushareDataSource.cpp`:

```cpp
std::vector<MoneyFlow> TushareDataSource::getMoneyFlow(
    const std::string& ts_code,
    const std::string& trade_date,
    const std::string& start_date,
    const std::string& end_date) {
    
    try {
        LOG_DEBUG("获取资金流向数据" + 
                  (ts_code.empty() ? "" : ": " + ts_code) +
                  (trade_date.empty() ? "" : " 日期: " + trade_date));
        
        auto response = client_->getMoneyFlow(ts_code, trade_date, start_date, end_date);
        
        if (response.isSuccess()) {
            return parseMoneyFlow(response);
        } else {
            LOG_ERROR("获取资金流向失败: " + response.msg);
            return {};
        }
    } catch (const std::exception& e) {
        LOG_ERROR("获取资金流向异常: " + std::string(e.what()));
        return {};
    }
}

std::vector<MoneyFlow> TushareDataSource::parseMoneyFlow(const TushareResponse& response) {
    std::vector<MoneyFlow> result;
    
    try {
        if (!response.data.contains("fields") || !response.data.contains("items")) {
            LOG_ERROR("响应数据格式错误");
            return result;
        }
        
        auto fields = response.data["fields"].get<std::vector<std::string>>();
        auto items = response.data["items"];
        
        std::unordered_map<std::string, size_t> field_index;
        for (size_t i = 0; i < fields.size(); ++i) {
            field_index[fields[i]] = i;
        }
        
        auto get_string = [&](const json& item, const std::string& field_name) -> std::string {
            auto it = field_index.find(field_name);
            if (it != field_index.end() && it->second < item.size() && !item[it->second].is_null()) {
                return item[it->second].get<std::string>();
            }
            return "";
        };
        
        auto get_int = [&](const json& item, const std::string& field_name) -> int {
            auto it = field_index.find(field_name);
            if (it != field_index.end() && it->second < item.size() && !item[it->second].is_null()) {
                return item[it->second].get<int>();
            }
            return 0;
        };
        
        auto get_double = [&](const json& item, const std::string& field_name) -> double {
            auto it = field_index.find(field_name);
            if (it != field_index.end() && it->second < item.size() && !item[it->second].is_null()) {
                return item[it->second].get<double>();
            }
            return 0.0;
        };
        
        for (const auto& item : items) {
            MoneyFlow flow;
            
            flow.ts_code = get_string(item, "ts_code");
            flow.trade_date = get_string(item, "trade_date");
            
            // 小单
            flow.buy_sm_vol = get_int(item, "buy_sm_vol");
            flow.buy_sm_amount = get_double(item, "buy_sm_amount");
            flow.sell_sm_vol = get_int(item, "sell_sm_vol");
            flow.sell_sm_amount = get_double(item, "sell_sm_amount");
            
            // 中单
            flow.buy_md_vol = get_int(item, "buy_md_vol");
            flow.buy_md_amount = get_double(item, "buy_md_amount");
            flow.sell_md_vol = get_int(item, "sell_md_vol");
            flow.sell_md_amount = get_double(item, "sell_md_amount");
            
            // 大单
            flow.buy_lg_vol = get_int(item, "buy_lg_vol");
            flow.buy_lg_amount = get_double(item, "buy_lg_amount");
            flow.sell_lg_vol = get_int(item, "sell_lg_vol");
            flow.sell_lg_amount = get_double(item, "sell_lg_amount");
            
            // 特大单
            flow.buy_elg_vol = get_int(item, "buy_elg_vol");
            flow.buy_elg_amount = get_double(item, "buy_elg_amount");
            flow.sell_elg_vol = get_int(item, "sell_elg_vol");
            flow.sell_elg_amount = get_double(item, "sell_elg_amount");
            
            // 净流入
            flow.net_mf_vol = get_int(item, "net_mf_vol");
            flow.net_mf_amount = get_double(item, "net_mf_amount");
            
            result.push_back(flow);
        }
        
        LOG_INFO("解析资金流向成功，共 " + std::to_string(result.size()) + " 条");
        
    } catch (const json::exception& e) {
        LOG_ERROR("解析资金流向失败: " + std::string(e.what()));
    }
    
    return result;
}
```

**Step 3: Compile**

Run: `cd cpp/build && cmake .. && make -j4`

Expected: Compilation success.

**Step 4: Commit**

```bash
git add cpp/network/TushareDataSource.h cpp/network/TushareDataSource.cpp
git commit -m "feat(network): implement getMoneyFlow in TushareDataSource"
```

---

## Task 5: Add Database Migration

**Files:**
- Create: `backend/prisma/migrations/<timestamp>_add_money_flow_table/migration.sql`
- Modify: `backend/prisma/schema.prisma`

**Step 1: Add Prisma schema**

Add to `backend/prisma/schema.prisma`:

```prisma
model MoneyFlow {
  id             Int      @id @default(autoincrement())
  tsCode         String   @db.VarChar(20) @map("ts_code")
  tradeDate      DateTime @db.Date @map("trade_date")
  
  // 小单
  buySmVol       Int?     @map("buy_sm_vol")
  buySmAmount    Float?   @map("buy_sm_amount")
  sellSmVol      Int?     @map("sell_sm_vol")
  sellSmAmount   Float?   @map("sell_sm_amount")
  
  // 中单
  buyMdVol       Int?     @map("buy_md_vol")
  buyMdAmount    Float?   @map("buy_md_amount")
  sellMdVol      Int?     @map("sell_md_vol")
  sellMdAmount   Float?   @map("sell_md_amount")
  
  // 大单
  buyLgVol       Int?     @map("buy_lg_vol")
  buyLgAmount    Float?   @map("buy_lg_amount")
  sellLgVol      Int?     @map("sell_lg_vol")
  sellLgAmount   Float?   @map("sell_lg_amount")
  
  // 特大单
  buyElgVol      Int?     @map("buy_elg_vol")
  buyElgAmount   Float?   @map("buy_elg_amount")
  sellElgVol     Int?     @map("sell_elg_vol")
  sellElgAmount  Float?   @map("sell_elg_amount")
  
  // 净流入
  netMfVol       Int?     @map("net_mf_vol")
  netMfAmount    Float?   @map("net_mf_amount")
  
  createdAt      DateTime @default(now()) @map("created_at")
  
  @@unique([tsCode, tradeDate], name: "unique_stock_date")
  @@index([tradeDate], name: "idx_trade_date")
  @@index([tsCode], name: "idx_ts_code")
  @@map("money_flow")
}
```

**Step 2: Generate migration**

Run: `cd backend && npx prisma migrate dev --name add_money_flow_table`

Expected: Migration created and applied.

**Step 3: Commit**

```bash
git add backend/prisma/schema.prisma backend/prisma/migrations/
git commit -m "feat(db): add MoneyFlow table schema and migration"
```

---

## Task 6: Create MoneyFlowDAO

**Files:**
- Create: `cpp/data/database/MoneyFlowDAO.h`
- Create: `cpp/data/database/MoneyFlowDAO.cpp`
- Modify: `cpp/data/Data.h`

**Step 1: Create MoneyFlowDAO.h**

```cpp
#ifndef MONEY_FLOW_DAO_H
#define MONEY_FLOW_DAO_H

#include "Connection.h"
#include "../../core/FundamentalData.h"
#include <vector>

namespace data {
namespace database {

using MoneyFlow = core::MoneyFlow;

class MoneyFlowDAO {
public:
    explicit MoneyFlowDAO(Connection& conn);
    
    /**
     * @brief 创建表
     */
    void createTable();
    
    /**
     * @brief 插入资金流向数据
     * @param flow 资金流向数据
     * @return 是否成功
     */
    bool insert(const MoneyFlow& flow);
    
    /**
     * @brief 批量插入
     * @param flows 资金流向列表
     * @return 成功插入的数量
     */
    int batchInsert(const std::vector<MoneyFlow>& flows);
    
    /**
     * @brief 根据股票代码查询
     * @param ts_code 股票代码
     * @param start_date 开始日期（可选）
     * @param end_date 结束日期（可选）
     * @return 资金流向列表
     */
    std::vector<MoneyFlow> findByTsCode(
        const std::string& ts_code,
        const std::string& start_date = "",
        const std::string& end_date = ""
    );
    
    /**
     * @brief 根据交易日期查询
     * @param trade_date 交易日期
     * @return 资金流向列表
     */
    std::vector<MoneyFlow> findByTradeDate(const std::string& trade_date);
    
private:
    Connection& conn_;
};

} // namespace database
} // namespace data

#endif // MONEY_FLOW_DAO_H
```

**Step 2: Create MoneyFlowDAO.cpp**

```cpp
#include "MoneyFlowDAO.h"
#include "Logger.h"
#include <sstream>

namespace data {
namespace database {

MoneyFlowDAO::MoneyFlowDAO(Connection& conn) : conn_(conn) {}

void MoneyFlowDAO::createTable() {
    std::string sql = 
        "CREATE TABLE IF NOT EXISTS money_flow ("
        "id INT AUTO_INCREMENT PRIMARY KEY,"
        "ts_code VARCHAR(20) NOT NULL,"
        "trade_date DATE NOT NULL,"
        
        // 小单
        "buy_sm_vol INT,"
        "buy_sm_amount DECIMAL(12,2),"
        "sell_sm_vol INT,"
        "sell_sm_amount DECIMAL(12,2),"
        
        // 中单
        "buy_md_vol INT,"
        "buy_md_amount DECIMAL(12,2),"
        "sell_md_vol INT,"
        "sell_md_amount DECIMAL(12,2),"
        
        // 大单
        "buy_lg_vol INT,"
        "buy_lg_amount DECIMAL(12,2),"
        "sell_lg_vol INT,"
        "sell_lg_amount DECIMAL(12,2),"
        
        // 特大单
        "buy_elg_vol INT,"
        "buy_elg_amount DECIMAL(12,2),"
        "sell_elg_vol INT,"
        "sell_elg_amount DECIMAL(12,2),"
        
        // 净流入
        "net_mf_vol INT,"
        "net_mf_amount DECIMAL(12,2),"
        
        "created_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
        
        "UNIQUE KEY unique_stock_date (ts_code, trade_date),"
        "INDEX idx_trade_date (trade_date),"
        "INDEX idx_ts_code (ts_code)"
        ")";
    
    conn_.execute(sql);
    LOG_INFO("money_flow 表创建成功");
}

bool MoneyFlowDAO::insert(const MoneyFlow& flow) {
    std::ostringstream sql;
    sql << "INSERT INTO money_flow (ts_code, trade_date, "
        << "buy_sm_vol, buy_sm_amount, sell_sm_vol, sell_sm_amount, "
        << "buy_md_vol, buy_md_amount, sell_md_vol, sell_md_amount, "
        << "buy_lg_vol, buy_lg_amount, sell_lg_vol, sell_lg_amount, "
        << "buy_elg_vol, buy_elg_amount, sell_elg_vol, sell_elg_amount, "
        << "net_mf_vol, net_mf_amount) VALUES ("
        << "'" << flow.ts_code << "', '" << flow.trade_date << "', "
        << flow.buy_sm_vol << ", " << flow.buy_sm_amount << ", "
        << flow.sell_sm_vol << ", " << flow.sell_sm_amount << ", "
        << flow.buy_md_vol << ", " << flow.buy_md_amount << ", "
        << flow.sell_md_vol << ", " << flow.sell_md_amount << ", "
        << flow.buy_lg_vol << ", " << flow.buy_lg_amount << ", "
        << flow.sell_lg_vol << ", " << flow.sell_lg_amount << ", "
        << flow.buy_elg_vol << ", " << flow.buy_elg_amount << ", "
        << flow.sell_elg_vol << ", " << flow.sell_elg_amount << ", "
        << flow.net_mf_vol << ", " << flow.net_mf_amount
        << ") ON DUPLICATE KEY UPDATE "
        << "buy_sm_vol=" << flow.buy_sm_vol << ", "
        << "net_mf_vol=" << flow.net_mf_vol;
    
    return conn_.execute(sql.str());
}

int MoneyFlowDAO::batchInsert(const std::vector<MoneyFlow>& flows) {
    int successCount = 0;
    for (const auto& flow : flows) {
        if (insert(flow)) {
            successCount++;
        }
    }
    LOG_INFO("批量插入资金流向: " + std::to_string(successCount) + "/" + std::to_string(flows.size()));
    return successCount;
}

std::vector<MoneyFlow> MoneyFlowDAO::findByTsCode(
    const std::string& ts_code,
    const std::string& start_date,
    const std::string& end_date) {
    
    std::vector<MoneyFlow> result;
    std::ostringstream sql;
    sql << "SELECT * FROM money_flow WHERE ts_code='" << ts_code << "'";
    
    if (!start_date.empty()) {
        sql << " AND trade_date >= '" << start_date << "'";
    }
    if (!end_date.empty()) {
        sql << " AND trade_date <= '" << end_date << "'";
    }
    sql << " ORDER BY trade_date DESC";
    
    // TODO: 实现查询结果解析
    // 需要根据 Connection 的返回类型处理
    
    return result;
}

std::vector<MoneyFlow> MoneyFlowDAO::findByTradeDate(const std::string& trade_date) {
    return findByTsCode("", trade_date, trade_date);
}

} // namespace database
} // namespace data
```

**Step 3: Add to Data.h**

Add include to `cpp/data/Data.h`:

```cpp
#include "database/MoneyFlowDAO.h"
```

**Step 4: Update CMakeLists.txt**

Add to `cpp/CMakeLists.txt` source files:

```cmake
data/database/MoneyFlowDAO.cpp
```

**Step 5: Compile**

Run: `cd cpp/build && cmake .. && make -j4`

Expected: Compilation success.

**Step 6: Commit**

```bash
git add cpp/data/database/MoneyFlowDAO.h cpp/data/database/MoneyFlowDAO.cpp cpp/data/Data.h cpp/CMakeLists.txt
git commit -m "feat(data): add MoneyFlowDAO for database operations"
```

---

## Task 7: Push and Test

**Step 1: Push changes**

Run: `git push origin master`

**Step 2: Verify in production**

After deployment, test the API:

```bash
# Test moneyflow API (via Node.js service or directly)
curl "http://localhost:3000/api/moneyflow?trade_date=20240501"
```

**Step 3: Final commit message**

```bash
git log --oneline -7
```

Expected: All commits visible.

---

## Summary

| Task | Description | Files |
|------|-------------|-------|
| 1 | Data structure | FundamentalData.h, IDataSource.h |
| 2 | HTTP method | TushareClient.h/cpp |
| 3 | Interface method | IDataSource.h |
| 4 | Implementation | TushareDataSource.h/cpp |
| 5 | Database schema | schema.prisma, migration |
| 6 | DAO layer | MoneyFlowDAO.h/cpp |
| 7 | Push and test | - |