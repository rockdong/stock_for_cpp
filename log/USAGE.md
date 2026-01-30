# 日志系统使用指南

## 快速开始

### 1. 创建配置文件

复制 `env.example` 为 `.env`：

```bash
cp env.example .env
```

或手动创建 `.env` 文件，内容如下：

```env
# 日志配置
LOG_LEVEL=DEBUG
LOG_PATTERN=[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%t] %v
LOG_FILE_PATH=logs/app.log
LOG_CONSOLE_ENABLED=true
LOG_FILE_ENABLED=true
LOG_MAX_FILE_SIZE=10485760
LOG_MAX_FILES=3
LOG_ASYNC_ENABLED=false
LOG_ASYNC_QUEUE_SIZE=8192
```

### 2. 编译项目

```bash
mkdir -p build
cd build
cmake ..
make
```

### 3. 运行示例

```bash
./stock_for_cpp
```

## 代码示例

### 基础使用

```cpp
#include "Logger.h"

int main() {
    // 初始化日志系统
    log::init(".env");
    
    // 使用宏记录日志
    LOG_TRACE("这是追踪信息");
    LOG_DEBUG("这是调试信息");
    LOG_INFO("这是普通信息");
    LOG_WARN("这是警告信息");
    LOG_ERROR("这是错误信息");
    LOG_CRITICAL("这是严重错误");
    
    return 0;
}
```

### 使用命名日志器

```cpp
#include "Logger.h"

int main() {
    log::init(".env");
    
    // 获取命名日志器
    auto db_logger = log::getLogger("database");
    db_logger->info("数据库连接成功");
    db_logger->debug("执行 SQL: SELECT * FROM users");
    
    auto net_logger = log::getLogger("network");
    net_logger->info("HTTP 请求: GET /api/data");
    net_logger->warn("请求超时，正在重试...");
    
    // 使用宏（推荐）
    LOG_INFO_N("database", "查询完成，返回 100 条记录");
    LOG_ERROR_N("network", "连接失败");
    
    return 0;
}
```

### 自定义配置

```cpp
#include "Logger.h"

int main() {
    log::init(".env");
    
    // 创建自定义配置的日志器
    log::LogConfig custom_config;
    // 配置会自动从环境变量读取
    
    auto custom_logger = log::LoggerManager::getInstance()
        .createLogger("custom", custom_config);
    
    custom_logger->info("使用自定义配置");
    
    return 0;
}
```

### 动态修改日志级别

```cpp
#include "Logger.h"

int main() {
    log::init(".env");
    
    auto logger = log::getLogger();
    
    // 设置为 DEBUG 级别
    logger->setLevel(log::LogLevel::DEBUG);
    logger->debug("现在可以看到调试信息");
    
    // 设置为 ERROR 级别
    logger->setLevel(log::LogLevel::ERROR);
    logger->info("这条信息不会显示");
    logger->error("只有错误及以上级别会显示");
    
    return 0;
}
```

### 刷新和关闭

```cpp
#include "Logger.h"

int main() {
    log::init(".env");
    
    LOG_INFO("开始处理任务");
    
    // 手动刷新所有日志
    log::LoggerManager::getInstance().flushAll();
    
    LOG_INFO("任务完成");
    
    // 程序退出前关闭日志系统
    log::LoggerManager::getInstance().shutdown();
    
    return 0;
}
```

## 配置说明

### 日志级别

- `TRACE`: 最详细的信息，用于追踪程序执行流程
- `DEBUG`: 调试信息，开发阶段使用
- `INFO`: 一般信息，记录重要的业务流程
- `WARN`: 警告信息，潜在的问题
- `ERROR`: 错误信息，程序出现错误但可以继续运行
- `CRITICAL`: 严重错误，程序可能无法继续运行
- `OFF`: 关闭日志

### 日志格式

日志格式使用 spdlog 的格式化语法：

- `%Y-%m-%d`: 日期（年-月-日）
- `%H:%M:%S.%e`: 时间（时:分:秒.毫秒）
- `%l`: 日志级别
- `%^%l%$`: 带颜色的日志级别
- `%t`: 线程 ID
- `%v`: 日志消息内容
- `%n`: 日志器名称
- `%P`: 进程 ID

示例格式：
```
[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%n] [%t] %v
```

输出：
```
[2026-01-31 10:30:45.123] [INFO] [default] [12345] 应用程序启动
```

### 文件轮转

当日志文件达到 `LOG_MAX_FILE_SIZE` 时，会自动创建新文件：

```
logs/app.log      # 当前日志
logs/app.1.log    # 第一个轮转文件
logs/app.2.log    # 第二个轮转文件
```

最多保留 `LOG_MAX_FILES` 个文件。

### 异步日志

异步日志可以提高性能，但程序异常退出时可能丢失部分日志：

