# Stock for C++ - 系统设计文档

## 📋 项目概述

**项目名称**: Stock for C++  
**版本**: 1.0.0  
**更新日期**: 2026-02-01  
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

## ⚠️ 重要技术决策

### 1. HTTP-Only 架构（不使用 SSL/HTTPS）

**决策**: 项目只使用 HTTP 协议，不支持 HTTPS，不依赖 OpenSSL

**原因**:
- ✅ **数据源需求**: Tushare Pro API 使用 HTTP 协议（`http://api.waditu.com`）
- ✅ **简化依赖**: 不需要安装和链接 OpenSSL 库
- ✅ **避免架构冲突**: 避免 ARM64/x86_64 架构不匹配问题
- ✅ **降低复杂度**: 减少编译配置和部署难度
- ✅ **满足需求**: 完全满足股票数据获取的业务需求

**实现方式**:
```cmake
# CMakeLists.txt 中禁用 SSL 支持
add_definitions(-DCPPHTTPLIB_OPENSSL_SUPPORT=0)

target_compile_definitions(network_lib PRIVATE
    CPPHTTPLIB_OPENSSL_SUPPORT=0
)
```

**影响范围**:
- `network/http/HttpClient`: 只创建 `httplib::Client`，不创建 `httplib::SSLClient`
- `CMakeLists.txt`: 添加编译选项禁用 SSL
- 所有网络请求都使用 HTTP 协议

**注意事项**:
- ⚠️ 如果将来需要访问 HTTPS API，需要重新启用 SSL 支持并链接 OpenSSL
- ⚠️ 当前架构不支持 HTTPS 请求，尝试访问 HTTPS URL 会失败

### 2. 配置驱动架构

**决策**: 所有配置通过 `.env` 文件管理，使用配置模块统一访问

**优势**:
- ✅ 集中管理：所有配置在一个文件中
- ✅ 环境隔离：开发/测试/生产环境配置分离
- ✅ 安全性：敏感信息（如 API Token）不进入代码库
- ✅ 灵活性：无需重新编译即可修改配置

### 3. 模块化命名空间

**决策**: 每个模块使用独立的命名空间

**命名空间列表**:
- `logger::` - 日志系统
- `config::` - 配置管理
- `network::` - 网络层
- `data::` - 数据层
- `analysis::` - 分析层
- `utils::` - 工具类
- `core::` - 核心业务（待开发）
- `output::` - 输出层（待开发）

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

| 库名 | 版本 | 用途 | 许可证 | 备注 |
|------|------|------|--------|------|
| **spdlog** | v1.x | 高性能日志库 | MIT | 已集成 |
| **dotenv-cpp** | master | 环境变量管理 | BSD | 已集成 |
| **nlohmann/json** | develop | JSON 处理 | MIT | 已集成 |
| **cpp-httplib** | master | HTTP 客户端 | MIT | ⚠️ 已集成，**仅 HTTP，禁用 SSL** |
| **sqlpp11** | main | SQL 查询构建器 | BSD | 待集成 |
| **ta-lib** | main | 技术分析库 | BSD | 待集成 |

### 重要说明

#### cpp-httplib 配置
- ⚠️ **仅支持 HTTP 协议**
- ⚠️ **已禁用 SSL/HTTPS 支持**
- ⚠️ **不依赖 OpenSSL**
- ✅ 通过 `CPPHTTPLIB_OPENSSL_SUPPORT=0` 编译选项禁用
- ✅ 完全满足 Tushare API（HTTP）的需求

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
├── data/                       # 数据层模块 (已完成 ✅)
│   ├── database/              # 数据库访问
│   │   ├── Connection.h/cpp   # 数据库连接（单例）
│   │   ├── StockDAO.h/cpp     # 股票数据访问对象
│   │   └── PriceDAO.h/cpp     # 价格数据访问对象
│   ├── cache/                 # 缓存管理
│   │   ├── ICache.h           # 缓存接口
│   │   └── MemoryCache.h/cpp  # 内存缓存（LRU）
│   ├── file/                  # 文件存储
│   │   ├── CSVReader.h/cpp    # CSV 文件读取
│   │   └── CSVWriter.h/cpp    # CSV 文件写入
│   └── Data.h                 # 统一头文件
│
├── network/                    # 网络层模块 (已完成 ✅)
│   ├── http/                  # HTTP 客户端
│   │   ├── HttpClient.h/cpp   # HTTP 客户端封装
│   │   ├── TushareClient.h/cpp # Tushare API 客户端
│   ├── IDataSource.h          # 数据源接口
│   ├── TushareDataSource.h/cpp # Tushare 数据源实现
│   ├── DataSourceFactory.h/cpp # 数据源工厂
│   ├── Network.h              # 统一头文件
│   ├── README.md              # 使用文档
│   ├── SUMMARY.md             # 开发总结
│   └── examples/              # 使用示例
│       ├── network_example.cpp # 示例代码
│       └── README.md          # 示例说明
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
├── analysis/                   # 分析层模块 (已完成 ✅)
│   ├── IIndicator.h           # 指标接口
│   ├── IndicatorBase.h/cpp    # 指标基类
│   ├── indicators/            # 技术指标
│   │   ├── MA.h/cpp          # 移动平均线
│   │   ├── EMA.h/cpp         # 指数移动平均线
│   │   ├── MACD.h/cpp        # MACD 指标
│   │   ├── RSI.h/cpp         # RSI 指标
│   │   ├── KDJ.h/cpp         # KDJ 指标
│   │   ├── BOLL.h/cpp        # 布林带
│   │   └── ATR.h/cpp         # 平均真实波幅
│   ├── IndicatorFactory.h/cpp # 指标工厂
│   ├── Analysis.h             # 统一头文件
│   ├── README.md              # 使用文档
│   └── SUMMARY.md             # 开发总结
│
├── core/                       # 核心业务模块 (已完成 ✅)
│   ├── Stock.h                # 股票数据结构
│   ├── Trade.h/cpp            # 交易记录
│   ├── Position.h/cpp         # 持仓管理
│   ├── Portfolio.h/cpp        # 投资组合
│   ├── Strategy.h/cpp         # 策略接口和基类
│   ├── StrategyFactory.h/cpp  # 策略工厂
│   ├── strategies/            # 具体策略实现
│   │   ├── MACrossStrategy.h/cpp  # 均线交叉策略
│   │   ├── MACDStrategy.h/cpp     # MACD策略
│   │   ├── RSIStrategy.h/cpp      # RSI策略
│   │   ├── BOLLStrategy.h/cpp     # 布林带策略
│   │   └── GridStrategy.h/cpp     # 网格交易策略
│   └── Core.h                 # 统一头文件
│
├── utils/                      # 工具类模块 (已完成 ✅)
│   ├── TimeUtil.h/cpp         # 时间工具
│   ├── StringUtil.h/cpp       # 字符串工具
│   ├── MathUtil.h/cpp         # 数学工具
│   ├── Utils.h                # 统一头文件
│   ├── README.md              # 使用文档
│   └── SUMMARY.md             # 开发总结
│
├── config/                     # 配置模块 (已完成 ✅)
│   ├── Config.h               # 配置管理类（头文件）
│   ├── Config.cpp             # 配置管理类（实现）
│   ├── README.md              # 使用文档
│   ├── EXAMPLES.md            # 示例代码
│   └── SUMMARY.md             # 开发总结
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

