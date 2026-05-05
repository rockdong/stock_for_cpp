# Core 模块 - 核心数据结构

## 📋 概述

Core 模块提供系统的核心数据结构，是整个系统的数据基础。所有模块都应该使用 Core 模块定义的数据结构，避免重复定义。

## 🎯 设计目标

### 问题
在重构之前，系统中存在多个相似的数据结构定义：
- `network::StockBasic` - 网络层的股票信息
- `data::Stock` - 数据层的股票信息

这导致了：
- ❌ 代码重复
- ❌ 维护困难
- ❌ 类型不一致
- ❌ 转换复杂

### 解决方案
创建统一的 Core 模块，定义核心数据结构：
- ✅ 单一数据源
- ✅ 统一维护
- ✅ 类型一致
- ✅ 易于扩展

## 🏗️ 架构设计

```
┌─────────────────────────────────────────────────────────┐
│                    Core 模块（核心层）                    │
│                                                           │
│  ┌─────────────────────────────────────────────────┐   │
│  │  Stock - 股票基本信息（核心数据结构）            │   │
│  │  - ts_code, symbol, name, area, industry...     │   │
│  └─────────────────────────────────────────────────┘   │
│                          ▲                               │
│                          │                               │
│  ┌───────────────────────┴──────────────────────────┐   │
│  │                                                   │   │
│  │  StockEntity - 数据库实体（扩展核心结构）         │   │
│  │  - id, Stock, created_at, updated_at            │   │
│  │                                                   │   │
│  │  StockData - 股票行情数据                         │   │
│  │  - ts_code, trade_date, open, high, low...      │   │
│  └───────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────┘
                          ▲
                          │
        ┌─────────────────┼─────────────────┐
        │                 │                 │
┌───────▼──────┐  ┌───────▼──────┐  ┌──────▼───────┐
│ Network 模块  │  │  Data 模块   │  │  Core 模块   │
│              │  │              │  │              │
│ using Stock  │  │ using Stock  │  │ 业务逻辑处理  │
│ = core::Stock│  │ = core::Stock│  │              │
└──────────────┘  └──────────────┘  └──────────────┘
```

## 📦 数据结构

### 1. Stock - 股票基本信息

**核心数据结构**，包含股票的业务信息，不包含数据库特定字段。

```cpp
namespace core {

struct Stock {
    std::string ts_code;        // TS代码（如 000001.SZ）
    std::string symbol;         // 股票代码（如 000001）
    std::string name;           // 股票名称
    std::string area;           // 地域
    std::string industry;       // 所属行业
    std::string fullname;       // 股票全称
    std::string enname;         // 英文全称
    std::string cnspell;        // 拼音缩写
    std::string market;         // 市场类型
    std::string exchange;       // 交易所代码
    std::string curr_type;      // 交易货币
    std::string list_status;    // 上市状态
    std::string list_date;      // 上市日期
    std::string delist_date;    // 退市日期
    std::string is_hs;          // 是否沪深港通标的
};

} // namespace core
```

**使用场景**：
- Network 模块：从 API 获取数据
- Data 模块：业务数据传递
- Core 模块：业务逻辑处理

### 2. StockEntity - 数据库实体

**扩展核心结构**，在 Stock 基础上添加数据库特定字段。

```cpp
namespace core {

struct StockEntity {
    int id = 0;                 // 数据库主键
    Stock stock;                // 核心股票信息
    std::string created_at;     // 创建时间
    std::string updated_at;     // 更新时间
};

} // namespace core
```

**使用场景**：
- Data 模块：数据库持久化
- 需要数据库元信息的场景

### 3. StockData - 股票行情数据

**行情数据结构**，包含股票的价格和成交信息。

```cpp
namespace core {

struct StockData {
    std::string ts_code;        // 股票代码
    std::string trade_date;     // 交易日期
    double open;                // 开盘价
    double high;                // 最高价
    double low;                 // 最低价
    double close;               // 收盘价
    double pre_close;           // 昨收价
    double change;              // 涨跌额
    double pct_chg;             // 涨跌幅
    long volume;                // 成交量（手）
    double amount;              // 成交额（千元）
};

} // namespace core
```

**使用场景**：
- Network 模块：获取行情数据
- Analysis 模块：技术分析
- Utils 模块：数据提取

### 4. FinancialIndicator - 财务指标数据

**财务指标数据结构**，包含公司的估值、盈利能力、增长能力等指标。

```cpp
namespace core {

struct FinancialIndicator {
    std::string ts_code;            // 股票代码
    std::string ann_date;           // 公告日期
    std::string end_date;           // 报告期
    
    // 估值指标
    double pe;                      // 市盈率
    double pe_ttm;                  // 市盈率TTM
    double pb;                      // 市净率
    double ps;                      // 市销率
    double pcf;                     // 市现率
    
    // 盈利能力指标
    double roe;                     // 净资产收益率
    double roa;                     // 总资产收益率
    double grossprofit_margin;      // 毛利率
    double netprofit_margin;        // 净利率
    
    // 增长能力指标
    double or_yoy;                  // 营业收入同比增长
    double netprofit_yoy;           // 净利润同比增长
    
    // 健康指标
    double debt_to_assets;          // 资产负债率
    double current_ratio;           // 流动比率
};

} // namespace core
```

