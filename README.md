# Stock for C++ - 股票分析系统

## 项目简介

这是一个基于 C++ 的股票分析系统，集成了多个开源库，提供完整的数据处理、分析和日志功能。

## 📦 集成的开源库

- **spdlog** - 高性能日志库
- **dotenv-cpp** - 环境变量管理
- **nlohmann/json** - JSON 处理
- **cpp-httplib** - HTTP 客户端/服务器
- **sqlpp11** - SQL 查询构建器
- **ta-lib** - 技术分析库

## 🎯 日志系统

本项目已完成对 spdlog 的封装，提供了一个遵循 SOLID 原则和设计模式的日志系统。

### 特性

- ✅ **SOLID 原则**：完全遵循单一职责、开闭、里氏替换、接口隔离、依赖倒置原则
- ✅ **设计模式**：工厂模式、单例模式、策略模式
- ✅ **配置驱动**：所有配置通过 `.env` 文件管理
- ✅ **高性能**：支持异步日志、文件轮转
- ✅ **线程安全**：支持多线程环境
- ✅ **易于使用**：简单的宏定义和统一接口

### 快速开始

1. **创建配置文件**

```bash
cp env.example .env
```

2. **在代码中使用**

```cpp
#include "Logger.h"

int main() {
    // 初始化日志系统
    log::init(".env");
    
    // 记录日志
    LOG_INFO("应用程序启动");
    LOG_DEBUG("调试信息");
    LOG_ERROR("错误信息");
    
    return 0;
}
```

3. **编译运行**

```bash
mkdir -p build
cd build
cmake ..
make
./stock_for_cpp
```

### 日志系统文档

- **[QUICKSTART.md](log/QUICKSTART.md)** - 5 分钟快速入门
- **[README.md](log/README.md)** - 项目概述和配置说明
- **[USAGE.md](log/USAGE.md)** - 详细使用指南
- **[ARCHITECTURE.md](log/ARCHITECTURE.md)** - 架构设计文档
- **[SUMMARY.md](log/SUMMARY.md)** - 项目总结

## 📁 项目结构

```
stock_for_cpp/
├── log/                    # 日志系统封装
│   ├── LogLevel.h         # 日志级别定义
│   ├── LogConfig.h/cpp    # 配置管理
│   ├── ILogger.h          # 日志接口
│   ├── SpdlogLogger.h/cpp # spdlog 实现
│   ├── LoggerFactory.h/cpp # 日志工厂
│   ├── LoggerManager.h/cpp # 日志管理器
│   ├── Logger.h           # 统一头文件
│   └── *.md               # 文档
├── thirdparty/            # 第三方库
│   ├── spdlog/
│   ├── dotenv-cpp/
│   ├── json/
│   ├── cpp-httplib/
│   ├── sqlpp11/
│   └── ta-lib/
├── main.cpp               # 主程序
├── CMakeLists.txt         # CMake 配置
├── .env                   # 环境变量配置（需创建）
└── env.example            # 配置示例
```

## 🔧 编译要求

- CMake 3.1+
- C++17 编译器
- 支持的平台：Linux, macOS, Windows

## 📝 配置说明

创建 `.env` 文件配置日志系统：

```env
# 日志级别: TRACE, DEBUG, INFO, WARN, ERROR, CRITICAL, OFF
LOG_LEVEL=DEBUG

# 日志格式
LOG_PATTERN=[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%t] %v

# 日志文件路径
LOG_FILE_PATH=logs/app.log

# 启用控制台和文件输出
LOG_CONSOLE_ENABLED=true
LOG_FILE_ENABLED=true

# 文件轮转配置
LOG_MAX_FILE_SIZE=10485760  # 10MB
LOG_MAX_FILES=3

# 异步日志
LOG_ASYNC_ENABLED=false
LOG_ASYNC_QUEUE_SIZE=8192
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

- [ ] 完善股票数据获取模块
- [ ] 实现技术分析指标
- [ ] 添加数据库存储功能
- [ ] 实现 Web API 接口
- [ ] 添加单元测试

## 📞 联系方式

如有问题或建议，请提交 Issue。

---

**版本**: 1.0.0  
**更新日期**: 2026-01-31