**已实现**: ✅ 日志系统、✅ 网络层

```cpp
// LoggerFactory - 创建不同类型的日志器
namespace logger {
    class LoggerFactory {
    public:
        static LoggerPtr createLogger(LoggerType type, const LogConfig& config);
    };
}

// DataSourceFactory - 创建不同类型的数据源
namespace network {
    class DataSourceFactory {
    public:
        static DataSourcePtr createDataSource(DataSourceType type, const std::string& config);
        static DataSourcePtr createFromConfig();  // 从配置模块创建
    };
}

// 使用示例
auto logger = logger::LoggerFactory::createLogger(
    logger::LoggerFactory::LoggerType::SPDLOG, 
    config
);

auto dataSource = network::DataSourceFactory::createFromConfig();
```

### 2. 单例模式 (Singleton Pattern)
**应用场景**: 日志管理器、配置管理器、数据库连接池

**已实现**: ✅ 日志管理器、✅ 配置管理器

```cpp
// LoggerManager - 全局唯一的日志管理器
namespace logger {
    class LoggerManager {
    public:
        static LoggerManager& getInstance();
    private:
        LoggerManager() = default;
    };
}

// Config - 全局唯一的配置管理器
namespace config {
    class Config {
    public:
        static Config& getInstance();
    private:
        Config() = default;
    };
}

// 使用示例
auto& logManager = logger::LoggerManager::getInstance();
auto& config = config::Config::getInstance();
```

### 3. 策略模式 (Strategy Pattern)
**应用场景**: 日志实现、交易策略、数据源

**已实现**: ✅ 日志系统、✅ 网络层

```cpp
// ILogger - 日志策略接口
namespace logger {
    class ILogger {
    public:
        virtual void info(const std::string& msg) = 0;
        virtual void error(const std::string& msg) = 0;
        // ...
    };

    // SpdlogLogger - 具体策略实现
    class SpdlogLogger : public ILogger {
    public:
        void info(const std::string& msg) override;
        void error(const std::string& msg) override;
        // ...
    };
}

// IDataSource - 数据源策略接口
namespace network {
    class IDataSource {
    public:
        virtual std::vector<StockData> getDailyData(...) = 0;
        virtual StockData getLatestQuote(...) = 0;
        // ...
    };

    // TushareDataSource - 具体策略实现
    class TushareDataSource : public IDataSource {
        // 实现接口...
    };
}
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

**已实现**: ✅ 网络层

```cpp
// IDataSource - 数据源接口
namespace network {
    class IDataSource {
    public:
        virtual std::vector<StockData> getDailyData(...) = 0;
        virtual StockData getLatestQuote(...) = 0;
    };
}

// TushareDataSource - 适配 Tushare API 到统一接口
namespace network {
    class TushareDataSource : public IDataSource {
    public:
        std::vector<StockData> getDailyData(...) override;
        StockData getLatestQuote(...) override;
    };
}
```

## 🔧 核心模块设计

### 1. 日志系统 (已完成 ✅)

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

#### 命名空间
```cpp
namespace logger {
    // 所有日志相关类和函数
}
```

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

#### 文档
- `log/README.md` - 项目概述
- `log/QUICKSTART.md` - 快速入门
- `log/USAGE.md` - 详细使用指南
- `log/ARCHITECTURE.md` - 架构设计
- `log/SUMMARY.md` - 项目总结

### 2. 配置系统 (已完成 ✅)

#### 设计原则
- ✅ 单例模式：全局唯一的配置管理器
- ✅ 单一职责：只负责配置的读取和管理
- ✅ 线程安全：使用互斥锁保护
- ✅ 配置驱动：所有配置通过 .env 管理

#### 核心类
- `Config`: 全局配置管理类（单例）

#### 配置分类（7 大类，39 个配置项）

| 分类 | 配置项数量 | 说明 |
|------|-----------|------|
| 应用配置 | 4 项 | 应用名称、版本、环境、调试模式 |
| 日志配置 | 9 项 | 日志级别、格式、文件、异步等 |
| 数据库配置 | 8 项 | 主机、端口、用户、密码、连接池等 |
| 数据源配置 | 5 项 | URL、API密钥、超时、重试等 |
| 缓存配置 | 4 项 | 启用、大小、TTL、策略 |
| 输出配置 | 4 项 | 输出目录、格式、图表、宽度 |
| 分析配置 | 5 项 | MA、MACD、RSI 等技术指标参数 |

#### 命名空间
```cpp
namespace config {
    class Config;  // 配置管理类
}
```

#### 使用示例
```cpp
#include "Config.h"

