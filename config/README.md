# 配置模块

## 概述

配置模块提供全局配置管理功能，采用单例模式，线程安全，从 `.env` 文件读取所有系统配置。

## 设计原则

- **单例模式**: 全局唯一的配置管理器
- **单一职责**: 只负责配置的读取和管理
- **线程安全**: 使用互斥锁保护配置访问
- **配置驱动**: 所有配置通过环境变量管理

## 核心类

### Config

全局配置管理类，提供系统所有模块的配置访问接口。

#### 主要方法

```cpp
// 获取单例实例
static Config& getInstance();

// 初始化配置
bool initialize(const char* env_file = ".env");

// 重新加载配置
bool reload(const char* env_file = ".env");

// 检查是否已初始化
bool isInitialized() const;
```

## 配置分类

### 1. 日志配置

| 方法 | 说明 | 默认值 |
|------|------|--------|
| `getLogLevel()` | 日志级别 | INFO |
| `getLogPattern()` | 日志格式 | [%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%t] %v |
| `getLogFilePath()` | 日志文件路径 | logs/app.log |
| `isLogConsoleEnabled()` | 是否启用控制台输出 | true |
| `isLogFileEnabled()` | 是否启用文件输出 | true |
| `getLogMaxFileSize()` | 最大文件大小 | 10485760 (10MB) |
| `getLogMaxFiles()` | 最大文件数 | 3 |
| `isLogAsyncEnabled()` | 是否启用异步日志 | false |
| `getLogAsyncQueueSize()` | 异步队列大小 | 8192 |

### 2. 数据库配置

| 方法 | 说明 | 默认值 |
|------|------|--------|
| `getDbHost()` | 数据库主机 | localhost |
| `getDbPort()` | 数据库端口 | 3306 |
| `getDbName()` | 数据库名称 | stock_db |
| `getDbUser()` | 数据库用户 | root |
| `getDbPassword()` | 数据库密码 | (空) |
| `getDbPoolSize()` | 连接池大小 | 10 |
| `getDbTimeout()` | 连接超时(秒) | 30 |
| `getDbCharset()` | 字符集 | utf8mb4 |
| `getDbConnectionString()` | 连接字符串 | - |

### 3. 数据源配置

| 方法 | 说明 | 默认值 |
|------|------|--------|
| `getDataSourceUrl()` | 数据源 URL | https://api.example.com |
| `getDataSourceApiKey()` | API 密钥 | (空) |
| `getDataSourceTimeout()` | 请求超时(秒) | 10 |
| `getDataSourceRetryTimes()` | 重试次数 | 3 |
| `getDataSourceRetryDelay()` | 重试延迟(毫秒) | 1000 |

### 4. 缓存配置

| 方法 | 说明 | 默认值 |
|------|------|--------|
| `isCacheEnabled()` | 是否启用缓存 | true |
| `getCacheSize()` | 缓存大小 | 1000 |
| `getCacheTtl()` | 缓存过期时间(秒) | 300 |
| `getCachePolicy()` | 缓存策略 | LRU |

### 5. 输出配置

| 方法 | 说明 | 默认值 |
|------|------|--------|
| `getOutputDir()` | 输出目录 | output |
| `getExportFormat()` | 导出格式 | csv |
| `isChartEnabled()` | 是否启用图表 | false |
| `getConsoleWidth()` | 控制台宽度 | 120 |

### 6. 分析配置

| 方法 | 说明 | 默认值 |
|------|------|--------|
| `getDefaultMaPeriod()` | MA 周期 | 20 |
| `getDefaultMacdFast()` | MACD 快线周期 | 12 |
| `getDefaultMacdSlow()` | MACD 慢线周期 | 26 |
| `getDefaultMacdSignal()` | MACD 信号线周期 | 9 |
| `getDefaultRsiPeriod()` | RSI 周期 | 14 |

### 7. 应用配置

| 方法 | 说明 | 默认值 |
|------|------|--------|
| `getAppName()` | 应用名称 | Stock Analysis System |
| `getAppVersion()` | 应用版本 | 1.0.0 |
| `getAppEnv()` | 运行环境 | development |
| `isDebugMode()` | 是否调试模式 | false |

## 使用示例

### 基础使用

```cpp
#include "Config.h"

int main() {
    // 初始化配置
    auto& config = config::Config::getInstance();
    if (!config.initialize(".env")) {
        std::cerr << "配置初始化失败" << std::endl;
        return 1;
    }
    
    // 读取配置
    std::cout << "应用名称: " << config.getAppName() << std::endl;
    std::cout << "数据库主机: " << config.getDbHost() << std::endl;
    std::cout << "日志级别: " << config.getLogLevel() << std::endl;
    
    return 0;
}
```

