# Stock for C++ - 系统设计文档

## 📋 项目概述

**项目名称**: Stock for C++  
**版本**: 1.0.0  
**更新日期**: 2026-01-31  
**开发语言**: C++17  
**构建工具**: CMake 3.1+

这是一个基于 C++ 的股票分析系统，采用模块化设计，集成多个开源库，提供数据获取、存储、分析和可视化的完整解决方案。

## 🎯 系统目标

### 核心功能
- **数据获取**: 实时和历史股票数据获取
- **数据存储**: 高效的数据库存储和查询
- **技术分析**: 常用技术指标计算（MA、MACD、RSI、KDJ 等）
- **策略回测**: 交易策略的历史数据回测
- **数据处理**: JSON 数据解析和处理
- **日志系统**: 完善的日志记录和管理
- **数据可视化**: 图表展示和分析结果输出

### 设计原则
- ✅ **SOLID 原则**: 遵循面向对象设计原则
- ✅ **模块化**: 各模块独立，低耦合高内聚
- ✅ **可扩展**: 易于添加新功能和模块
- ✅ **高性能**: 优化的数据处理和存储
- ✅ **易维护**: 清晰的代码结构和文档

## 🏗️ 系统架构

### 整体架构图

```
┌─────────────────────────────────────────────────────────────┐
│                  Stock for C++ 分析系统                      │
├─────────────────────────────────────────────────────────────┤
│                                                               │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐      │
│  │  数据获取层   │  │  数据处理层   │  │  输出展示层   │      │
│  │              │  │              │  │              │      │
│  │ - HTTP 客户端│  │ - JSON 解析  │  │ - 控制台输出  │      │
│  │ - 数据源接口 │  │ - 数据验证   │  │ - 文件导出    │      │
│  │ - CSV 导入   │  │ - 格式转换   │  │ - 图表生成    │      │
│  └──────────────┘  └──────────────┘  └──────────────┘      │
│         │                  │                  │              │
│         └──────────────────┼──────────────────┘              │
│                            │                                 │
│  ┌─────────────────────────┴──────────────────────────┐     │
│  │              业务逻辑层 (核心层)                     │     │
│  │                                                      │     │
│  │  ┌──────────────┐  ┌──────────────┐  ┌──────────┐ │     │
│  │  │  股票管理    │  │  技术分析    │  │  策略引擎 │ │     │
│  │  │              │  │              │  │          │ │     │
│  │  │ - 股票信息   │  │ - MA/EMA     │  │ - 买卖信号│ │     │
│  │  │ - 价格管理   │  │ - MACD       │  │ - 风险控制│ │     │
│  │  │ - K线数据    │  │ - RSI/KDJ    │  │ - 策略回测│ │     │
│  │  │ - 历史数据   │  │ - BOLL       │  │ - 收益计算│ │     │
│  │  └──────────────┘  └──────────────┘  └──────────┘ │     │
│  └──────────────────────────────────────────────────┘      │
│                            │                                 │
│  ┌─────────────────────────┴──────────────────────────┐     │
│  │                   数据持久层                         │     │
│  │                                                      │     │
│  │  ┌──────────────┐  ┌──────────────┐  ┌──────────┐ │     │
│  │  │  数据库层    │  │  缓存层      │  │  文件存储 │ │     │
│  │  │              │  │              │  │          │ │     │
│  │  │ - SQL 查询   │  │ - 内存缓存   │  │ - CSV    │ │     │
│  │  │ - 历史数据   │  │ - 热数据     │  │ - JSON   │ │     │
│  │  │ - 连接池     │  │ - 快速访问   │  │ - 日志文件│ │     │
│  │  └──────────────┘  └──────────────┘  └──────────┘ │     │
│  └──────────────────────────────────────────────────┘      │
│                            │                                 │
│  ┌─────────────────────────┴──────────────────────────┐     │
│  │                   基础设施层                         │     │
│  │                                                      │     │
│  │  ┌──────────────┐  ┌──────────────┐  ┌──────────┐ │     │
│  │  │  日志系统    │  │  配置管理    │  │  工具类   │ │     │
│  │  │              │  │              │  │          │ │     │
│  │  │ - Logger     │  │ - .env 配置  │  │ - 时间工具│ │     │
│  │  │ - 多级别     │  │ - 参数管理   │  │ - 字符串  │ │     │
│  │  │ - 文件轮转   │  │ - 环境变量   │  │ - 数学计算│ │     │
│  │  └──────────────┘  └──────────────┘  └──────────┘ │     │
│  └──────────────────────────────────────────────────┘      │
│                                                               │
└─────────────────────────────────────────────────────────────┘
```