int main() {
    // 初始化配置
    auto& config = config::Config::getInstance();
    config.initialize(".env");
    
    // 读取配置
    std::cout << "应用: " << config.getAppName() << std::endl;
    std::cout << "数据库: " << config.getDbHost() << std::endl;
    std::cout << "缓存: " << config.isCacheEnabled() << std::endl;
    
    return 0;
}
```

#### 主要方法

**应用配置**
```cpp
getAppName()        // 应用名称
getAppVersion()     // 应用版本
getAppEnv()         // 运行环境
isDebugMode()       // 调试模式
```

**数据库配置**
```cpp
getDbHost()              // 数据库主机
getDbPort()              // 数据库端口
getDbName()              // 数据库名称
getDbUser()              // 数据库用户
getDbPassword()          // 数据库密码
getDbConnectionString()  // 连接字符串
```

**数据源配置**
```cpp
getDataSourceUrl()         // 数据源 URL
getDataSourceApiKey()      // API 密钥
getDataSourceTimeout()     // 请求超时
```

**缓存配置**
```cpp
isCacheEnabled()    // 是否启用缓存
getCacheSize()      // 缓存大小
getCacheTtl()       // 缓存过期时间
```

**输出配置**
```cpp
getOutputDir()      // 输出目录
getExportFormat()   // 导出格式
isChartEnabled()    // 是否启用图表
```

**分析配置**
```cpp
getDefaultMaPeriod()      // MA 周期
getDefaultMacdFast()      // MACD 快线周期
getDefaultRsiPeriod()     // RSI 周期
```

#### 特性
- ✅ **线程安全**: 使用互斥锁保护配置访问
- ✅ **默认值**: 所有配置都有合理的默认值
- ✅ **类型安全**: 提供类型化的 getter 方法
- ✅ **热重载**: 支持运行时重新加载配置
- ✅ **错误处理**: 配置解析失败时使用默认值并输出警告

#### 文档
- `config/README.md` - 使用文档
- `config/EXAMPLES.md` - 示例代码
- `config/SUMMARY.md` - 开发总结

### 3. 数据层 (已完成 ✅)

#### 设计原则
- ✅ 完全遵循 SOLID 原则
- ✅ DAO 模式 + 单例模式
- ✅ 从配置模块自动获取参数
- ✅ 支持 SQLite 数据库

#### 核心类
- `Connection`: 数据库连接管理（单例）
- `StockDAO`: 股票数据访问对象
- `PriceDAO`: 价格数据访问对象
- `ICache`: 缓存接口
- `MemoryCache`: 内存缓存实现
- `CSVReader`: CSV 文件读取
- `CSVWriter`: CSV 文件写入

#### 命名空间
```cpp
namespace data {
    namespace database {
        // 数据库相关类
    }
    namespace cache {
        // 缓存相关类
    }
    namespace file {
        // 文件操作相关类
    }
}
```

#### 数据库访问
```cpp
namespace data::database {

// 数据库连接类（单例）
class Connection {
public:
    static Connection& getInstance();
    bool connect(const std::string& dbPath);
    void disconnect();
    bool isConnected() const;
    sqlite3* getHandle();
    
    // 事务支持
    bool beginTransaction();
    bool commit();
    bool rollback();
};

// 股票数据访问对象
class StockDAO {
public:
    explicit StockDAO(Connection& conn);
    
    bool createTable();
    bool insert(const StockInfo& stock);
    std::optional<StockInfo> findBySymbol(const std::string& symbol);
    std::vector<StockInfo> findAll();
    bool update(const StockInfo& stock);
    bool remove(const std::string& symbol);
    size_t count();
};

// 价格数据访问对象
class PriceDAO {
public:
    explicit PriceDAO(Connection& conn);
    
    bool createTable();
    bool insert(const PriceData& price);
    std::vector<PriceData> findBySymbol(const std::string& symbol);
    std::vector<PriceData> findByDateRange(
        const std::string& symbol,
        const std::string& startDate,
        const std::string& endDate
    );
    bool update(const PriceData& price);
    bool remove(const std::string& symbol, const std::string& date);
    size_t count();
};

} // namespace data::database
```

#### 缓存系统
```cpp
namespace data::cache {

// 缓存接口
template<typename K, typename V>
class ICache {
public:
    virtual ~ICache() = default;
    virtual bool put(const K& key, const V& value) = 0;
    virtual std::optional<V> get(const K& key) = 0;
    virtual bool remove(const K& key) = 0;
    virtual void clear() = 0;
    virtual size_t size() const = 0;
};

// 内存缓存实现（LRU）
template<typename K, typename V>
class MemoryCache : public ICache<K, V> {
public:
    explicit MemoryCache(size_t maxSize = 1000, int ttlSeconds = 300);
    
