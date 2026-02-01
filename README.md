# Stock for C++ - 股票分析系统

## 项目简介

这是一个基于 C++ 的股票分析系统，集成了多个开源库，提供完整的数据处理、分析和日志功能。

## 📦 集成的开源库

- **spdlog** - 高性能日志库
- **dotenv-cpp** - 环境变量管理
- **nlohmann/json** - JSON 处理
- **cpp-httplib** - HTTP 客户端（⚠️ 仅 HTTP，不支持 HTTPS）
- **sqlpp11** - SQL 查询构建器
- **ta-lib** - 技术分析库

### ⚠️ 重要说明

**本项目只使用 HTTP 协议，不支持 HTTPS**
- 已禁用 SSL/OpenSSL 支持
- Tushare Pro API 使用 HTTP 协议，完全满足需求
- 简化依赖，避免架构冲突问题

## 🎯 已完成模块

### 1. 日志系统 (logger::)

本项目已完成对 spdlog 的封装，提供了一个遵循 SOLID 原则和设计模式的日志系统。

#### 特性

- ✅ **SOLID 原则**：完全遵循单一职责、开闭、里氏替换、接口隔离、依赖倒置原则
- ✅ **设计模式**：工厂模式、单例模式、策略模式
- ✅ **配置驱动**：所有配置通过 `.env` 文件管理
- ✅ **高性能**：支持异步日志、文件轮转
- ✅ **线程安全**：支持多线程环境
- ✅ **易于使用**：简单的宏定义和统一接口

### 2. 配置系统 (config::)

全局配置管理，单例模式，线程安全。

#### 特性

- ✅ **单例模式**：全局唯一的配置管理器
- ✅ **线程安全**：使用互斥锁保护
- ✅ **配置分类**：7 大类 39 个配置项
- ✅ **类型安全**：提供类型化的 getter 方法

### 3. 网络层 (network::)

HTTP 客户端和 Tushare Pro API 封装。

#### 特性

- ✅ **HTTP-Only**：只支持 HTTP，不依赖 OpenSSL
- ✅ **Tushare API**：封装 11 个常用接口
- ✅ **数据源抽象**：支持多种数据源
- ✅ **工厂模式**：统一创建接口
- ✅ **自动配置**：从配置模块获取参数

### 4. 数据层 (data::)

数据库访问、缓存管理和文件操作。

#### 特性

- ✅ **SQLite 支持**：轻量级嵌入式数据库
- ✅ **DAO 模式**：StockDAO、PriceDAO
- ✅ **事务支持**：支持事务的 ACID 特性
- ✅ **LRU 缓存**：基于 LRU 算法的内存缓存
- ✅ **TTL 支持**：缓存项自动过期
- ✅ **CSV 支持**：读写 CSV 文件
- ✅ **线程安全**：使用互斥锁保护共享资源

### 快速开始

#### 1. 创建配置文件

```bash
cp env.example .env
# 编辑 .env，设置 Tushare API Token
# DATA_SOURCE_API_KEY=your_token_here
```

#### 2. 在代码中使用

```cpp
#include "Logger.h"
#include "Config.h"
#include "Network.h"
#include "Data.h"

int main() {
    // 初始化配置
    auto& config = config::Config::getInstance();
    config.initialize(".env");
    
    // 初始化日志系统
    logger::init(config);
    
    // 使用网络层获取股票数据
    auto source = network::DataSourceFactory::createFromConfig();
    auto stocks = source->getStockList();
    
    LOG_INFO("获取到 " + std::to_string(stocks.size()) + " 只股票");
    
    // 使用数据层存储数据
    auto& conn = data::database::Connection::getInstance();
    conn.connect("stock.db");
    
    data::database::StockDAO stockDao(conn);
    stockDao.createTable();
    
    // 保存股票信息到数据库
    for (const auto& stock : stocks) {
        stockDao.insert(stock);
    }
    
    LOG_INFO("股票数据已保存到数据库");
    
    return 0;
}
```

#### 3. 编译运行

```bash
mkdir -p build
cd build
cmake ..
make
./stock_for_cpp
```

