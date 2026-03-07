# 网络层模块 (Network Layer)

## 📋 模块概述

网络层模块提供了完整的数据获取功能，封装了 HTTP 客户端和 Tushare Pro API 接口，遵循 SOLID 原则和设计模式。

**命名空间**: `network`  
**依赖**: cpp-httplib, nlohmann/json, logger, config  
**状态**: ✅ 框架完成

## 🎯 设计原则

### SOLID 原则应用

1. **单一职责原则 (SRP)**
   - `HttpClient`: 只负责 HTTP 请求
   - `TushareClient`: 只负责 Tushare API 调用
   - `TushareDataSource`: 只负责数据源实现
   - `DataSourceFactory`: 只负责创建数据源

2. **开闭原则 (OCP)**
   - 通过 `IDataSource` 接口扩展新的数据源
   - 无需修改现有代码即可添加新功能

3. **里氏替换原则 (LSP)**
   - 所有 `IDataSource` 实现都可以互相替换
   - 保证接口行为一致性

4. **接口隔离原则 (ISP)**
   - `IDataSource` 只定义必要的数据获取方法
   - 不强制实现不需要的功能

5. **依赖倒置原则 (DIP)**
   - 依赖抽象接口 `IDataSource` 而非具体实现
   - 通过工厂模式注入依赖

### 设计模式

- **工厂模式**: `DataSourceFactory` 创建数据源
- **策略模式**: `IDataSource` 接口支持多种数据源策略
- **适配器模式**: `TushareDataSource` 适配 Tushare API

## 🏗️ 架构设计

```
network/
├── http/                       # HTTP 层
│   ├── HttpClient.h/cpp       # HTTP 客户端（封装 cpp-httplib）
│   └── TushareClient.h/cpp    # Tushare API 客户端
│
├── IDataSource.h              # 数据源接口（抽象基类）
├── TushareDataSource.h/cpp    # Tushare 数据源实现
├── DataSourceFactory.h/cpp    # 数据源工厂
└── Network.h                  # 统一头文件
```

## 📦 核心类

### 1. HttpClient - HTTP 客户端

封装 cpp-httplib，提供简单易用的 HTTP 请求接口。

**特性**:
- ✅ 支持 HTTP/HTTPS
- ✅ 自动重试机制
- ✅ 超时控制
- ✅ 请求头管理

**示例**:
```cpp
#include "http/HttpClient.h"

// 创建客户端
network::HttpClient client("http://api.example.com", 30);

// GET 请求
auto response = client.get("/api/data");
if (response.isSuccess()) {
    std::cout << response.body << std::endl;
}

// POST 请求
std::string json_body = R"({"key": "value"})";
auto response = client.post("/api/submit", json_body);

// 配置重试
client.setRetryTimes(3);
client.setRetryDelay(1000);  // 1秒
```

### 2. TushareClient - Tushare API 客户端

封装 Tushare Pro API，提供类型安全的接口调用。

**特性**:
- ✅ 完整的 API 封装
- ✅ JSON 自动序列化/反序列化
- ✅ 错误处理
- ✅ 响应验证
- ✅ **从配置模块自动获取 URL 和 Token**

**API 分类**:
- 基础数据: 股票列表、交易日历
- 行情数据: 日线、复权因子、每日指标
- 财务数据: 利润表、资产负债表、现金流量表
- 市场数据: 沪深股通成份股

**示例**:
```cpp
#include "http/TushareClient.h"

// 方式1: 从配置模块创建（推荐）
network::TushareClient client;

// 方式2: 手动指定参数
network::TushareClient client("your_token_here", "http://api.waditu.com");

// 获取股票列表
auto response = client.getStockBasic("L", "");
if (response.isSuccess()) {
    auto data = response.data;
    // 处理数据...
}

// 获取日线行情
auto response = client.getDailyQuote("000001.SZ", "", "20240101", "20240131");

// 获取财务数据
auto response = client.getIncome("000001.SZ", "20231231", "", "");
```