    bool put(const K& key, const V& value) override;
    std::optional<V> get(const K& key) override;
    bool remove(const K& key) override;
    void clear() override;
    size_t size() const override;
    
    // 统计信息
    size_t getHitCount() const;
    size_t getMissCount() const;
    double getHitRate() const;
};

} // namespace data::cache
```

#### 文件操作
```cpp
namespace data::file {

// CSV 读取器
class CSVReader {
public:
    explicit CSVReader(const std::string& filepath);
    
    bool open();
    void close();
    bool isOpen() const;
    
    std::vector<std::string> readHeader();
    std::vector<std::vector<std::string>> readAll();
    std::optional<std::vector<std::string>> readLine();
    
    void setDelimiter(char delimiter);
    void setSkipEmptyLines(bool skip);
};

// CSV 写入器
class CSVWriter {
public:
    explicit CSVWriter(const std::string& filepath);
    
    bool open();
    void close();
    bool isOpen() const;
    
    bool writeHeader(const std::vector<std::string>& header);
    bool writeLine(const std::vector<std::string>& line);
    bool writeAll(const std::vector<std::vector<std::string>>& data);
    
    void setDelimiter(char delimiter);
};

} // namespace data::file
```

#### 使用示例
```cpp
#include "Data.h"

int main() {
    // 数据库操作
    auto& conn = data::database::Connection::getInstance();
    conn.connect("stock.db");
    
    data::database::StockDAO stockDao(conn);
    stockDao.createTable();
    
    // 插入股票
    StockInfo stock{"000001.SZ", "平安银行", "深交所", "银行"};
    stockDao.insert(stock);
    
    // 查询股票
    auto result = stockDao.findBySymbol("000001.SZ");
    if (result) {
        std::cout << "找到股票: " << result->name << std::endl;
    }
    
    // 缓存操作
    data::cache::MemoryCache<std::string, StockInfo> cache(1000, 300);
    cache.put("000001.SZ", stock);
    auto cached = cache.get("000001.SZ");
    
    // CSV 操作
    data::file::CSVWriter writer("stocks.csv");
    writer.open();
    writer.writeHeader({"symbol", "name", "market", "sector"});
    writer.writeLine({"000001.SZ", "平安银行", "深交所", "银行"});
    writer.close();
    
    return 0;
}
```

#### 特性
- ✅ **SQLite 支持**: 轻量级嵌入式数据库
- ✅ **事务支持**: 支持事务的 ACID 特性
- ✅ **连接池**: 单例模式管理数据库连接
- ✅ **LRU 缓存**: 基于 LRU 算法的内存缓存
- ✅ **TTL 支持**: 缓存项自动过期
- ✅ **CSV 支持**: 读写 CSV 文件
- ✅ **线程安全**: 使用互斥锁保护共享资源
- ✅ **错误处理**: 完善的错误处理和日志记录

### 4. 网络层 (已完成 ✅)

#### 设计原则
- ✅ 完全遵循 SOLID 原则
- ✅ 工厂模式 + 策略模式 + 适配器模式
- ✅ 从配置模块自动获取参数
- ✅ 封装 Tushare Pro API

#### 核心类
- `HttpClient`: HTTP 客户端封装
- `TushareClient`: Tushare API 客户端
- `IDataSource`: 数据源接口
- `TushareDataSource`: Tushare 数据源实现
- `DataSourceFactory`: 数据源工厂

#### 数据结构
- `HttpResponse`: HTTP 响应
- `TushareResponse`: Tushare API 响应
- `StockBasic`: 股票基本信息
- `StockData`: 股票行情数据

#### 命名空间
```cpp
namespace network {
    // 所有网络相关类和函数
}
```

#### 使用示例
```cpp
#include "Network.h"