**使用场景**：
- Network 模块：获取财务指标数据
- Core 模块：基本面分析、股票筛选

### 5. MoneyFlow - 资金流向数据

**资金流向数据结构**，包含股票的资金买卖信息。

```cpp
namespace core {

struct MoneyFlow {
    std::string ts_code;            // TS股票代码
    std::string trade_date;         // 交易日期
    
    // 小单（<5万）
    int buy_sm_vol;                 // 小单买入量（手）
    double buy_sm_amount;           // 小单买入金额（万元）
    
    // 中单（5万~20万）、大单（20万~100万）、特大单（>=100万）...
    
    // 净流入
    int net_mf_vol;                 // 净流入量（手）
    double net_mf_amount;           // 净流入额（万元）
};

} // namespace core
```

**使用场景**：
- Network 模块：获取资金流向数据
- Analysis 模块：资金流向分析

### 6. SuspendInfo - 停复牌信息

**停复牌数据结构**，记录股票的停牌和复牌信息。

```cpp
namespace core {

struct SuspendInfo {
    std::string ts_code;            // TS股票代码
    std::string suspend_date;       // 停牌日期
    std::string resume_date;        // 复牌日期
    std::string suspend_type;       // 停牌类型 (S-停牌, R-复牌)
    std::string suspend_reason;     // 停牌原因
};

} // namespace core
```

**使用场景**：
- Network 模块：获取停复牌信息
- Analysis 模块：停牌风险分析

### 7. Forecast - 业绩预告

**业绩预告数据结构**，记录上市公司的业绩预告信息。

```cpp
namespace core {

struct Forecast {
    std::string ts_code;            // TS股票代码
    std::string ann_date;           // 公告日期
    std::string end_date;           // 报告期
    std::string type;               // 预告类型
    double p_change_min;            // 预告净利润变动幅度下限 (%)
    double p_change_max;            // 预告净利润变动幅度上限 (%)
    double net_profit_min;          // 预告净利润下限 (万元)
    double net_profit_max;          // 预告净利润上限 (万元)
    std::string summary;            // 预告摘要
    std::string change_reason;      // 变动原因
};

} // namespace core
```

**使用场景**：
- Network 模块：获取业绩预告
- Analysis 模块：业绩预测分析

### 8. Express - 业绩快报

**业绩快报数据结构**，记录上市公司的业绩快报数据。

```cpp
namespace core {

struct Express {
    std::string ts_code;            // TS股票代码
    std::string ann_date;           // 公告日期
    std::string end_date;           // 报告期
    double revenue;                 // 营业收入 (万元)
    double operate_profit;          // 营业利润 (万元)
    double total_profit;            // 利润总额 (万元)
    double n_income;                // 净利润 (万元)
    double total_assets;            // 总资产 (万元)
    double total_share;             // 总股本 (万股)
    double eps;                     // 每股收益
};

} // namespace core
```

**使用场景**：
- Network 模块：获取业绩快报
- Analysis 模块：业绩跟踪分析

### 9. Dividend - 分红送股

**分红送股数据结构**，记录上市公司的分红送股信息。

```cpp
namespace core {

struct Dividend {
    std::string ts_code;            // TS股票代码
    std::string ann_date;           // 公告日期
    std::string end_date;           // 报告期/年度
    std::string div_proc;           // 实施进度
    double stk_div;                 // 送股比例 (每股送股)
    double stk_bo_rate;             // 转增比例 (每股转增)
    double cash_div;                // 每股现金分红 (元)
    std::string ex_date;            // 除权除息日
    std::string pay_date;           // 派息日
};

} // namespace core
```

**使用场景**：
- Network 模块：获取分红送股数据
- Analysis 模块：分红收益分析

## 🔧 使用方式

### 1. Network 模块

```cpp
// network/IDataSource.h
#include "../core/Stock.h"

namespace network {

// 使用核心模块的数据结构
using Stock = core::Stock;
using StockData = core::StockData;

class IDataSource {
public:
    virtual std::vector<Stock> getStockList() = 0;
    virtual std::vector<StockData> getDailyData(...) = 0;
};

} // namespace network
```

### 2. Data 模块

```cpp
// data/database/StockDAO.h
#include "../../core/Stock.h"

namespace data {

// 使用核心模块的数据结构
using Stock = core::Stock;
using StockEntity = core::StockEntity;

class StockDAO {
public:
    bool insert(const Stock& stock);
    std::vector<Stock> findAll();
};

} // namespace data
```