### 分层说明

#### 1. 数据获取层
- **职责**: 从外部数据源获取股票数据
- **组件**: HTTP 客户端、数据源适配器
- **技术**: cpp-httplib

#### 2. 数据处理层
- **职责**: 数据解析、验证和转换
- **组件**: JSON 解析器、数据验证器、CSV 解析器
- **技术**: nlohmann/json

#### 3. 输出展示层
- **职责**: 分析结果的展示和导出
- **组件**: 控制台输出、文件导出、图表生成
- **技术**: 标准输出、文件 I/O

#### 4. 业务逻辑层
- **职责**: 核心业务逻辑处理
- **组件**: 股票管理、技术分析、策略引擎
- **技术**: ta-lib

#### 5. 数据持久层
- **职责**: 数据存储和查询
- **组件**: 数据库访问、缓存管理
- **技术**: sqlpp11

#### 6. 基础设施层
- **职责**: 提供基础服务
- **组件**: 日志系统、配置管理、工具类
- **技术**: spdlog, dotenv-cpp

## 📦 技术栈

### 核心依赖

| 库名 | 版本 | 用途 | 许可证 |
|------|------|------|--------|
| **spdlog** | v1.x | 高性能日志库 | MIT |
| **dotenv-cpp** | master | 环境变量管理 | BSD |
| **nlohmann/json** | develop | JSON 处理 | MIT |
| **cpp-httplib** | master | HTTP 客户端/服务器 | MIT |
| **sqlpp11** | main | SQL 查询构建器 | BSD |
| **ta-lib** | main | 技术分析库 | BSD |

### 开发工具
- **编译器**: GCC 7+, Clang 5+, MSVC 2017+
- **构建系统**: CMake 3.1+
- **版本控制**: Git
- **包管理**: Git Submodules

## 🗂️ 项目结构

```
stock_for_cpp/
├── log/                        # 日志系统模块 (已完成)
│   ├── LogLevel.h             # 日志级别定义
│   ├── LogConfig.h/cpp        # 配置管理
│   ├── ILogger.h              # 日志接口
│   ├── SpdlogLogger.h/cpp     # spdlog 实现
│   ├── LoggerFactory.h/cpp    # 日志工厂
│   ├── LoggerManager.h/cpp    # 日志管理器
│   ├── Logger.h               # 统一头文件
│   └── docs/                  # 日志系统文档
│
├── data/                       # 数据层模块 (待开发)
│   ├── database/              # 数据库访问
│   │   ├── Connection.h/cpp   # 数据库连接
│   │   ├── StockDAO.h/cpp     # 股票数据访问对象
│   │   └── TransactionDAO.h/cpp # 交易数据访问对象
│   ├── cache/                 # 缓存管理
│   │   └── MemoryCache.h/cpp  # 内存缓存
│   └── file/                  # 文件存储
│       └── CSVReader.h/cpp    # CSV 文件读取
│
├── network/                    # 网络层模块 (待开发)
│   ├── http/                  # HTTP 客户端
│   │   ├── HttpClient.h/cpp   # HTTP 客户端封装
│   │   └── DataSource.h/cpp   # 数据源接口
│   └── parser/                # 数据解析
│       ├── JsonParser.h/cpp   # JSON 解析器
│       └── CsvParser.h/cpp    # CSV 解析器
│
├── output/                     # 输出层模块 (待开发)
│   ├── console/               # 控制台输出
│   │   └── ConsoleOutput.h/cpp # 控制台格式化输出
│   ├── file/                  # 文件导出
│   │   ├── CsvExporter.h/cpp  # CSV 导出
│   │   └── JsonExporter.h/cpp # JSON 导出
│   └── chart/                 # 图表生成
│       └── ChartGenerator.h/cpp # 图表生成器
│
├── analysis/                   # 分析层模块 (待开发)
│   ├── indicators/            # 技术指标
│   │   ├── MA.h/cpp          # 移动平均线
│   │   ├── MACD.h/cpp        # MACD 指标
│   │   └── RSI.h/cpp         # RSI 指标
│   └── strategy/              # 策略引擎
│       ├── Strategy.h         # 策略接口
│       └── SimpleStrategy.cpp # 简单策略实现
│
├── core/                       # 核心业务模块 (待开发)
│   ├── Stock.h/cpp            # 股票实体
│   ├── Trade.h/cpp            # 交易实体
│   ├── Portfolio.h/cpp        # 投资组合
│   └── StockManager.h/cpp     # 股票管理器
│
├── utils/                      # 工具类模块 (待开发)
│   ├── TimeUtil.h/cpp         # 时间工具
│   ├── StringUtil.h/cpp       # 字符串工具
│   └── MathUtil.h/cpp         # 数学工具
│
├── config/                     # 配置模块 (待开发)
│   └── Config.h/cpp           # 配置管理
│
├── thirdparty/                 # 第三方库
│   ├── spdlog/                # 日志库
│   ├── dotenv-cpp/            # 环境变量
│   ├── json/                  # JSON 库
│   ├── cpp-httplib/           # HTTP 库
│   ├── sqlpp11/               # SQL 库
│   └── ta-lib/                # 技术分析库
│
├── tests/                      # 测试目录 (待开发)
│   ├── unit/                  # 单元测试
│   └── integration/           # 集成测试
│
├── docs/                       # 文档目录
│   └── api/                   # API 文档
│
├── CMakeLists.txt             # CMake 配置
├── main.cpp                   # 主程序入口
├── .env                       # 环境变量配置
├── env.example                # 配置示例
├── README.md                  # 项目说明
└── DESIGN.md                  # 本设计文档
```