### 3. IDataSource - 数据源接口

定义统一的数据获取接口，支持多种数据源实现。

**接口方法**:
```cpp
class IDataSource {
public:
    // 获取股票列表
    virtual std::vector<StockBasic> getStockList() = 0;
    
    // 获取日线数据
    virtual std::vector<StockData> getDailyData(
        const std::string& ts_code,
        const std::string& start_date,
        const std::string& end_date
    ) = 0;
    
    // 获取最新行情
    virtual StockData getLatestQuote(const std::string& ts_code) = 0;
    
    // 测试连接
    virtual bool testConnection() = 0;
};
```

### 4. TushareDataSource - Tushare 数据源

实现 `IDataSource` 接口，提供 Tushare Pro 数据访问。

**特性**:
- ✅ 自动数据解析
- ✅ 类型转换
- ✅ 错误处理
- ✅ 日志记录
- ✅ **从配置模块自动获取参数**

**示例**:
```cpp
#include "TushareDataSource.h"

// 方式1: 从配置模块创建（推荐）
network::TushareDataSource source;

// 方式2: 手动指定 Token
network::TushareDataSource source("your_token_here");

// 获取股票列表
auto stocks = source.getStockList();
for (const auto& stock : stocks) {
    std::cout << stock.ts_code << " " << stock.name << std::endl;
}

// 获取日线数据
auto data = source.getDailyData("000001.SZ", "20240101", "20240131");
for (const auto& d : data) {
    std::cout << d.trade_date << " " << d.close << std::endl;
}

// 获取最新行情
auto quote = source.getLatestQuote("000001.SZ");
std::cout << "最新价: " << quote.close << std::endl;
```

### 5. DataSourceFactory - 数据源工厂

使用工厂模式创建不同类型的数据源。

**特性**:
- ✅ 统一创建接口
- ✅ **从配置模块自动创建（推荐）**
- ✅ 类型安全
- ✅ 易于扩展

**示例**:
```cpp
#include "DataSourceFactory.h"

// 方式1: 从配置模块创建（推荐，自动获取所有参数）
auto source = network::DataSourceFactory::createFromConfig();

// 方式2: 手动指定 Token
auto source = network::DataSourceFactory::createDataSource(
    network::DataSourceFactory::DataSourceType::TUSHARE,
    "your_token_here"
);

// 方式3: 使用配置模块的参数
auto source = network::DataSourceFactory::createDataSource(
    network::DataSourceFactory::DataSourceType::TUSHARE,
    ""  // 空字符串表示从配置模块获取
);

// 使用数据源
auto stocks = source->getStockList();
```

## 📊 数据结构

### StockBasic - 股票基本信息

```cpp
struct StockBasic {
    std::string ts_code;        // 股票代码（如 000001.SZ）
    std::string symbol;         // 股票简称（如 000001）
    std::string name;           // 股票名称（如 平安银行）
    std::string area;           // 地域
    std::string industry;       // 行业
    std::string market;         // 市场类型
    std::string list_date;      // 上市日期
};
```

### StockData - 股票行情数据

```cpp
struct StockData {
    std::string ts_code;        // 股票代码
    std::string trade_date;     // 交易日期（YYYYMMDD）
    double open;                // 开盘价
    double high;                // 最高价
    double low;                 // 最低价
    double close;               // 收盘价
    double pre_close;           // 昨收价
    double change;              // 涨跌额
    double pct_chg;             // 涨跌幅（%）
    long volume;                // 成交量（手）
    double amount;              // 成交额（千元）
};
```

### HttpResponse - HTTP 响应

```cpp
struct HttpResponse {
    int status_code;                          // HTTP 状态码
    std::string body;                         // 响应体
    std::map<std::string, std::string> headers;  // 响应头
    std::string error_message;                // 错误信息
    
    bool isSuccess() const;  // 是否成功（200-299）
};
```

