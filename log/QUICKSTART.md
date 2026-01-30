# 日志系统快速入门

## 🚀 5 分钟快速开始

### 步骤 1: 创建配置文件

在项目根目录创建 `.env` 文件：

```bash
# 复制示例配置
cp env.example .env
```

或手动创建 `.env` 文件，内容如下：

```env
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

### 步骤 2: 在代码中使用

```cpp
#include "Logger.h"

int main() {
    // 初始化日志系统
    log::init(".env");
    
    // 记录日志
    LOG_INFO("应用程序启动");
    LOG_DEBUG("调试信息");
    LOG_WARN("警告信息");
    LOG_ERROR("错误信息");
    
    return 0;
}
```

### 步骤 3: 编译运行

```bash
mkdir -p build
cd build
cmake ..
make
./stock_for_cpp
```

## 📝 常用功能

### 1. 基础日志记录

```cpp
LOG_TRACE("追踪信息");
LOG_DEBUG("调试信息");
LOG_INFO("普通信息");
LOG_WARN("警告信息");
LOG_ERROR("错误信息");
LOG_CRITICAL("严重错误");
```

### 2. 命名日志器（推荐用于模块化）

```cpp
// 为不同模块创建日志器
auto db_logger = log::getLogger("database");
db_logger->info("数据库连接成功");

auto net_logger = log::getLogger("network");
net_logger->info("网络请求完成");

// 或使用宏
LOG_INFO_N("database", "查询完成");
LOG_ERROR_N("network", "连接失败");
```

### 3. 动态调整日志级别

```cpp
auto logger = log::getLogger();
logger->setLevel(log::LogLevel::DEBUG);  // 设置为 DEBUG
logger->setLevel(log::LogLevel::ERROR);  // 设置为 ERROR
```

### 4. 手动刷新日志

```cpp
// 刷新所有日志（确保写入磁盘）
log::LoggerManager::getInstance().flushAll();
```

## ⚙️ 配置说明

| 配置项 | 说明 | 默认值 |
|--------|------|--------|
| `LOG_LEVEL` | 日志级别 (TRACE/DEBUG/INFO/WARN/ERROR/CRITICAL/OFF) | INFO |
| `LOG_PATTERN` | 日志格式 | [%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%t] %v |
| `LOG_FILE_PATH` | 日志文件路径 | logs/app.log |
| `LOG_CONSOLE_ENABLED` | 是否输出到控制台 | true |
| `LOG_FILE_ENABLED` | 是否输出到文件 | true |
| `LOG_MAX_FILE_SIZE` | 单个文件最大大小（字节） | 10485760 (10MB) |
| `LOG_MAX_FILES` | 最大文件数量 | 3 |
| `LOG_ASYNC_ENABLED` | 是否启用异步日志 | false |
| `LOG_ASYNC_QUEUE_SIZE` | 异步队列大小 | 8192 |

## 💡 使用建议

### 开发环境配置

```env
LOG_LEVEL=DEBUG
LOG_CONSOLE_ENABLED=true
LOG_FILE_ENABLED=true
LOG_ASYNC_ENABLED=false
```

### 生产环境配置

```env
LOG_LEVEL=INFO
LOG_CONSOLE_ENABLED=false
LOG_FILE_ENABLED=true
LOG_ASYNC_ENABLED=true
LOG_MAX_FILE_SIZE=52428800  # 50MB
LOG_MAX_FILES=10
```

## 🎯 最佳实践

1. **使用宏定义**
   ```cpp
   LOG_INFO("消息");  // ✅ 推荐
   logger->info("消息");  // ❌ 不推荐
   ```

2. **使用命名日志器区分模块**
   ```cpp
   LOG_INFO_N("database", "数据库操作");
   LOG_INFO_N("network", "网络操作");
   LOG_INFO_N("business", "业务逻辑");
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

## 📚 更多文档

- **README.md** - 项目概述和配置说明
- **USAGE.md** - 详细使用指南和代码示例
- **ARCHITECTURE.md** - 架构设计和 SOLID 原则详解
- **SUMMARY.md** - 项目总结

## ❓ 常见问题

**Q: 日志文件没有创建？**  
A: 程序会自动创建目录，检查父目录是否有写权限。

**Q: 看不到 DEBUG 日志？**  
A: 检查 `LOG_LEVEL` 设置，确保不高于 DEBUG。

**Q: 程序崩溃后日志丢失？**  
A: 异步模式下可能丢失，使用同步模式或手动调用 `flush()`。

**Q: 如何禁用控制台输出？**  
A: 设置 `LOG_CONSOLE_ENABLED=false`。

**Q: 如何在多线程中使用？**  
A: 日志系统是线程安全的，可以直接在多线程中使用。

## 🎉 完成！

现在你已经掌握了日志系统的基本使用，可以开始在项目中使用了！

如需了解更多高级功能，请查看 `USAGE.md` 和 `ARCHITECTURE.md`。