## 🎨 设计模式应用

### 1. 工厂模式 (Factory Pattern)
**应用场景**: 日志系统、数据源创建

```cpp
// LoggerFactory - 创建不同类型的日志器
class LoggerFactory {
public:
    static LoggerPtr createLogger(LoggerType type, const LogConfig& config);
};

// 使用示例
auto logger = LoggerFactory::createLogger(LoggerType::SPDLOG, config);
```

### 2. 单例模式 (Singleton Pattern)
**应用场景**: 日志管理器、配置管理器、数据库连接池

```cpp
// LoggerManager - 全局唯一的日志管理器
class LoggerManager {
public:
    static LoggerManager& getInstance();
private:
    LoggerManager() = default;
};

// 使用示例
auto& manager = LoggerManager::getInstance();
```

### 3. 策略模式 (Strategy Pattern)
**应用场景**: 日志实现、交易策略、数据源

```cpp
// ILogger - 日志策略接口
class ILogger {
public:
    virtual void info(const std::string& msg) = 0;
};

// SpdlogLogger - 具体策略实现
class SpdlogLogger : public ILogger {
public:
    void info(const std::string& msg) override;
};
```

### 4. 观察者模式 (Observer Pattern)
**应用场景**: 价格变动通知、事件监听

```cpp
// 价格变动观察者
class PriceObserver {
public:
    virtual void onPriceChange(const Stock& stock, double newPrice) = 0;
};
```

### 5. 建造者模式 (Builder Pattern)
**应用场景**: 复杂对象构建（如查询构建器）

```cpp
// SQL 查询构建器
auto query = QueryBuilder()
    .select("*")
    .from("stocks")
    .where("price > 100")
    .orderBy("price DESC")
    .build();
```

### 6. 适配器模式 (Adapter Pattern)
**应用场景**: 不同数据源的统一接口

```cpp
// 数据源适配器
class DataSourceAdapter {
public:
    virtual StockData fetchData(const std::string& symbol) = 0;
};
```

## 🔧 核心模块设计

### 1. 日志系统 (已完成)

#### 设计原则
- ✅ 完全遵循 SOLID 原则
- ✅ 工厂模式 + 单例模式 + 策略模式
- ✅ 配置驱动，通过 .env 管理

#### 核心类
- `LogLevel`: 日志级别枚举
- `LogConfig`: 配置管理类
- `ILogger`: 日志接口
- `SpdlogLogger`: spdlog 实现
- `LoggerFactory`: 日志工厂
- `LoggerManager`: 日志管理器（单例）

#### 使用示例
```cpp
#include "Logger.h"

int main() {
    logger::init(".env");
    LOG_INFO("应用程序启动");
    LOG_ERROR("错误信息");
    return 0;
}
```

### 2. 数据层设计 (待开发)

#### 数据库访问
```cpp
namespace data {

// 数据库连接类
class Connection {
public:
    static Connection& getInstance();
    bool connect(const std::string& connStr);
    void disconnect();
    bool isConnected() const;
};

// 股票数据访问对象
class StockDAO {
public:
    bool insert(const Stock& stock);
    Stock findBySymbol(const std::string& symbol);
    std::vector<Stock> findAll();
    bool update(const Stock& stock);
    bool remove(const std::string& symbol);
};

} // namespace data
```