int main() {
    // 创建数据源（从配置模块自动获取参数）
    auto source = network::DataSourceFactory::createFromConfig();
    
    // 获取股票列表
    auto stocks = source->getStockList();
    
    // 获取日线数据
    auto data = source->getDailyData("000001.SZ", "20240101", "20240131");
    
    // 获取最新行情
    auto quote = source->getLatestQuote("000001.SZ");
    
    return 0;
}
```

#### API 接口封装（11个）

**基础数据**:
- `getStockBasic()` - 获取股票列表
- `getTradeCal()` - 获取交易日历

**行情数据**:
- `getDailyQuote()` - 获取日线行情
- `getAdjFactor()` - 获取复权因子
- `getDailyBasic()` - 获取每日指标

**财务数据**:
- `getIncome()` - 获取利润表
- `getBalanceSheet()` - 获取资产负债表
- `getCashFlow()` - 获取现金流量表

**市场数据**:
- `getHsConst()` - 获取沪深股通成份股

**通用接口**:
- `query()` - 通用 API 调用

#### 特性
- ✅ **自动配置**: 从配置模块获取 URL 和 Token
- ✅ **只支持 HTTP**: 不依赖 OpenSSL，简化部署
- ✅ **重试机制**: 自动重试失败的请求
- ✅ **错误处理**: 完善的错误处理和日志记录
- ✅ **类型安全**: 强类型数据结构
- ✅ **易于扩展**: 支持添加新的数据源

#### 技术说明
- **HTTP 协议**: 只支持 HTTP，不支持 HTTPS
- **无需 OpenSSL**: 简化依赖，降低部署复杂度
- **适用场景**: Tushare Pro API 使用 HTTP 协议，完全满足需求

#### 文档
- `network/README.md` - 完整使用文档
- `network/SUMMARY.md` - 开发总结
- `network/examples/README.md` - 示例说明
- `network/examples/network_example.cpp` - 5个使用示例

### 5. 分析层设计 (已完成 ✅)

#### 设计原则
- ✅ 完全遵循 SOLID 原则
- ✅ 策略模式 + 工厂模式 + 模板方法模式
- ✅ 基于 TA-Lib 库实现
- ✅ 支持 7 种常用技术指标

#### 核心类
- `IIndicator`: 指标接口
- `IndicatorBase`: 指标基类
- `MA`: 移动平均线
- `EMA`: 指数移动平均线
- `MACD`: MACD 指标
- `RSI`: RSI 指标
- `KDJ`: KDJ 指标
- `BOLL`: 布林带
- `ATR`: 平均真实波幅
- `IndicatorFactory`: 指标工厂

#### 命名空间
```cpp
namespace analysis {
    // 所有技术分析相关类和函数
}
```

#### 技术指标
```cpp
namespace analysis {

// 指标接口
class IIndicator {
public:
    virtual std::shared_ptr<IndicatorResult> calculate(const std::vector<double>& prices) = 0;
    virtual std::string getName() const = 0;
    virtual void setParameters(const std::map<std::string, double>& params) = 0;
    virtual std::map<std::string, double> getParameters() const = 0;
};

// 移动平均线
class MA : public IndicatorBase {
public:
    explicit MA(int period = 20, TA_MAType maType = TA_MAType_SMA);
    std::shared_ptr<IndicatorResult> calculate(const std::vector<double>& prices) override;
    static std::vector<double> compute(const std::vector<double>& prices, int period, TA_MAType maType = TA_MAType_SMA);
};

// MACD 指标
class MACD : public IndicatorBase {
public:
    explicit MACD(int fastPeriod = 12, int slowPeriod = 26, int signalPeriod = 9);
    std::shared_ptr<IndicatorResult> calculate(const std::vector<double>& prices) override;
    static std::vector<double> compute(const std::vector<double>& prices, int fastPeriod = 12, int slowPeriod = 26, int signalPeriod = 9);
};

// RSI 指标
class RSI : public IndicatorBase {
public:
    explicit RSI(int period = 14);
    std::shared_ptr<IndicatorResult> calculate(const std::vector<double>& prices) override;
    static std::vector<double> compute(const std::vector<double>& prices, int period = 14);
};

// KDJ 指标
class KDJ : public IndicatorBase {
public:
    explicit KDJ(int fastK_Period = 9, int slowK_Period = 3, int slowD_Period = 3);
    std::shared_ptr<IndicatorResult> calculate(const std::vector<double>& high, const std::vector<double>& low, const std::vector<double>& close) override;
    static std::vector<double> compute(const std::vector<double>& high, const std::vector<double>& low, const std::vector<double>& close, int fastK_Period = 9, int slowK_Period = 3, int slowD_Period = 3);
};

// 布林带
class BOLL : public IndicatorBase {
public:
    explicit BOLL(int period = 20, double nbDevUp = 2.0, double nbDevDn = 2.0);
    std::shared_ptr<IndicatorResult> calculate(const std::vector<double>& prices) override;
    static std::vector<double> compute(const std::vector<double>& prices, int period = 20, double nbDevUp = 2.0, double nbDevDn = 2.0);
};

// 平均真实波幅
class ATR : public IndicatorBase {
public:
    explicit ATR(int period = 14);
    std::shared_ptr<IndicatorResult> calculate(const std::vector<double>& high, const std::vector<double>& low, const std::vector<double>& close) override;
    static std::vector<double> compute(const std::vector<double>& high, const std::vector<double>& low, const std::vector<double>& close, int period = 14);
};

// 指标工厂
class IndicatorFactory {
public:
    enum class IndicatorType { MA, EMA, MACD, RSI, KDJ, BOLL, ATR };
    
    static IndicatorPtr create(IndicatorType type, const std::map<std::string, double>& params = {});
    static IndicatorPtr create(const std::string& name, const std::map<std::string, double>& params = {});
    static std::vector<std::string> getSupportedIndicators();
};

} // namespace analysis
```

#### 使用示例
```cpp
#include "Analysis.h"