### 文档索引

#### 日志系统文档
- **[QUICKSTART.md](log/QUICKSTART.md)** - 5 分钟快速入门
- **[README.md](log/README.md)** - 项目概述和配置说明
- **[USAGE.md](log/USAGE.md)** - 详细使用指南
- **[ARCHITECTURE.md](log/ARCHITECTURE.md)** - 架构设计文档
- **[SUMMARY.md](log/SUMMARY.md)** - 项目总结

#### 配置系统文档
- **[README.md](config/README.md)** - 使用文档
- **[EXAMPLES.md](config/EXAMPLES.md)** - 示例代码
- **[SUMMARY.md](config/SUMMARY.md)** - 开发总结

#### 网络层文档
- **[README.md](network/README.md)** - 完整使用文档
- **[SUMMARY.md](network/SUMMARY.md)** - 开发总结
- **[examples/](network/examples/)** - 使用示例

#### 数据层文档
- **[Data.h](data/Data.h)** - 统一头文件和使用说明

#### 项目文档
- **[DESIGN.md](DESIGN.md)** - 系统设计文档
- **[QUICK_REFERENCE.md](QUICK_REFERENCE.md)** - 快速参考

## 📁 项目结构

```
stock_for_cpp/
├── log/                    # 日志系统封装 (已完成 ✅)
│   ├── LogLevel.h         # 日志级别定义
│   ├── LogConfig.h/cpp    # 配置管理
│   ├── ILogger.h          # 日志接口
│   ├── SpdlogLogger.h/cpp # spdlog 实现
│   ├── LoggerFactory.h/cpp # 日志工厂
│   ├── LoggerManager.h/cpp # 日志管理器
│   ├── Logger.h           # 统一头文件
│   └── *.md               # 文档
├── config/                # 配置系统 (已完成 ✅)
│   ├── Config.h/cpp       # 配置管理类
│   └── *.md               # 文档
├── network/               # 网络层 (已完成 ✅)
│   ├── http/              # HTTP 客户端
│   │   ├── HttpClient.h/cpp    # HTTP 客户端封装
│   │   └── TushareClient.h/cpp # Tushare API 客户端
│   ├── IDataSource.h      # 数据源接口
│   ├── TushareDataSource.h/cpp # Tushare 数据源实现
│   ├── DataSourceFactory.h/cpp # 数据源工厂
│   ├── Network.h          # 统一头文件
│   ├── *.md               # 文档
│   └── examples/          # 使用示例
├── thirdparty/            # 第三方库
│   ├── spdlog/
│   ├── dotenv-cpp/
│   ├── json/
│   ├── cpp-httplib/       # ⚠️ 仅 HTTP，禁用 SSL
│   ├── sqlpp11/
│   └── ta-lib/
├── main.cpp               # 主程序
├── CMakeLists.txt         # CMake 配置
├── .env                   # 环境变量配置（需创建）
├── env.example            # 配置示例
├── DESIGN.md              # 系统设计文档
├── QUICK_REFERENCE.md     # 快速参考
└── README.md              # 本文件
```

## 🔧 编译要求

- CMake 3.1+
- C++17 编译器
- 支持的平台：Linux, macOS, Windows

## 📝 配置说明

创建 `.env` 文件配置系统：

```env
# ========== 日志配置 ==========
# 日志级别: TRACE, DEBUG, INFO, WARN, ERROR, CRITICAL, OFF
LOG_LEVEL=DEBUG
LOG_PATTERN=[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%t] %v
LOG_FILE_PATH=logs/app.log
LOG_CONSOLE_ENABLED=true
LOG_FILE_ENABLED=true
LOG_MAX_FILE_SIZE=10485760  # 10MB
LOG_MAX_FILES=3
LOG_ASYNC_ENABLED=false
LOG_ASYNC_QUEUE_SIZE=8192

# ========== 数据源配置 ==========
# Tushare Pro API 配置（HTTP 协议）
DATA_SOURCE_URL=http://api.waditu.com
DATA_SOURCE_API_KEY=your_tushare_token_here
DATA_SOURCE_TIMEOUT=30
DATA_SOURCE_RETRY_TIMES=3
DATA_SOURCE_RETRY_DELAY=1000

# ========== 数据库配置 ==========
DB_HOST=localhost
DB_PORT=3306
DB_NAME=stock_db
DB_USER=root
DB_PASSWORD=password
```