### 3. 网络层设计 (待开发)

#### HTTP 客户端
```cpp
namespace network {

// HTTP 客户端封装
class HttpClient {
public:
    HttpClient(const std::string& baseUrl);
    std::string get(const std::string& path);
    bool download(const std::string& url, const std::string& filepath);
};

// 数据源接口
class IDataSource {
public:
    virtual StockData fetchRealtime(const std::string& symbol) = 0;
    virtual std::vector<StockData> fetchHistory(
        const std::string& symbol,
        const std::string& startDate,
        const std::string& endDate
    ) = 0;
};

// CSV 解析器
class CsvParser {
public:
    std::vector<StockData> parse(const std::string& filepath);
    std::vector<StockData> parseString(const std::string& csvContent);
};

} // namespace network
```

### 4. 输出层设计 (待开发)

#### 控制台输出
```cpp
namespace output {

// 控制台输出类
class ConsoleOutput {
public:
    // 输出股票信息
    void printStock(const Stock& stock);
    
    // 输出技术指标
    void printIndicators(const std::map<std::string, double>& indicators);
    
    // 输出回测结果
    void printBacktestResult(const BacktestResult& result);
    
    // 输出表格
    void printTable(const std::vector<std::vector<std::string>>& data);
};

// CSV 导出类
class CsvExporter {
public:
    bool exportStocks(const std::vector<Stock>& stocks, const std::string& filepath);
    bool exportPrices(const std::vector<StockData>& prices, const std::string& filepath);
    bool exportTrades(const std::vector<Trade>& trades, const std::string& filepath);
};

} // namespace output
```

### 5. 分析层设计 (待开发)

#### 技术指标
```cpp
namespace analysis {

// 移动平均线
class MA {
public:
    static std::vector<double> calculate(
        const std::vector<double>& prices,
        int period
    );
};

// MACD 指标
class MACD {
public:
    struct Result {
        std::vector<double> macd;
        std::vector<double> signal;
        std::vector<double> histogram;
    };
    
    static Result calculate(
        const std::vector<double>& prices,
        int fastPeriod = 12,
        int slowPeriod = 26,
        int signalPeriod = 9
    );
};

} // namespace analysis
```

### 6. 核心业务设计 (待开发)

#### 股票实体
```cpp
namespace core {

// 股票实体类
class Stock {
public:
    Stock(const std::string& symbol, const std::string& name);
    
    // Getters
    std::string getSymbol() const;
    std::string getName() const;
    double getPrice() const;
    double getChange() const;
    
    // Setters
    void setPrice(double price);
    void setVolume(long volume);
    
private:
    std::string symbol_;
    std::string name_;
    double price_;
    double change_;
    long volume_;
    std::time_t updateTime_;
};

// 股票管理器
class StockManager {
public:
    static StockManager& getInstance();
    
    bool addStock(const Stock& stock);
    Stock getStock(const std::string& symbol);
    std::vector<Stock> getAllStocks();
    bool updatePrice(const std::string& symbol, double price);
    
private:
    std::unordered_map<std::string, Stock> stocks_;
};

} // namespace core
```

## 📝 配置管理

### 环境变量配置 (.env)

```env
# ========== 日志配置 ==========
LOG_LEVEL=DEBUG
LOG_PATTERN=[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%t] %v
LOG_FILE_PATH=logs/app.log
LOG_CONSOLE_ENABLED=true
LOG_FILE_ENABLED=true
LOG_MAX_FILE_SIZE=10485760
LOG_MAX_FILES=3
LOG_ASYNC_ENABLED=false
LOG_ASYNC_QUEUE_SIZE=8192

# ========== 数据库配置 ==========
DB_HOST=localhost
DB_PORT=3306
DB_NAME=stock_db
DB_USER=root
DB_PASSWORD=password
DB_POOL_SIZE=10

# ========== 数据源配置 ==========
DATA_SOURCE_URL=https://api.example.com
DATA_SOURCE_API_KEY=your_api_key
DATA_SOURCE_TIMEOUT=10

# ========== 缓存配置 ==========
CACHE_ENABLED=true
CACHE_SIZE=1000
CACHE_TTL=300

# ========== 输出配置 ==========
OUTPUT_DIR=output
EXPORT_FORMAT=csv
CHART_ENABLED=false
```

## 🚀 开发路线图