int main() {
    std::vector<double> prices = {10.0, 11.0, 12.0, 11.5, 13.0};
    
    // 方法1：静态方法（最简单）
    auto ma = analysis::MA::compute(prices, 5);
    auto rsi = analysis::RSI::compute(prices, 14);
    
    // 方法2：对象方式
    analysis::MA maIndicator(20);
    auto result = maIndicator.calculate(prices);
    
    // 方法3：工厂模式
    auto indicator = analysis::IndicatorFactory::create("MA", {{"period", 20}});
    auto factoryResult = indicator->calculate(prices);
    
    return 0;
}
```

#### 特性
- ✅ **7种技术指标**: MA、EMA、MACD、RSI、KDJ、BOLL、ATR
- ✅ **多种使用方式**: 静态方法、对象方式、工厂模式
- ✅ **类型安全**: C++ 强类型系统，智能指针管理
- ✅ **设计模式**: 策略模式、工厂模式、模板方法模式
- ✅ **易于扩展**: 统一接口，便于添加新指标
- ✅ **错误处理**: 完善的数据验证和异常处理

#### 文档
- `analysis/README.md` - 完整使用文档（400+ 行）
- `analysis/SUMMARY.md` - 开发总结

### 6. 工具类模块 (已完成 ✅)

#### 设计原则
- ✅ 完全遵循 SOLID 原则
- ✅ 工具类模式（静态方法）
- ✅ 功能分类清晰
- ✅ 易于使用和扩展

#### 核心类
- `TimeUtil`: 时间工具类（28个方法）
- `StringUtil`: 字符串工具类（37个方法）
- `MathUtil`: 数学工具类（34个方法）

#### 命名空间
```cpp
namespace utils {
    // 所有工具类相关函数
}
```

#### 功能概览

**TimeUtil - 时间工具类**：
- 格式化功能（now, today, timestampToString, format）
- 解析功能（stringToTimestamp, parseDate, parseDateTime）
- 计算功能（daysBetween, addDays, addMonths, addYears）
- 判断功能（isLeapYear, isValidDate, isWeekday, isWeekend）
- 信息获取（getDayOfWeek, getDayOfYear, getDaysInMonth等）
- 日期范围（getDateRange, getWeekStart/End, getMonthStart/End等）

**StringUtil - 字符串工具类**：
- 修剪功能（trim, ltrim, rtrim）
- 分割与连接（split, join）
- 大小写转换（toUpper, toLower, capitalize）
- 查找与替换（startsWith, endsWith, contains, replaceAll）
- 判断功能（isBlank, isNumeric, isInteger, isFloat等）
- 类型转换（toInt, toDouble, toString等）
- 格式化功能（padLeft, padRight, repeat, reverse, truncate）

**MathUtil - 数学工具类**：
- 基础统计（mean, median, variance, stddev, percentile等）
- 相关性分析（covariance, correlation）
- 金融计算（returnRate, sharpeRatio, maxDrawdown, volatility, cagr）
- 数值处理（round, ceil, floor, clamp, equals）
- 数组操作（normalize, standardize, cumsum, diff, pctChange）
- 移动计算（movingAverage, movingStddev, movingMax, movingMin）

#### 使用示例
```cpp
#include "Utils.h"

int main() {
    // 时间工具
    std::string today = utils::TimeUtil::today();
    int days = utils::TimeUtil::daysBetween("2024-01-01", "2024-12-31");
    
    // 字符串工具
    std::string trimmed = utils::StringUtil::trim("  hello  ");
    auto parts = utils::StringUtil::split("a,b,c", ",");
    
    // 数学工具
    std::vector<double> values = {1.0, 2.0, 3.0, 4.0, 5.0};
    double avg = utils::MathUtil::mean(values);
    double std = utils::MathUtil::stddev(values);
    
    return 0;
}
```

#### 特性
- ✅ **99个实用方法**: 覆盖时间、字符串、数学计算
- ✅ **静态方法**: 无需实例化，直接调用
- ✅ **类型安全**: C++ 强类型系统
- ✅ **错误处理**: 返回默认值或 NaN，不抛出异常
- ✅ **跨平台**: 支持 Windows/Linux/macOS
- ✅ **高性能**: 优化的算法实现

#### 文档
- `utils/README.md` - 完整使用文档（650+ 行）
- `utils/SUMMARY.md` - 开发总结（300+ 行）

### 7. 输出层设计 (待开发)

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

### 8. 核心业务设计 (已完成 ✅)

#### 核心数据结构
```cpp
namespace core {

// 交易记录
class Trade {
public:
    Trade(const std::string& tsCode, TradeType type, double price, int quantity, const std::string& tradeTime = "");
    
    // Getters
    int getId() const;
    std::string getTsCode() const;
    TradeType getType() const;
    double getPrice() const;
    int getQuantity() const;
    double getAmount() const;
    double getCommission() const;
    TradeStatus getStatus() const;
    
    // Setters
    void setCommission(double commission);
    void setStatus(TradeStatus status);
    
    // 计算盈亏
    double calculateProfit(const Trade& other) const;
    std::string toString() const;
};

// 建造者模式
class TradeBuilder {
public:
    TradeBuilder& tsCode(const std::string& tsCode);
    TradeBuilder& buy();
    TradeBuilder& sell();
    TradeBuilder& price(double price);
    TradeBuilder& quantity(int quantity);
    TradeBuilder& commission(double commission);
    TradePtr build() const;
};

// 持仓管理
class Position {
public:
    explicit Position(const std::string& tsCode);
    
    // Getters
    std::string getTsCode() const;
    int getQuantity() const;
    double getAvgCost() const;
    double getTotalCost() const;
    double getCurrentPrice() const;
    double getMarketValue() const;
    double getProfit() const;
    double getProfitRate() const;
    
    // 操作
    void updatePrice(double price);
    void addTrade(TradePtr trade);
    void buy(double price, int quantity, double commission = 0.0);
    void sell(double price, int quantity, double commission = 0.0);
    bool isEmpty() const;
};

// 投资组合
class Portfolio {
public:
    explicit Portfolio(const std::string& name = "Default", double initialCash = 0.0);
    
    // Getters
    std::string getName() const;
    double getCash() const;
    double getTotalValue() const;
    double getTotalProfit() const;
    double getTotalProfitRate() const;
    const std::map<std::string, PositionPtr>& getPositions() const;
    
    // 操作
    void addCash(double amount);
    bool buy(const std::string& tsCode, double price, int quantity, double commission = 0.0);
    bool sell(const std::string& tsCode, double price, int quantity, double commission = 0.0);
    void updatePrice(const std::string& tsCode, double price);
    void updatePrices(const std::map<std::string, double>& prices);
    void calculate();
    void clear();
    void reset();
    
