# 日志系统架构设计文档

## 1. 系统概述

本日志系统是对 spdlog 开源库的封装，遵循 SOLID 原则和多种设计模式，提供灵活、可扩展、易用的日志功能。所有配置信息通过 `.env` 文件管理，使用 dotenv-cpp 库读取。

## 2. 设计目标

- **可扩展性**：易于添加新的日志库实现
- **易用性**：简单的 API，统一的接口
- **可配置性**：通过环境变量灵活配置
- **高性能**：支持异步日志，减少 I/O 阻塞
- **线程安全**：支持多线程环境
- **可维护性**：清晰的代码结构，遵循设计原则

## 3. SOLID 原则应用

### 3.1 单一职责原则 (Single Responsibility Principle)

每个类只负责一个功能：

| 类名 | 职责 |
|------|------|
| `LogLevel` | 定义日志级别枚举 |
| `LogConfig` | 管理日志配置，从环境变量读取 |
| `ILogger` | 定义日志接口规范 |
| `SpdlogLogger` | 实现 spdlog 日志功能 |
| `LoggerFactory` | 创建日志器实例 |
| `LoggerManager` | 管理日志器生命周期 |

### 3.2 开闭原则 (Open/Closed Principle)

- **对扩展开放**：通过 `ILogger` 接口可以添加新的日志库实现（如 glog, boost.log）
- **对修改关闭**：添加新实现不需要修改现有代码

```cpp
// 扩展示例：添加 glog 支持
class GlogLogger : public ILogger {
    // 实现接口方法
};

// 在工厂中注册
enum class LoggerType {
    SPDLOG,
    GLOG  // 新增，无需修改现有代码
};
```

### 3.3 里氏替换原则 (Liskov Substitution Principle)

所有 `ILogger` 的实现类都可以互相替换，不影响程序行为：

```cpp
LoggerPtr logger1 = std::make_shared<SpdlogLogger>(config);
LoggerPtr logger2 = std::make_shared<GlogLogger>(config);  // 可替换
```

### 3.4 接口隔离原则 (Interface Segregation Principle)

`ILogger` 接口只包含必要的日志方法，不强迫实现类依赖不需要的方法：

```cpp
class ILogger {
    virtual void trace(const std::string& message) = 0;
    virtual void debug(const std::string& message) = 0;
    virtual void info(const std::string& message) = 0;
    virtual void warn(const std::string& message) = 0;
    virtual void error(const std::string& message) = 0;
    virtual void critical(const std::string& message) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual void flush() = 0;
};
```

### 3.5 依赖倒置原则 (Dependency Inversion Principle)

- 高层模块（`LoggerManager`）依赖抽象（`ILogger`）
- 低层模块（`SpdlogLogger`）实现抽象
- 两者都依赖于抽象接口，而非具体实现

```cpp
class LoggerManager {
    std::unordered_map<std::string, LoggerPtr> loggers_;  // 依赖抽象
};
```

## 4. 设计模式应用

### 4.1 工厂模式 (Factory Pattern)

**类名**：`LoggerFactory`

**作用**：封装日志器的创建逻辑，根据类型创建不同的日志器实例。

**优点**：
- 解耦对象创建和使用
- 易于添加新的日志器类型
- 统一的创建接口

```cpp
class LoggerFactory {
public:
    enum class LoggerType {
        SPDLOG,
        // 可扩展其他类型
    };
    
    static LoggerPtr createLogger(
        LoggerType type,
        const LogConfig& config,
        const std::string& logger_name
    );
};
```

### 4.2 单例模式 (Singleton Pattern)

**类名**：`LoggerManager`

**作用**：确保全局只有一个日志管理器实例，统一管理所有日志器。

**优点**：
- 全局访问点
- 避免资源浪费
- 统一管理日志器生命周期

```cpp
class LoggerManager {
public:
    static LoggerManager& getInstance() {
        static LoggerManager instance;
        return instance;
    }
    
    // 禁用拷贝和赋值
    LoggerManager(const LoggerManager&) = delete;
    LoggerManager& operator=(const LoggerManager&) = delete;
    
private:
    LoggerManager() = default;
};
```

### 4.3 策略模式 (Strategy Pattern)

**接口**：`ILogger`

**作用**：定义日志记录的策略接口，不同的实现类提供不同的日志策略。

**优点**：
- 算法独立于使用它的客户端
- 易于切换不同的日志实现
- 符合开闭原则

```cpp
// 策略接口
class ILogger {
    virtual void info(const std::string& message) = 0;
};

// 具体策略 1
class SpdlogLogger : public ILogger {
    void info(const std::string& message) override {
        // spdlog 实现
    }
};

// 具体策略 2
class GlogLogger : public ILogger {
    void info(const std::string& message) override {
        // glog 实现
    }
};
```

## 5. 类图