### TushareResponse - Tushare 响应

```cpp
struct TushareResponse {
    int code;                    // 响应码（0表示成功）
    std::string msg;             // 响应消息
    json data;                   // 响应数据
    
    bool isSuccess() const;  // 是否成功（code == 0）
};
```

## ⚙️ 配置说明

在 `.env` 文件中配置网络层参数：

```env
# ========== 数据源配置 ==========
# Tushare Pro API 配置
DATA_SOURCE_URL=http://api.waditu.com
DATA_SOURCE_API_KEY=your_tushare_token_here
DATA_SOURCE_TIMEOUT=30
DATA_SOURCE_RETRY_TIMES=3
DATA_SOURCE_RETRY_DELAY=1000
```

**配置项说明**:
- `DATA_SOURCE_URL`: Tushare API 地址
- `DATA_SOURCE_API_KEY`: Tushare API Token（必填）
- `DATA_SOURCE_TIMEOUT`: 请求超时时间（秒）
- `DATA_SOURCE_RETRY_TIMES`: 重试次数
- `DATA_SOURCE_RETRY_DELAY`: 重试延迟（毫秒）

## 🚀 快速开始

### 1. 获取 Tushare Token

访问 [Tushare Pro](https://tushare.pro/) 注册并获取 API Token。

### 2. 配置环境变量

```bash
# 复制配置模板
cp env.example .env

# 编辑 .env 文件，填入你的 Token
DATA_SOURCE_API_KEY=your_token_here
```

### 3. 使用示例

```cpp
#include "Network.h"
#include "Logger.h"
#include "Config.h"

int main() {
    // 初始化配置和日志
    auto& config = config::Config::getInstance();
    config.initialize(".env");
    logger::init(config);
    
    // 创建数据源（从配置）
    auto source = network::DataSourceFactory::createFromConfig();
    
    // 测试连接
    if (source->testConnection()) {
        LOG_INFO("连接成功");
    }
    
    // 获取股票列表
    auto stocks = source->getStockList();
    LOG_INFO("获取到 " + std::to_string(stocks.size()) + " 只股票");
    
    // 获取平安银行的日线数据
    auto data = source->getDailyData("000001.SZ", "20240101", "20240131");
    for (const auto& d : data) {
        LOG_INFO(d.trade_date + " 收盘价: " + std::to_string(d.close));
    }
    
    return 0;
}
```

## 📚 API 参考

### Tushare API 接口列表

#### 基础数据

| 方法 | 说明 | 参数 |
|------|------|------|
| `getStockBasic()` | 获取股票列表 | list_status, exchange |
| `getTradeCal()` | 获取交易日历 | start_date, end_date, exchange |

#### 行情数据

| 方法 | 说明 | 参数 |
|------|------|------|
| `getDailyQuote()` | 获取日线行情 | ts_code, trade_date, start_date, end_date |
| `getAdjFactor()` | 获取复权因子 | ts_code, trade_date, start_date, end_date |
| `getDailyBasic()` | 获取每日指标 | ts_code, trade_date, start_date, end_date |

#### 财务数据

| 方法 | 说明 | 参数 |
|------|------|------|
| `getIncome()` | 获取利润表 | ts_code, period, start_date, end_date |
| `getBalanceSheet()` | 获取资产负债表 | ts_code, period, start_date, end_date |
| `getCashFlow()` | 获取现金流量表 | ts_code, period, start_date, end_date |

#### 市场数据

| 方法 | 说明 | 参数 |
|------|------|------|
| `getHsConst()` | 获取沪深股通成份股 | ts_code, hs_type |

### 通用 API 调用

如果需要调用未封装的 API，可以使用通用接口：

```cpp
auto response = client.query(
    "api_name",                              // API 名称
    {{"param1", "value1"}, {"param2", "value2"}},  // 参数
    {"field1", "field2"}                     // 返回字段（可选）
);
```

## 🔧 扩展开发

### 添加新的数据源

1. 创建新的数据源类，继承 `IDataSource`：

```cpp
class MyDataSource : public IDataSource {
public:
    std::vector<StockBasic> getStockList() override {
        // 实现...
    }
    
    std::vector<StockData> getDailyData(
        const std::string& ts_code,
        const std::string& start_date,
        const std::string& end_date
    ) override {
        // 实现...
    }
    
    // 实现其他接口...
};
```

2. 在 `DataSourceFactory` 中添加新类型：

```cpp
enum class DataSourceType {
    TUSHARE,
    CSV,
    DATABASE,
    MY_SOURCE  // 新增
};

DataSourcePtr createDataSource(DataSourceType type, const std::string& config) {
    switch (type) {
        case DataSourceType::MY_SOURCE:
            return std::make_shared<MyDataSource>(config);
        // ...
    }
}
```

### 添加新的 Tushare API

在 `TushareClient` 中添加新方法：

```cpp
TushareResponse TushareClient::getNewApi(const std::string& param) {
    std::map<std::string, std::string> params;
    params["param"] = param;
    return query("new_api", params);
}
```

## 🐛 错误处理

### HTTP 错误

```cpp
auto response = client.get("/api/data");
if (!response.isSuccess()) {
    LOG_ERROR("HTTP 请求失败: " + response.error_message);
    LOG_ERROR("状态码: " + std::to_string(response.status_code));
}
```

### Tushare API 错误

```cpp
auto response = client.getDailyQuote("000001.SZ");
if (!response.isSuccess()) {
    LOG_ERROR("API 调用失败: " + response.msg);
    LOG_ERROR("错误码: " + std::to_string(response.code));
}
```

### 数据源错误

```cpp
try {
    auto source = network::DataSourceFactory::createFromConfig();
    auto data = source->getDailyData("000001.SZ", "20240101", "20240131");
} catch (const std::exception& e) {
    LOG_ERROR("数据获取失败: " + std::string(e.what()));
}
```

## 📈 性能优化

### 1. 连接复用

`HttpClient` 自动复用连接，无需手动管理。

### 2. 批量请求

```cpp
// 批量获取多只股票数据
std::vector<std::string> codes = {"000001.SZ", "000002.SZ", "600000.SH"};
for (const auto& code : codes) {
    auto data = source->getDailyData(code, "20240101", "20240131");
    // 处理数据...
}
```

### 3. 异步请求（待实现）

未来版本将支持异步请求以提高性能。

## 🔒 安全性

### 1. Token 保护

- ✅ Token 存储在 `.env` 文件中
- ✅ `.env` 文件已加入 `.gitignore`
- ✅ 不在日志中输出 Token

### 2. HTTPS 支持

- ✅ 自动识别 HTTPS URL
- ✅ SSL 证书验证

### 3. 请求限制

- ✅ 超时控制
- ✅ 重试限制
- ✅ 错误处理

## 📝 待实现功能

- ⏳ CSV 数据源
- ⏳ 数据库数据源
- ⏳ 异步请求支持
- ⏳ 请求缓存
- ⏳ 数据验证
- ⏳ 更多 Tushare API 封装

## 🤝 贡献指南

欢迎贡献代码！请遵循以下规范：

1. 遵循 SOLID 原则
2. 添加完整的注释
3. 编写单元测试
4. 更新文档

## 📄 许可证

本模块使用的第三方库：
- cpp-httplib (MIT License)
- nlohmann/json (MIT License)

## 📞 相关链接

- [Tushare Pro 官网](https://tushare.pro/)
- [Tushare Pro API 文档](https://tushare.pro/document/1?doc_id=130)
- [cpp-httplib GitHub](https://github.com/yhirose/cpp-httplib)
- [nlohmann/json GitHub](https://github.com/nlohmann/json)

---

**版本**: 1.0.0  
**最后更新**: 2026-02-01  
**维护者**: Development Team

