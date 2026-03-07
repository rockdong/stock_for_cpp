# 日志系统封装

## 概述

这是一个基于 spdlog 的日志系统封装，遵循 SOLID 原则和多种设计模式，提供灵活、可扩展的日志功能。

## 设计原则

### SOLID 原则

1. **单一职责原则 (SRP)**
   - `LogConfig`: 只负责配置管理
   - `ILogger`: 只定义日志接口
   - `SpdlogLogger`: 只负责 spdlog 实现
   - `LoggerFactory`: 只负责创建日志器
   - `LoggerManager`: 只负责管理日志器生命周期

2. **开闭原则 (OCP)**
   - 通过 `ILogger` 接口扩展新的日志实现
   - 无需修改现有代码即可添加新的日志库支持

3. **里氏替换原则 (LSP)**
   - 所有 `ILogger` 的实现都可以互相替换

4. **接口隔离原则 (ISP)**
   - `ILogger` 接口只包含必要的日志方法

5. **依赖倒置原则 (DIP)**
   - 依赖于 `ILogger` 抽象接口，而非具体实现

### 设计模式

1. **工厂模式** (`LoggerFactory`)
   - 负责创建不同类型的日志器实例

2. **单例模式** (`LoggerManager`)
   - 全局唯一的日志管理器

3. **策略模式** (`ILogger`)
   - 不同的日志实现策略（spdlog, glog 等）

## 文件结构

```
log/
├── LogLevel.h          # 日志级别定义
├── LogConfig.h         # 日志配置类（头文件）
├── LogConfig.cpp       # 日志配置类（实现）
├── ILogger.h           # 日志接口
├── SpdlogLogger.h      # spdlog 实现（头文件）
├── SpdlogLogger.cpp    # spdlog 实现
├── LoggerFactory.h     # 日志工厂（头文件）
├── LoggerFactory.cpp   # 日志工厂（实现）
├── LoggerManager.h     # 日志管理器（头文件）
├── LoggerManager.cpp   # 日志管理器（实现）
└── Logger.h            # 统一头文件
```

## 配置说明

所有配置通过 `.env` 文件管理，支持以下配置项：

| 配置项 | 说明 | 默认值 |
|--------|------|--------|
| `LOG_LEVEL` | 日志级别 | INFO |
| `LOG_PATTERN` | 日志格式 | [%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%t] %v |
| `LOG_FILE_PATH` | 日志文件路径 | logs/app.log |
| `LOG_CONSOLE_ENABLED` | 是否启用控制台输出 | true |
| `LOG_FILE_ENABLED` | 是否启用文件输出 | true |
| `LOG_MAX_FILE_SIZE` | 单个日志文件最大大小（字节） | 10485760 (10MB) |
| `LOG_MAX_FILES` | 最大日志文件数量 | 3 |
| `LOG_ASYNC_ENABLED` | 是否启用异步日志 | false |
| `LOG_ASYNC_QUEUE_SIZE` | 异步日志队列大小 | 8192 |

## 使用示例

### 基本使用

```cpp
#include "Logger.h"

int main() {
    // 初始化日志系统
    logger::init(".env");
    
    // 使用宏记录日志
    LOG_INFO("应用程序启动");
    LOG_DEBUG("调试信息");
    LOG_WARN("警告信息");
    LOG_ERROR("错误信息");
    
    return 0;
}
```

### 使用命名日志器

```cpp
// 获取或创建命名日志器
auto db_logger = logger::getLogger("database");
db_logger->info("数据库连接成功");
db_logger->error("数据库查询失败");

// 使用宏
LOG_INFO_N("network", "网络请求完成");
```

### 自定义配置

```cpp
// 创建自定义配置
logger::LogConfig custom_config;
// 配置会自动从环境变量读取

// 创建自定义日志器
auto custom_logger = logger::LoggerManager::getInstance()
    .createLogger("custom", custom_config);
custom_logger->info("使用自定义配置");
```

### 刷新和关闭

```cpp
// 刷新所有日志
logger::LoggerManager::getInstance().flushAll();

// 关闭日志系统
logger::LoggerManager::getInstance().shutdown();
```

## 日志级别

支持以下日志级别（从低到高）：

- `TRACE`: 最详细的信息
- `DEBUG`: 调试信息
- `INFO`: 一般信息
- `WARN`: 警告信息
- `ERROR`: 错误信息
- `CRITICAL`: 严重错误
- `OFF`: 关闭日志

## 扩展性

### 添加新的日志库实现

1. 创建新的实现类继承 `ILogger`
2. 在 `LoggerFactory` 中添加新的类型
3. 实现所有虚函数

示例：

```cpp
class GlogLogger : public ILogger {
public:
    void info(const std::string& message) override {
        // glog 实现
    }
    // ... 其他方法
};

// 在 LoggerFactory 中添加
enum class LoggerType {
    SPDLOG,
    GLOG  // 新增
};
```

## 线程安全

- `LoggerManager` 使用互斥锁保证线程安全
- spdlog 本身是线程安全的
- 支持异步日志模式以提高性能

## 性能优化

1. 使用异步日志模式（设置 `LOG_ASYNC_ENABLED=true`）
2. 调整日志级别，生产环境建议使用 INFO 或更高级别
3. 合理设置日志文件大小和轮转数量
4. 使用宏定义避免不必要的字符串构造

## 注意事项

1. 确保在使用日志前调用 `logger::init()`
2. 程序退出前建议调用 `flushAll()` 确保日志写入
3. 日志文件目录会自动创建
4. 异步模式下，程序异常退出可能丢失部分日志

