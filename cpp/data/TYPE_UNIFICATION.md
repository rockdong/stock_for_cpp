# Data 层类型统一说明

## 问题

之前 `dataSource->getStockList()` 返回 `std::vector<network::StockBasic>`，而 `stockDao.findAll()` 返回 `std::vector<data::Stock>`，两者类型不一致。

## 解决方案

已将 `data::Stock` 结构体更新为与 `network::StockBasic` 完全一致的字段，包括：

### 字段列表
- `ts_code` - TS代码（如 000001.SZ）
- `symbol` - 股票代码（如 000001）
- `name` - 股票名称
- `area` - 地域
- `industry` - 所属行业
- `fullname` - 股票全称
- `enname` - 英文全称
- `cnspell` - 拼音缩写
- `market` - 市场类型
- `exchange` - 交易所代码
- `curr_type` - 交易货币
- `list_status` - 上市状态
- `list_date` - 上市日期
- `delist_date` - 退市日期
- `is_hs` - 是否沪深港通标的

## 使用方法

### 方法1：使用转换函数（推荐）

```cpp
#include "Data.h"
#include "Network.h"

// 从网络获取股票列表
auto dataSource = network::DataSourceFactory::createFromConfig();
auto stockBasics = dataSource->getStockList();

// 转换为 data::Stock 类型
auto stocks = data::convertFromStockBasics(stockBasics);

// 保存到数据库
data::StockDAO stockDao;
for (const auto& stock : stocks) {
    stockDao.insert(stock);
}

// 从数据库读取（返回相同类型）
auto dbStocks = stockDao.findAll();
```

### 方法2：手动转换

```cpp
#include "Data.h"
#include "Network.h"

auto dataSource = network::DataSourceFactory::createFromConfig();
auto stockBasics = dataSource->getStockList();

data::StockDAO stockDao;
for (const auto& sb : stockBasics) {
    data::Stock stock;
    stock.ts_code = sb.ts_code;
    stock.symbol = sb.symbol;
    stock.name = sb.name;
    stock.area = sb.area;
    stock.industry = sb.industry;
    stock.fullname = sb.fullname;
    stock.enname = sb.enname;
    stock.cnspell = sb.cnspell;
    stock.market = sb.market;
    stock.exchange = sb.exchange;
    stock.curr_type = sb.curr_type;
    stock.list_status = sb.list_status;
    stock.list_date = sb.list_date;
    stock.delist_date = sb.delist_date;
    stock.is_hs = sb.is_hs;
    
    stockDao.insert(stock);
}
```

## 数据库表结构

已更新 `stocks` 表以支持所有字段：

```sql
CREATE TABLE IF NOT EXISTS stocks (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    ts_code TEXT NOT NULL UNIQUE,
    symbol TEXT,
    name TEXT,
    area TEXT,
    industry TEXT,
    fullname TEXT,
    enname TEXT,
    cnspell TEXT,
    market TEXT,
    exchange TEXT,
    curr_type TEXT,
    list_status TEXT,
    list_date TEXT,
    delist_date TEXT,
    is_hs TEXT,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME DEFAULT CURRENT_TIMESTAMP
)
```

## 注意事项

1. **类型一致性**：现在 `data::Stock` 和 `network::StockBasic` 字段完全一致（除了 `data::Stock` 多了 `id`, `created_at`, `updated_at` 三个数据库字段）

2. **转换函数**：提供了 `data::convertFromStockBasic()` 和 `data::convertFromStockBasics()` 两个转换函数

3. **数据库迁移**：如果之前已经创建了数据库，需要删除旧的 `stocks` 表或重新创建数据库

4. **编译依赖**：`data_lib` 现在依赖 `network` 头文件（仅用于类型定义，不链接）

## 更新的文件

- `data/database/StockDAO.h` - 更新 Stock 结构体
- `data/database/StockTable.h` - 更新表定义
- `data/database/StockDAO.cpp` - 更新 CRUD 操作
- `data/database/Connection.cpp` - 更新建表 SQL
- `data/Data.h` - 添加转换函数声明
- `data/Data.cpp` - 实现转换函数
- `CMakeLists.txt` - 添加 Data.cpp 和 network 头文件路径

