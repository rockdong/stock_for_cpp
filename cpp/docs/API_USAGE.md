# Tushare API 使用文档

## 行情数据接口

### 概述

新增了统一的行情数据获取接口 `getQuoteData()`，支持获取日线、周线、月线数据。

### 接口说明

```cpp
std::vector<StockData> getQuoteData(
    const std::string& ts_code,      // 股票代码（如 000001.SZ）
    const std::string& start_date,   // 开始日期（YYYYMMDD）
    const std::string& end_date,     // 结束日期（YYYYMMDD）
    const std::string& freq = "d"    // 频率：d=日线, w=周线, m=月线
);
```

### 参数说明

| 参数 | 类型 | 必填 | 说明 |
|------|------|------|------|
| ts_code | string | 是 | 股票代码，如 000001.SZ |
| start_date | string | 否 | 开始日期，格式 YYYYMMDD |
| end_date | string | 否 | 结束日期，格式 YYYYMMDD |
| freq | string | 否 | 数据频率，默认 "d" |

### 频率参数

- `d` 或 `D`: 日线数据（默认）
- `w` 或 `W`: 周线数据
- `m` 或 `M`: 月线数据

### 使用示例

#### 1. 获取日线数据

```cpp
#include "DataSourceFactory.h"

// 创建数据源
auto dataSource = network::DataSourceFactory::createFromConfig();

// 获取日线数据
auto daily_data = dataSource->getQuoteData(
    "000001.SZ",    // 股票代码
    "20240101",     // 开始日期
    "20240131",     // 结束日期
    "d"             // 日线
);

// 遍历数据
for (const auto& data : daily_data) {
    std::cout << "日期: " << data.trade_date 
              << ", 收盘价: " << data.close 
              << std::endl;
}
```

#### 2. 获取周线数据

```cpp
// 获取周线数据
auto weekly_data = dataSource->getQuoteData(
    "000001.SZ",    // 股票代码
    "20240101",     // 开始日期
    "20240331",     // 结束日期
    "w"             // 周线
);

std::cout << "周线数据条数: " << weekly_data.size() << std::endl;
```

#### 3. 获取月线数据

```cpp
// 获取月线数据
auto monthly_data = dataSource->getQuoteData(
    "000001.SZ",    // 股票代码
    "20230101",     // 开始日期
    "20231231",     // 结束日期
    "m"             // 月线
);

std::cout << "月线数据条数: " << monthly_data.size() << std::endl;
```

#### 4. 使用默认参数（日线）

```cpp
// 不指定 freq 参数，默认获取日线数据
auto data = dataSource->getQuoteData("000001.SZ", "20240101", "20240131");
```

### 数据结构

返回的 `StockData` 结构包含以下字段：

```cpp
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
```

### 错误处理

```cpp
try {
    auto data = dataSource->getQuoteData("000001.SZ", "20240101", "20240131", "w");
    
    if (data.empty()) {
        LOG_WARN("未获取到数据");
    } else {
        LOG_INFO("获取到 " + std::to_string(data.size()) + " 条数据");
    }
} catch (const std::exception& e) {
    LOG_ERROR("获取数据失败: " + std::string(e.what()));
}
```

### 注意事项

1. **API Token**: 需要在 `.env` 文件中配置 `DATA_SOURCE_API_KEY`
2. **日期格式**: 必须使用 YYYYMMDD 格式（如 20240101）
3. **数据权限**: 周线和月线数据可能需要 Tushare 积分权限
4. **频率参数**: 不区分大小写，`d`、`D`、`w`、`W`、`m`、`M` 都有效
5. **返回顺序**: 数据按交易日期降序排列（最新的在前）

### 底层 API

如果需要更底层的控制，可以直接使用 `TushareClient`：

```cpp
auto client = dataSource->getClient();

// 日线
auto daily_response = client->getDailyQuote("000001.SZ", "", "20240101", "20240131");

// 周线
auto weekly_response = client->getWeeklyQuote("000001.SZ", "", "20240101", "20240331");

// 月线
auto monthly_response = client->getMonthlyQuote("000001.SZ", "", "20230101", "20231231");
```

## 参考文档

- [Tushare Pro 日线行情](https://tushare.pro/document/2?doc_id=27)
- [Tushare Pro 周线行情](https://tushare.pro/document/2?doc_id=144)
- [Tushare Pro 月线行情](https://tushare.pro/document/2?doc_id=145)

