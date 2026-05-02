# MoneyFlow API 设计文档

## 概述

新增 Tushare `moneyflow` 接口（个股资金流向），用于策略分析。

## 需求

| 项目 | 要求 |
|------|------|
| 用途 | 策略分析 |
| 存储 | 需要存储到数据库 |
| 字段 | 全部18个字段 |
| 调用方式 | 单日批量查询 + 单股历史查询 |

## 数据结构

### MoneyFlow 结构体

位置：`cpp/core/FundamentalData.h`

```cpp
struct MoneyFlow {
    std::string ts_code;           // 股票代码
    std::string trade_date;        // 交易日期
    
    // 小单（<5万）
    int buy_sm_vol;                // 小单买入量（手）
    double buy_sm_amount;          // 小单买入金额（万元）
    int sell_sm_vol;               // 小单卖出量（手）
    double sell_sm_amount;         // 小单卖出金额（万元）
    
    // 中单（5万~20万）
    int buy_md_vol;                // 中单买入量（手）
    double buy_md_amount;          // 中单买入金额（万元）
    int sell_md_vol;               // 中单卖出量（手）
    double sell_md_amount;         // 中单卖出金额（万元）
    
    // 大单（20万~100万）
    int buy_lg_vol;                // 大单买入量（手）
    double buy_lg_amount;          // 大单买入金额（万元）
    int sell_lg_vol;               // 大单卖出量（手）
    double sell_lg_amount;         // 大单卖出金额（万元）
    
    // 特大单（>=100万）
    int buy_elg_vol;               // 特大单买入量（手）
    double buy_elg_amount;         // 特大单买入金额（万元）
    int sell_elg_vol;              // 特大单卖出量（手）
    double sell_elg_amount;        // 特大单卖出金额（万元）
    
    // 净流入
    int net_mf_vol;                // 净流入量（手）
    double net_mf_amount;          // 净流入额（万元）
};
```

## 模块改动

### 1. HTTP层

文件：`cpp/network/http/TushareClient.h/cpp`

新增方法：
```cpp
TushareResponse getMoneyFlow(
    const std::string& ts_code = "",
    const std::string& trade_date = "",
    const std::string& start_date = "",
    const std::string& end_date = ""
);
```

### 2. 接口层

文件：`cpp/network/IDataSource.h`

新增虚方法：
```cpp
virtual std::vector<MoneyFlow> getMoneyFlow(
    const std::string& ts_code = "",
    const std::string& trade_date = "",
    const std::string& start_date = "",
    const std::string& end_date = ""
) = 0;
```

### 3. 实现层

文件：`cpp/network/TushareDataSource.h/cpp`

- 实现 `getMoneyFlow()` 方法
- 新增 `parseMoneyFlow()` 解析方法

### 4. 数据库层

文件：`cpp/data/database/MoneyFlowDAO.h/cpp`

表结构：
```sql
CREATE TABLE money_flow (
    id INT AUTO_INCREMENT PRIMARY KEY,
    ts_code VARCHAR(20) NOT NULL,
    trade_date DATE NOT NULL,
    
    -- 小单
    buy_sm_vol INT,
    buy_sm_amount DECIMAL(12,2),
    sell_sm_vol INT,
    sell_sm_amount DECIMAL(12,2),
    
    -- 中单
    buy_md_vol INT,
    buy_md_amount DECIMAL(12,2),
    sell_md_vol INT,
    sell_md_amount DECIMAL(12,2),
    
    -- 大单
    buy_lg_vol INT,
    buy_lg_amount DECIMAL(12,2),
    sell_lg_vol INT,
    sell_lg_amount DECIMAL(12,2),
    
    -- 特大单
    buy_elg_vol INT,
    buy_elg_amount DECIMAL(12,2),
    sell_elg_vol INT,
    sell_elg_amount DECIMAL(12,2),
    
    -- 净流入
    net_mf_vol INT,
    net_mf_amount DECIMAL(12,2),
    
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    
    UNIQUE KEY unique_stock_date (ts_code, trade_date),
    INDEX idx_trade_date (trade_date),
    INDEX idx_ts_code (ts_code)
);
```

## 使用示例

```cpp
// 创建数据源
auto source = network::DataSourceFactory::createFromConfig();

// 方式1：单日批量查询
auto flows = source->getMoneyFlow("", "20240501", "", "");

// 方式2：单股历史查询
auto flows = source->getMoneyFlow("000001.SZ", "", "20240101", "20240501");

// 存储到数据库
MoneyFlowDAO dao(conn);
for (const auto& flow : flows) {
    dao.insert(flow);
}
```

## 文件清单

| 层级 | 文件 | 操作 |
|------|------|------|
| 数据结构 | `cpp/core/FundamentalData.h` | 修改 |
| HTTP层 | `cpp/network/http/TushareClient.h` | 修改 |
| HTTP层 | `cpp/network/http/TushareClient.cpp` | 修改 |
| 接口层 | `cpp/network/IDataSource.h` | 修改 |
| 实现层 | `cpp/network/TushareDataSource.h` | 修改 |
| 实现层 | `cpp/network/TushareDataSource.cpp` | 修改 |
| 数据库层 | `cpp/data/database/MoneyFlowDAO.h` | 新增 |
| 数据库层 | `cpp/data/database/MoneyFlowDAO.cpp` | 新增 |
| 数据库层 | `backend/prisma/schema.prisma` | 修改 |

## 设计原则

遵循现有代码的 SOLID 原则：
- **SRP**: 每个类只负责单一功能
- **OCP**: 通过接口扩展，不修改现有代码
- **LSP**: 所有 IDataSource 实现可互换
- **ISP**: 接口只包含必要方法
- **DIP**: 依赖抽象接口而非具体实现