```
┌─────────────────┐
│    LogLevel     │
│   (枚举类型)     │
├─────────────────┤
│ + TRACE         │
│ + DEBUG         │
│ + INFO          │
│ + WARN          │
│ + ERROR         │
│ + CRITICAL      │
│ + OFF           │
└─────────────────┘

┌─────────────────────────────────┐
│         LogConfig               │
│      (配置管理类)                │
├─────────────────────────────────┤
│ - log_level_: LogLevel          │
│ - log_pattern_: string          │
│ - log_file_path_: string        │
│ - console_enabled_: bool        │
│ - file_enabled_: bool           │
│ - max_file_size_: size_t        │
│ - max_files_: size_t            │
│ - async_enabled_: bool          │
│ - async_queue_size_: size_t     │
├─────────────────────────────────┤
│ + LogConfig()                   │
│ + getLogLevel(): LogLevel       │
│ + getLogPattern(): string       │
│ + isConsoleEnabled(): bool      │
│ + isFileEnabled(): bool         │
│ - getEnvString(): string        │
│ - getEnvBool(): bool            │
│ - getEnvSize(): size_t          │
└─────────────────────────────────┘

┌─────────────────────────────────┐
│          ILogger                │
│       (抽象接口)                 │
├─────────────────────────────────┤
│ + trace(message): void          │
│ + debug(message): void          │
│ + info(message): void           │
│ + warn(message): void           │
│ + error(message): void          │
│ + critical(message): void       │
│ + setLevel(level): void         │
│ + flush(): void                 │
└─────────────────────────────────┘
              △
              │ 实现
              │
┌─────────────┴───────────────────┐
│       SpdlogLogger              │
│      (spdlog 实现)              │
├─────────────────────────────────┤
│ - logger_: spdlog::logger       │
├─────────────────────────────────┤
│ + SpdlogLogger(config, name)    │
│ + trace(message): void          │
│ + debug(message): void          │
│ + info(message): void           │
│ + warn(message): void           │
│ + error(message): void          │
│ + critical(message): void       │
│ + setLevel(level): void         │
│ + flush(): void                 │
│ - initialize(config, name)      │
│ - toSpdlogLevel(level)          │
└─────────────────────────────────┘

┌─────────────────────────────────┐
│      LoggerFactory              │
│       (工厂类)                   │
├─────────────────────────────────┤
│ + createLogger(type, config)    │
│ + createDefaultLogger(config)   │
└─────────────────────────────────┘
              │ 创建
              ▼
┌─────────────────────────────────┐
│      LoggerManager              │
│       (单例类)                   │
├─────────────────────────────────┤
│ - loggers_: map<string, Logger> │
│ - default_config_: LogConfig    │
│ - mutex_: mutex                 │
│ - initialized_: bool            │
├─────────────────────────────────┤
│ + getInstance(): LoggerManager& │
│ + initialize(config): void      │
│ + getLogger(): LoggerPtr        │
│ + getLogger(name): LoggerPtr    │
│ + createLogger(name, config)    │
│ + removeLogger(name): void      │
│ + flushAll(): void              │
│ + shutdown(): void              │
└─────────────────────────────────┘
```

## 6. 时序图

### 6.1 初始化流程

```
用户代码          LoggerManager      LoggerFactory      SpdlogLogger      dotenv
   │                    │                  │                  │              │
   │ init(".env")       │                  │                  │              │
   ├───────────────────>│                  │                  │              │
   │                    │ dotenv::init()   │                  │              │
   │                    ├─────────────────────────────────────────────────>│
   │                    │                  │                  │              │
   │                    │ LogConfig()      │                  │              │
   │                    ├──────────────────┤                  │              │
   │                    │                  │                  │  getenv()    │
   │                    │                  │                  │<─────────────┤
   │                    │                  │                  │              │
   │                    │ createDefaultLogger()               │              │
   │                    ├─────────────────>│                  │              │
   │                    │                  │ new SpdlogLogger │              │
   │                    │                  ├─────────────────>│              │
   │                    │                  │                  │ initialize() │
   │                    │                  │                  ├──────────────┤
   │                    │                  │<─────────────────┤              │
   │                    │<─────────────────┤                  │              │
   │<───────────────────┤                  │                  │              │
```

### 6.2 日志记录流程

```
用户代码          LoggerManager      ILogger          SpdlogLogger      spdlog
   │                    │               │                  │              │
   │ LOG_INFO("msg")    │               │                  │              │
   ├───────────────────>│               │                  │              │
   │                    │ getLogger()   │                  │              │
   │                    ├──────────────>│                  │              │
   │                    │<──────────────┤                  │              │
   │                    │               │ info("msg")      │              │
   │                    │               ├─────────────────>│              │
   │                    │               │                  │ logger_->info()
   │                    │               │                  ├─────────────>│
   │                    │               │                  │              │
   │                    │               │                  │  写入控制台   │
   │                    │               │                  │  写入文件     │
   │                    │               │                  │<─────────────┤
   │                    │               │<─────────────────┤              │
   │<───────────────────┤               │                  │              │
```

## 7. 模块划分

### 7.1 核心模块

| 模块 | 文件 | 职责 |
|------|------|------|
| 日志级别 | `LogLevel.h` | 定义日志级别枚举 |
| 日志配置 | `LogConfig.h/cpp` | 管理配置，读取环境变量 |
| 日志接口 | `ILogger.h` | 定义日志接口规范 |