## 🎨 设计亮点

### SOLID 原则

| 原则 | 应用 |
|------|------|
| 单一职责 (SRP) | 每个类只负责一个功能 |
| 开闭原则 (OCP) | 通过接口扩展，无需修改现有代码 |
| 里氏替换 (LSP) | 所有 ILogger 实现可互换 |
| 接口隔离 (ISP) | 接口只包含必要方法 |
| 依赖倒置 (DIP) | 依赖抽象接口而非具体实现 |

### 设计模式

- **工厂模式** (LoggerFactory) - 创建不同类型的日志器
- **单例模式** (LoggerManager) - 全局唯一的日志管理器
- **策略模式** (ILogger) - 不同的日志实现策略

## 📚 使用示例

### 基础日志

```cpp
#include "Logger.h"

int main() {
    log::init(".env");
    
    LOG_TRACE("追踪信息");
    LOG_DEBUG("调试信息");
    LOG_INFO("普通信息");
    LOG_WARN("警告信息");
    LOG_ERROR("错误信息");
    LOG_CRITICAL("严重错误");
    
    return 0;
}
```

### 命名日志器

```cpp
#include "Logger.h"

int main() {
    log::init(".env");
    
    // 为不同模块创建日志器
    auto db_logger = log::getLogger("database");
    db_logger->info("数据库连接成功");
    
    auto net_logger = log::getLogger("network");
    net_logger->info("网络请求完成");
    
    // 使用宏
    LOG_INFO_N("database", "查询完成");
    LOG_ERROR_N("network", "连接失败");
    
    return 0;
}
```

## 🚀 性能优化

### 开发环境

```env
LOG_LEVEL=DEBUG
LOG_ASYNC_ENABLED=false
```

### 生产环境

```env
LOG_LEVEL=INFO
LOG_ASYNC_ENABLED=true
LOG_MAX_FILE_SIZE=52428800  # 50MB
```

## 🧪 测试

```bash
# 编译
mkdir -p build && cd build
cmake ..
make

# 运行示例
./stock_for_cpp

# 查看日志
cat logs/app.log
```

## 📖 文档

### 日志系统文档

- **[快速入门](log/QUICKSTART.md)** - 5 分钟上手
- **[使用指南](log/USAGE.md)** - 详细用法和示例
- **[架构设计](log/ARCHITECTURE.md)** - SOLID 原则和设计模式
- **[项目总结](log/SUMMARY.md)** - 完整总结

## 🤝 贡献

欢迎提交 Issue 和 Pull Request！

## 📄 许可证

本项目基于以下开源库：
- spdlog (MIT License)
- dotenv-cpp (BSD License)
- nlohmann/json (MIT License)
- cpp-httplib (MIT License)
- sqlpp11 (BSD License)
- ta-lib (BSD License)

## 🎯 下一步计划

- [x] 完善日志系统（已完成）
- [x] 实现配置管理（已完成）
- [x] 完善股票数据获取模块（已完成）
- [x] 实现数据库存储功能（已完成）
- [x] 添加数据缓存（已完成）
- [ ] 实现技术分析指标
- [ ] 实现策略引擎
- [ ] 添加单元测试

## 📊 开发进度

- **基础设施层**: ✅ 100% (日志系统 + 配置系统)
- **网络层**: ✅ 100% (HTTP 客户端 + Tushare API)
- **数据层**: ✅ 100% (数据库 + 缓存 + 文件操作)
- **业务层**: ⏳ 0%
- **分析层**: ⏳ 0%
- **输出层**: ⏳ 0%

**整体进度**: 约 50%

## 📞 联系方式

如有问题或建议，请提交 Issue。

---

**版本**: 1.1.0  
**更新日期**: 2026-02-01  
**整体进度**: 50% (4/8 个主要模块)