    std::string toString() const;
    std::string getPositionsSummary() const;
};

} // namespace core
```

#### 策略系统
```cpp
namespace core {

// 策略接口
class IStrategy {
public:
    virtual ~IStrategy() = default;
    
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    
    virtual TradeSignal analyze(
        const std::string& tsCode,
        const std::vector<StockData>& data,
        const Portfolio& portfolio
    ) = 0;
    
    virtual void setParameters(const std::map<std::string, double>& params) = 0;
    virtual std::map<std::string, double> getParameters() const = 0;
    virtual bool validateParameters() const = 0;
};

// 策略基类
class StrategyBase : public IStrategy {
protected:
    std::string name_;
    std::string description_;
    std::map<std::string, double> parameters_;
    
    double getParameter(const std::string& key, double defaultValue = 0.0) const;
    void setParameter(const std::string& key, double value);
    bool hasEnoughData(const std::vector<StockData>& data, size_t minSize) const;
    TradeSignal createSignal(...) const;
};

// 策略工厂
class StrategyFactory {
public:
    enum class StrategyType {
        MA_CROSS,   // 均线交叉策略
        MACD,       // MACD 策略
        RSI,        // RSI 策略
        BOLL,       // 布林带策略
        GRID        // 网格交易策略
    };
    
    static StrategyPtr create(StrategyType type, const std::map<std::string, double>& params = {});
    static StrategyPtr create(const std::string& name, const std::map<std::string, double>& params = {});
    static void registerStrategy(const std::string& name, std::function<StrategyPtr(...)> creator);
    static std::vector<std::string> getSupportedStrategies();
};

} // namespace core
```

#### 内置策略

**1. 均线交叉策略 (MACrossStrategy)**
- 短期均线上穿长期均线：买入信号（金叉）
- 短期均线下穿长期均线：卖出信号（死叉）
- 参数：short_period（默认5）、long_period（默认20）

**2. MACD 策略 (MACDStrategy)**
- MACD 线上穿信号线：买入信号
- MACD 线下穿信号线：卖出信号
- 柱状图由负转正：买入确认
- 柱状图由正转负：卖出确认
- 参数：fast_period（默认12）、slow_period（默认26）、signal_period（默认9）

**3. RSI 策略 (RSIStrategy)**
- RSI < 30：超卖，买入信号
- RSI > 70：超买，卖出信号
- 参数：period（默认14）、oversold（默认30）、overbought（默认70）

**4. 布林带策略 (BOLLStrategy)**
- 价格触及下轨：买入信号
- 价格触及上轨：卖出信号
- 参数：period（默认20）、std_dev（默认2.0）

**5. 网格交易策略 (GridStrategy)**
- 价格下穿网格线：买入信号
- 价格上穿网格线：卖出信号
- 参数：grid_size（默认5.0%）、base_price（默认当前价格）、max_grids（默认10）

#### 使用示例
```cpp
#include "Core.h"

int main() {
    // 创建投资组合
    core::Portfolio portfolio("我的组合", 100000.0);
    
    // 买入股票
    portfolio.buy("000001.SZ", 10.5, 1000, 5.0);
    
    // 更新价格
    portfolio.updatePrice("000001.SZ", 11.2);
    
    // 查看持仓
    auto position = portfolio.getPosition("000001.SZ");
    std::cout << "盈亏: " << position->getProfit() << std::endl;
    
    // 使用策略
    auto strategy = core::StrategyFactory::create("MA_CROSS", {
        {"short_period", 5}, {"long_period", 20}
    });
    
    std::vector<core::StockData> data; // 历史数据
    auto signal = strategy->analyze("000001.SZ", data, portfolio);
    
    if (signal.signal == core::Signal::BUY) {
        portfolio.buy(signal.tsCode, signal.price, signal.quantity);
    }
    
    return 0;
}
```

## 📝 配置管理 (已完成 ✅)

配置模块提供全局配置管理功能，采用单例模式，线程安全，从 `.env` 文件读取所有系统配置。

### 配置模块特性

- ✅ **单例模式**: 全局唯一的配置管理器
- ✅ **线程安全**: 使用互斥锁保护配置访问
- ✅ **配置分类**: 7 大类 39 个配置项
- ✅ **默认值**: 所有配置都有合理的默认值
- ✅ **类型安全**: 提供类型化的 getter 方法
- ✅ **热重载**: 支持运行时重新加载配置

### 使用方式

```cpp
#include "Config.h"