```env
LOG_ASYNC_ENABLED=true
LOG_ASYNC_QUEUE_SIZE=8192
```

建议：
- 开发环境：使用同步日志（`false`）
- 生产环境：根据性能需求选择

## 架构设计

### 类图

```
┌─────────────┐
│  LogLevel   │ (枚举)
└─────────────┘

┌─────────────┐
│  LogConfig  │ (配置类)
└─────────────┘

┌─────────────┐
│   ILogger   │ (接口)
└─────────────┘
       △
       │
┌──────┴──────┐
│SpdlogLogger │ (实现)
└─────────────┘

┌──────────────┐
│LoggerFactory │ (工厂)
└──────────────┘

┌──────────────┐
│LoggerManager │ (单例)
└──────────────┘
```

### SOLID 原则应用

1. **单一职责原则 (SRP)**
   - 每个类只负责一个功能
   - `LogConfig` 只管配置
   - `LoggerFactory` 只管创建
   - `LoggerManager` 只管生命周期

2. **开闭原则 (OCP)**
   - 通过 `ILogger` 接口扩展
   - 添加新日志库无需修改现有代码

3. **里氏替换原则 (LSP)**
   - 所有 `ILogger` 实现可互换

4. **接口隔离原则 (ISP)**
   - 接口只包含必要方法

5. **依赖倒置原则 (DIP)**
   - 依赖抽象接口而非具体实现

### 设计模式

1. **工厂模式** - `LoggerFactory`
2. **单例模式** - `LoggerManager`
3. **策略模式** - `ILogger` 接口

## 扩展开发

### 添加新的日志库

1. 创建新的实现类：

```cpp
// GlogLogger.h
#include "ILogger.h"
#include <glog/logging.h>

namespace log {

class GlogLogger : public ILogger {
public:
    explicit GlogLogger(const LogConfig& config, const std::string& logger_name);
    
    void trace(const std::string& message) override;
    void debug(const std::string& message) override;
    void info(const std::string& message) override;
    void warn(const std::string& message) override;
    void error(const std::string& message) override;
    void critical(const std::string& message) override;
    void setLevel(LogLevel level) override;
    void flush() override;
};

} // namespace log
```

2. 在 `LoggerFactory` 中注册：

```cpp
enum class LoggerType {
    SPDLOG,
    GLOG  // 新增
};

LoggerPtr LoggerFactory::createLogger(
    LoggerType type,
    const LogConfig& config,
    const std::string& logger_name
) {
    switch (type) {
        case LoggerType::SPDLOG:
            return std::make_shared<SpdlogLogger>(config, logger_name);
        case LoggerType::GLOG:
            return std::make_shared<GlogLogger>(config, logger_name);
        default:
            throw std::runtime_error("不支持的日志器类型");
    }
}
```

## 性能优化建议

1. **使用异步日志**
   - 生产环境启用异步模式
   - 合理设置队列大小

2. **调整日志级别**
   - 开发：DEBUG
   - 测试：INFO
   - 生产：WARN 或 ERROR

3. **使用宏定义**
   - 宏可以避免不必要的字符串构造
   - 推荐使用 `LOG_INFO()` 而非 `logger->info()`

4. **合理设置文件大小**
   - 避免单个文件过大
   - 根据磁盘空间设置轮转数量

## 常见问题

### Q: 日志文件没有创建？
A: 检查 `LOG_FILE_PATH` 的父目录是否有写权限，程序会自动创建目录。

### Q: 看不到 DEBUG 日志？
A: 检查 `LOG_LEVEL` 设置，确保级别不高于 DEBUG。

### Q: 程序崩溃后日志丢失？
A: 异步模式下可能丢失，建议关键日志使用同步模式或手动调用 `flush()`。

### Q: 如何在多线程中使用？
A: 日志系统是线程安全的，可以在多线程中直接使用。

### Q: 如何禁用控制台输出？
A: 设置 `LOG_CONSOLE_ENABLED=false`。

## 最佳实践

1. **统一使用宏定义**
   ```cpp
   LOG_INFO("用户登录成功");  // 推荐
   logger->info("用户登录成功");  // 不推荐
   ```

2. **使用命名日志器区分模块**
   ```cpp
   LOG_INFO_N("database", "连接成功");
   LOG_INFO_N("network", "请求完成");
   ```

3. **记录关键信息**
   - 程序启动/退出
   - 重要业务操作
   - 错误和异常
   - 性能指标

4. **避免敏感信息**
   - 不要记录密码
   - 不要记录完整的信用卡号
   - 注意个人隐私信息

5. **定期清理日志**
   - 设置合理的轮转策略
   - 定期归档或删除旧日志

## 许可证

本日志系统基于 spdlog 和 dotenv-cpp 开发，遵循相应的开源许可证。