### 7.2 实现模块

| 模块 | 文件 | 职责 |
|------|------|------|
| spdlog 实现 | `SpdlogLogger.h/cpp` | 封装 spdlog 功能 |

### 7.3 管理模块

| 模块 | 文件 | 职责 |
|------|------|------|
| 日志工厂 | `LoggerFactory.h/cpp` | 创建日志器实例 |
| 日志管理器 | `LoggerManager.h/cpp` | 管理日志器生命周期 |

### 7.4 统一接口

| 模块 | 文件 | 职责 |
|------|------|------|
| 统一头文件 | `Logger.h` | 提供统一的使用接口 |

## 8. 配置管理

### 8.1 配置来源

所有配置通过环境变量管理，使用 dotenv-cpp 从 `.env` 文件读取：

```cpp
class LogConfig {
    LogConfig() {
        // 从环境变量读取
        log_level_ = stringToLogLevel(getEnvString("LOG_LEVEL", "INFO"));
        log_pattern_ = getEnvString("LOG_PATTERN", "[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%t] %v");
        // ...
    }
};
```

### 8.2 配置项

| 配置项 | 类型 | 默认值 | 说明 |
|--------|------|--------|------|
| LOG_LEVEL | string | INFO | 日志级别 |
| LOG_PATTERN | string | [%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%t] %v | 日志格式 |
| LOG_FILE_PATH | string | logs/app.log | 日志文件路径 |
| LOG_CONSOLE_ENABLED | bool | true | 控制台输出 |
| LOG_FILE_ENABLED | bool | true | 文件输出 |
| LOG_MAX_FILE_SIZE | size_t | 10485760 | 最大文件大小 |
| LOG_MAX_FILES | size_t | 3 | 最大文件数 |
| LOG_ASYNC_ENABLED | bool | false | 异步模式 |
| LOG_ASYNC_QUEUE_SIZE | size_t | 8192 | 异步队列大小 |

## 9. 线程安全

### 9.1 LoggerManager

使用 `std::mutex` 保护共享资源：

```cpp
class LoggerManager {
private:
    std::mutex mutex_;
    
public:
    LoggerPtr getLogger(const std::string& name) {
        std::lock_guard<std::mutex> lock(mutex_);
        // 访问共享资源
    }
};
```

### 9.2 spdlog

spdlog 本身是线程安全的，使用 `_mt` 后缀的 sink：

```cpp
auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(...);
```

## 10. 性能优化

### 10.1 异步日志

支持异步模式，减少 I/O 阻塞：

```cpp
if (config.isAsyncEnabled()) {
    spdlog::init_thread_pool(config.getAsyncQueueSize(), 1);
    logger_ = std::make_shared<spdlog::async_logger>(...);
}
```

### 10.2 宏定义

使用宏避免不必要的字符串构造：

```cpp
#define LOG_INFO(msg) log::getLogger()->info(msg)
```

### 10.3 日志级别过滤

在记录前检查日志级别，避免不必要的格式化：

```cpp
logger_->set_level(toSpdlogLevel(config.getLogLevel()));
```

## 11. 扩展性

### 11.1 添加新的日志库

1. 创建新的实现类继承 `ILogger`
2. 在 `LoggerFactory` 中注册新类型
3. 实现所有接口方法

### 11.2 添加新的配置项

1. 在 `LogConfig` 中添加成员变量
2. 在构造函数中读取环境变量
3. 提供 getter 方法

### 11.3 添加新的日志格式

通过 `LOG_PATTERN` 环境变量自定义格式，无需修改代码。

## 12. 测试建议

### 12.1 单元测试

- 测试 `LogConfig` 读取环境变量
- 测试 `LoggerFactory` 创建不同类型日志器
- 测试 `LoggerManager` 单例模式
- 测试日志级别过滤

### 12.2 集成测试

- 测试完整的日志记录流程
- 测试文件轮转功能
- 测试多线程环境
- 测试异步模式

### 12.3 性能测试

- 测试同步 vs 异步性能
- 测试不同日志级别的性能影响
- 测试高并发场景

## 13. 最佳实践

1. **统一使用宏定义**：`LOG_INFO()` 而非 `logger->info()`
2. **使用命名日志器**：区分不同模块
3. **合理设置日志级别**：开发用 DEBUG，生产用 INFO/WARN
4. **定期刷新日志**：关键操作后调用 `flush()`
5. **程序退出前关闭**：调用 `shutdown()`

## 14. 总结

本日志系统通过应用 SOLID 原则和设计模式，实现了：

- ✅ 高度可扩展：易于添加新的日志库实现
- ✅ 易于使用：简单的 API 和宏定义
- ✅ 灵活配置：通过环境变量管理所有配置
- ✅ 高性能：支持异步日志
- ✅ 线程安全：支持多线程环境
- ✅ 易于维护：清晰的代码结构和职责划分

这是一个生产级别的日志系统封装，可以直接用于实际项目中。