int main() {
    // 初始化配置
    auto& config = config::Config::getInstance();
    config.initialize(".env");
    
    // 读取配置
    std::string appName = config.getAppName();
    std::string dbHost = config.getDbHost();
    int dbPort = config.getDbPort();
    bool cacheEnabled = config.isCacheEnabled();
    
    return 0;
}
```

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
# Tushare Pro API 配置
DATA_SOURCE_URL=http://api.waditu.com
DATA_SOURCE_API_KEY=your_tushare_token_here
DATA_SOURCE_TIMEOUT=30
DATA_SOURCE_RETRY_TIMES=3
DATA_SOURCE_RETRY_DELAY=1000

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

### Phase 1: 基础设施 (已完成 ✅)
- ✅ 项目初始化
- ✅ 第三方库集成
- ✅ 日志系统实现（logger 命名空间）
- ✅ 配置管理实现（config 命名空间）

### Phase 2: 网络层 (已完成 ✅)
- ✅ HTTP 客户端封装
- ✅ Tushare API 客户端
- ✅ 数据源接口设计
- ✅ Tushare 数据源实现
- ✅ 数据源工厂
- ✅ 完整文档和示例

### Phase 3: 数据层 (已完成 ✅)
- ✅ 数据库连接池
- ✅ DAO 层实现（StockDAO、PriceDAO）
- ✅ 缓存系统（MemoryCache）
- ✅ 文件存储（CSVReader、CSVWriter）

### Phase 4: 分析层 (已完成 ✅)
- ✅ 指标接口设计
- ✅ 指标基类实现
- ✅ 7种技术指标（MA、EMA、MACD、RSI、KDJ、BOLL、ATR）
- ✅ 指标工厂
- ✅ 完整文档和示例

### Phase 5: 工具类模块 (已完成 ✅)
- ✅ TimeUtil - 时间工具类（28个方法）
- ✅ StringUtil - 字符串工具类（37个方法）
- ✅ MathUtil - 数学工具类（34个方法）
- ✅ 统一头文件和文档

### Phase 6: 核心业务层 (已完成 ✅)
- ✅ 股票数据结构（Stock、StockEntity、StockData）
- ✅ 交易记录（Trade、TradeBuilder）
- ✅ 持仓管理（Position）
- ✅ 投资组合（Portfolio）
- ✅ 策略系统（IStrategy、StrategyBase、StrategyFactory）
- ✅ 5种内置策略（MA交叉、MACD、RSI、布林带、网格交易）

### Phase 7: 输出层 (待开发)
- ⏳ 控制台输出
- ⏳ CSV 导出
- ⏳ JSON 导出
- ⏳ 图表生成

### Phase 8: 测试与优化 (待开发)
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

### 已完成的文档

#### 日志系统文档 ✅
- `log/README.md` - 项目概述和配置说明
- `log/QUICKSTART.md` - 5 分钟快速入门
- `log/USAGE.md` - 详细使用指南和代码示例
- `log/ARCHITECTURE.md` - 架构设计和 SOLID 原则详解
- `log/SUMMARY.md` - 项目总结
- `log/CHECKLIST.md` - 完成检查清单
- `log/DELIVERY.md` - 项目交付报告

#### 配置系统文档 ✅
- `config/README.md` - 使用文档（包含所有 API 说明）
- `config/EXAMPLES.md` - 示例代码（5 个实用示例）
- `config/SUMMARY.md` - 开发总结

#### 网络层文档 ✅
- `network/README.md` - 完整使用文档（400+ 行）
- `network/SUMMARY.md` - 开发总结
- `network/examples/README.md` - 示例说明
- `network/examples/network_example.cpp` - 5个使用示例

### 文档标准

#### 代码文档
- 类和函数注释
- 参数说明
- 返回值说明
- 使用示例

#### 使用文档
- 快速开始
- API 参考
- 配置说明
- 示例代码

#### 用户文档
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

## 📈 开发进度

### 已完成模块 ✅

| 模块 | 状态 | 完成日期 | 文件数 | 代码行数 | 文档行数 |
|------|------|----------|--------|----------|----------|
| 日志系统 | ✅ 完成 | 2026-01-31 | 11 个 | ~800 行 | ~1000 行 |
| 配置系统 | ✅ 完成 | 2026-01-31 | 2 个 | ~350 行 | ~500 行 |
| 网络层 | ✅ 完成 | 2026-02-01 | 10 个 | ~1200 行 | ~1000 行 |
| 数据层 | ✅ 完成 | 2026-02-01 | 13 个 | ~1500 行 | 待完善 |
| 分析层 | ✅ 完成 | 2026-02-01 | 19 个 | ~1000 行 | ~500 行 |
| 工具类模块 | ✅ 完成 | 2026-02-01 | 9 个 | ~1950 行 | ~950 行 |
| 核心业务模块 | ✅ 完成 | 2026-02-05 | 17 个 | ~2000 行 | 待完善 |

### 待开发模块 ⏳

| 模块 | 优先级 | 预计工作量 |
|------|--------|-----------|
| 输出层 | 中 | 2-3 天 |
| 回测系统 | 中 | 3-4 天 |
| 测试 | 高 | 持续 |

### 总体进度

- **基础设施层**: ✅ 100% (日志系统 + 配置系统 + 工具类)
- **网络层**: ✅ 100% (HTTP 客户端 + Tushare API)
- **数据层**: ✅ 100% (数据库 + 缓存 + 文件操作)
- **分析层**: ✅ 100% (7种技术指标 + 工厂模式)
- **核心业务层**: ✅ 100% (交易 + 持仓 + 组合 + 5种策略)
- **输出层**: ⏳ 0%

**整体进度**: 约 83% (5/6 个主要模块)

---

**文档版本**: 1.7.0  
**最后更新**: 2026-02-05  
**维护者**: Development Team  
**变更记录**: 
- 2026-02-05: 添加核心业务模块完成标记，更新开发进度（83%），完善核心业务设计文档
- 2026-02-01: 添加工具类模块完成标记，更新开发进度（67%），完善工具类设计文档
- 2026-02-01: 添加分析层模块完成标记，更新开发进度（67%），完善分析层设计文档
- 2026-02-01: 添加数据层模块完成标记，更新开发进度（50%），完善数据层设计文档
- 2026-02-01: 添加"重要技术决策"章节，明确 HTTP-Only 架构和技术选型说明
- 2026-02-01: 优化 HTTP 客户端，移除 HTTPS 支持，简化依赖（不需要 OpenSSL）
- 2026-02-01: 添加网络层模块完成标记，更新设计模式应用，更新开发进度（30%）
- 2026-02-01: 添加配置系统完成标记，更新开发进度
- 2026-01-31: 初始版本，日志系统完成