### 数据库连接

```cpp
#include "Config.h"

void connectDatabase() {
    auto& config = config::Config::getInstance();
    
    std::string connStr = config.getDbConnectionString();
    std::cout << "连接字符串: " << connStr << std::endl;
    
    // 使用连接字符串连接数据库
    // ...
}
```

### 重新加载配置

```cpp
#include "Config.h"

void reloadConfig() {
    auto& config = config::Config::getInstance();
    
    if (config.reload(".env")) {
        std::cout << "配置重新加载成功" << std::endl;
    } else {
        std::cerr << "配置重新加载失败" << std::endl;
    }
}
```

### 条件配置

```cpp
#include "Config.h"

void setupCache() {
    auto& config = config::Config::getInstance();
    
    if (config.isCacheEnabled()) {
        size_t cacheSize = config.getCacheSize();
        int cacheTtl = config.getCacheTtl();
        
        std::cout << "启用缓存: 大小=" << cacheSize 
                  << ", TTL=" << cacheTtl << "秒" << std::endl;
        
        // 初始化缓存
        // ...
    } else {
        std::cout << "缓存已禁用" << std::endl;
    }
}
```

## 配置文件示例

创建 `.env` 文件：

```env
# ========== 应用配置 ==========
APP_NAME=Stock Analysis System
APP_VERSION=1.0.0
APP_ENV=production
DEBUG_MODE=false

# ========== 日志配置 ==========
LOG_LEVEL=INFO
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
DB_PASSWORD=your_password
DB_POOL_SIZE=10
DB_TIMEOUT=30
DB_CHARSET=utf8mb4

# ========== 数据源配置 ==========
DATA_SOURCE_URL=https://api.example.com
DATA_SOURCE_API_KEY=your_api_key
DATA_SOURCE_TIMEOUT=10
DATA_SOURCE_RETRY_TIMES=3
DATA_SOURCE_RETRY_DELAY=1000

# ========== 缓存配置 ==========
CACHE_ENABLED=true
CACHE_SIZE=1000
CACHE_TTL=300
CACHE_POLICY=LRU

# ========== 输出配置 ==========
OUTPUT_DIR=output
EXPORT_FORMAT=csv
CHART_ENABLED=false
CONSOLE_WIDTH=120

# ========== 分析配置 ==========
DEFAULT_MA_PERIOD=20
DEFAULT_MACD_FAST=12
DEFAULT_MACD_SLOW=26
DEFAULT_MACD_SIGNAL=9
DEFAULT_RSI_PERIOD=14
```

## 线程安全

配置模块使用互斥锁保证线程安全：

```cpp
// 内部实现
std::lock_guard<std::mutex> lock(mutex_);
```

可以在多线程环境中安全使用：

```cpp
#include <thread>
#include "Config.h"

void worker() {
    auto& config = config::Config::getInstance();
    std::cout << "线程 " << std::this_thread::get_id() 
              << " 读取配置: " << config.getAppName() << std::endl;
}

int main() {
    auto& config = config::Config::getInstance();
    config.initialize(".env");
    
    std::thread t1(worker);
    std::thread t2(worker);
    
    t1.join();
    t2.join();
    
    return 0;
}
```

## 最佳实践

1. **尽早初始化**
   ```cpp
   int main() {
       // 程序启动时立即初始化配置
       config::Config::getInstance().initialize(".env");
       // ...
   }
   ```

2. **检查初始化状态**
   ```cpp
   auto& config = config::Config::getInstance();
   if (!config.isInitialized()) {
       std::cerr << "配置未初始化" << std::endl;
       return 1;
   }
   ```

3. **使用引用避免拷贝**
   ```cpp
   auto& config = config::Config::getInstance();  // 使用引用
   ```

4. **敏感信息保护**
   - 不要在日志中输出密码
   - 使用环境变量而非硬编码
   - 生产环境使用强密码

5. **配置验证**
   ```cpp
   auto& config = config::Config::getInstance();
   if (config.getDbPassword().empty()) {
       std::cerr << "警告: 数据库密码为空" << std::endl;
   }
   ```

## 注意事项

1. 配置模块是单例，全局只有一个实例
2. 初始化后不能再次初始化，使用 `reload()` 重新加载
3. 所有配置都有默认值，即使 .env 文件不存在也能运行
4. 配置读取是线程安全的
5. 修改 .env 文件后需要调用 `reload()` 才能生效

## 错误处理

配置模块会在以下情况输出错误信息：

- .env 文件不存在（使用默认值）
- 配置值格式错误（使用默认值）
- 重复初始化（提示使用 reload）

所有错误都会输出到 `std::cerr`，不会抛出异常。