### 3. 业务代码

```cpp
#include "Core.h"
#include "Network.h"
#include "Data.h"

int main() {
    // 从网络获取数据
    auto dataSource = network::DataSourceFactory::createFromConfig();
    std::vector<core::Stock> stocks = dataSource->getStockList();
    
    // 存储到数据库
    data::StockDAO stockDao;
    for (const auto& stock : stocks) {
        stockDao.insert(stock);  // 直接使用，无需转换
    }
    
    // 从数据库读取
    std::vector<core::Stock> dbStocks = stockDao.findAll();
    
    return 0;
}
```

## ✨ 优势

### 1. 单一数据源
- ✅ 所有模块使用相同的数据结构
- ✅ 避免重复定义
- ✅ 统一维护管理

### 2. 类型一致
- ✅ 编译期类型检查
- ✅ 无需类型转换
- ✅ 减少错误

### 3. 易于扩展
- ✅ 添加新字段只需修改一处
- ✅ 所有模块自动同步
- ✅ 降低维护成本

### 4. 清晰的职责
- ✅ Core 模块：定义数据结构
- ✅ Network 模块：获取数据
- ✅ Data 模块：存储数据
- ✅ 业务模块：处理数据

## 🔄 迁移指南

### 从旧代码迁移

**之前的代码**：
```cpp
// network/IDataSource.h
struct StockBasic {
    std::string ts_code;
    std::string name;
    // ...
};

// data/database/StockDAO.h
struct Stock {
    int id;
    std::string ts_code;
    std::string name;
    // ...
};

// 需要转换函数
Stock convertToDataStock(const StockBasic& basic) {
    Stock stock;
    stock.ts_code = basic.ts_code;
    stock.name = basic.name;
    // ...
    return stock;
}
```

**现在的代码**：
```cpp
// 所有模块都使用 core::Stock
#include "Core.h"

// network 模块
using Stock = core::Stock;

// data 模块
using Stock = core::Stock;

// 无需转换，直接使用
```

## 📊 模块依赖关系

```
Core 模块（核心层）
    ↑
    │ 依赖
    │
    ├─── Network 模块（网络层）
    ├─── Data 模块（数据层）
    ├─── Analysis 模块（分析层）
    └─── 其他业务模块
```

**依赖原则**：
- ✅ 其他模块依赖 Core 模块
- ✅ Core 模块不依赖任何业务模块
- ✅ Core 模块只包含数据结构定义

## 🎯 最佳实践

### 1. 使用 using 声明

```cpp
// 推荐：使用 using 声明
namespace network {
    using Stock = core::Stock;
}

// 不推荐：直接使用 core::Stock
namespace network {
    std::vector<core::Stock> getStockList();
}
```

### 2. 保持 Core 模块纯净

```cpp
// ✅ 正确：只包含数据结构
namespace core {
    struct Stock {
        std::string ts_code;
        std::string name;
    };
}

// ❌ 错误：不要在 Core 模块中添加业务逻辑
namespace core {
    struct Stock {
        std::string ts_code;
        std::string name;
        
        void save();  // 错误！业务逻辑应该在其他模块
    };
}
```

### 3. 扩展而不是修改

```cpp
// ✅ 正确：通过组合扩展
namespace data {
    struct StockEntity {
        int id;
        core::Stock stock;  // 组合核心结构
        std::string created_at;
    };
}

// ❌ 错误：不要修改核心结构
namespace core {
    struct Stock {
        int id;  // 错误！数据库字段不应该在核心结构中
        std::string ts_code;
    };
}
```

## 📝 文件结构

```
core/
├── Stock.h              # 股票核心数据结构
├── FundamentalData.h    # 财务/基本面数据结构（新增）
│   ├── FinancialIndicator   # 财务指标
│   ├── MoneyFlow             # 资金流向
│   ├── SuspendInfo           # 停复牌信息（新增）
│   ├── Forecast              # 业绩预告（新增）
│   ├── Express               # 业绩快报（新增）
│   ├── Dividend              # 分红送股（新增）
│   └── FundamentalScore      # 基本面评分
├── Core.h               # 统一头文件
└── README.md            # 本文档
```

## 🎉 最近更新

### v1.1.0 (2026-05-04)
- ✅ 新增 `SuspendInfo` 停复牌信息数据结构
- ✅ 新增 `Forecast` 业绩预告数据结构
- ✅ 新增 `Express` 业绩快报数据结构
- ✅ 新增 `Dividend` 分红送股数据结构
- ✅ 所有新数据结构已集成到 Network 模块

## 🔗 相关文档

- [DESIGN.md](../DESIGN.md) - 系统设计文档
- [network/README.md](../network/README.md) - 网络层文档
- [data/README.md](../data/README.md) - 数据层文档

---

**版本**: 1.1.0  
**创建日期**: 2026-02-04  
**最后更新**: 2026-05-04  
**维护者**: Development Team