### Phase 1: 基础设施 (已完成)
- ✅ 项目初始化
- ✅ 第三方库集成
- ✅ 日志系统实现
- ✅ 配置管理实现

### Phase 2: 数据层 (进行中)
- ⏳ 数据库连接池
- ⏳ DAO 层实现
- ⏳ 缓存系统
- ⏳ 文件存储

### Phase 3: 网络层 (待开发)
- ⏳ HTTP 客户端封装
- ⏳ 数据源适配器
- ⏳ CSV 解析器
- ⏳ JSON 解析器

### Phase 4: 业务层 (待开发)
- ⏳ 股票实体设计
- ⏳ 股票管理器
- ⏳ 交易记录
- ⏳ 投资组合

### Phase 5: 分析层 (待开发)
- ⏳ 技术指标实现
- ⏳ 策略引擎
- ⏳ 回测系统
- ⏳ 信号生成

### Phase 6: 输出层 (待开发)
- ⏳ 控制台输出
- ⏳ CSV 导出
- ⏳ JSON 导出
- ⏳ 图表生成

### Phase 7: 测试与优化 (待开发)
- ⏳ 单元测试
- ⏳ 集成测试
- ⏳ 性能优化
- ⏳ 文档完善

## 📊 数据库设计

### 股票表 (stocks)
```sql
CREATE TABLE stocks (
    id INT PRIMARY KEY AUTO_INCREMENT,
    symbol VARCHAR(20) NOT NULL UNIQUE,
    name VARCHAR(100) NOT NULL,
    market VARCHAR(20),
    sector VARCHAR(50),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
);
```

### 价格表 (prices)
```sql
CREATE TABLE prices (
    id BIGINT PRIMARY KEY AUTO_INCREMENT,
    stock_id INT NOT NULL,
    date DATE NOT NULL,
    open DECIMAL(10, 2),
    high DECIMAL(10, 2),
    low DECIMAL(10, 2),
    close DECIMAL(10, 2),
    volume BIGINT,
    FOREIGN KEY (stock_id) REFERENCES stocks(id),
    UNIQUE KEY (stock_id, date)
);
```

### 交易表 (trades)
```sql
CREATE TABLE trades (
    id BIGINT PRIMARY KEY AUTO_INCREMENT,
    stock_id INT NOT NULL,
    type ENUM('BUY', 'SELL') NOT NULL,
    price DECIMAL(10, 2) NOT NULL,
    quantity INT NOT NULL,
    amount DECIMAL(15, 2) NOT NULL,
    trade_time TIMESTAMP NOT NULL,
    FOREIGN KEY (stock_id) REFERENCES stocks(id)
);
```

## 🔒 安全设计

### 1. 数据安全
- 敏感信息加密存储
- 数据库连接使用 SSL
- API 密钥安全管理

### 2. 数据访问
- 数据源认证
- 请求频率限制
- 数据验证

### 3. 日志安全
- 不记录敏感信息
- 日志文件权限控制
- 定期清理旧日志

## 📈 性能优化

### 1. 数据库优化
- 索引优化
- 查询优化
- 连接池管理

### 2. 缓存策略
- 热数据缓存
- 查询结果缓存
- 过期策略

### 3. 并发处理
- 线程池
- 异步 I/O
- 无锁数据结构

## 🧪 测试策略

### 单元测试
- 每个模块独立测试
- 覆盖率 > 80%
- 使用 Google Test

### 集成测试
- 模块间交互测试
- 数据库集成测试
- API 接口测试

### 性能测试
- 压力测试
- 并发测试
- 响应时间测试

## 📚 文档规范

### 代码文档
- 类和函数注释
- 参数说明
- 返回值说明
- 使用示例

### 使用文档
- 命令行参数
- 配置说明
- 输出格式
- 示例代码

### 用户文档
- 快速开始
- 使用指南
- 常见问题
- 最佳实践

## 🤝 贡献指南

### 代码规范
- 遵循 C++17 标准
- 使用统一的命名风格
- 添加必要的注释
- 编写单元测试

### 提交规范
- 清晰的提交信息
- 一次提交一个功能
- 代码审查通过
- 测试通过

## 📄 许可证

本项目基于以下开源库：
- spdlog (MIT License)
- dotenv-cpp (BSD License)
- nlohmann/json (MIT License)
- cpp-httplib (MIT License)
- sqlpp11 (BSD License)
- ta-lib (BSD License)

---

**文档版本**: 1.0.0  
**最后更新**: 2026-01-31  
**维护者**: Development Team

