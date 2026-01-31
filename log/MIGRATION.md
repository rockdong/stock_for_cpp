# 日志系统更新说明

## 版本 2.0.0 (2026-02-01)

### 🔄 重大变更

#### 移除 dotenv 依赖

日志系统不再直接依赖 dotenv-cpp，改为依赖系统环境变量。配置的加载由配置模块统一管理。

### 📝 API 变更

#### 旧的初始化方式（已废弃）

```cpp
#include "Logger.h"

int main() {
    // 旧方式：直接加载 .env 文件
    logger::init(".env");  // ❌ 不再支持
    
    LOG_INFO("Hello");
    return 0;
}
```

#### 新的初始化方式（推荐）

**方式 1：使用配置模块（推荐）**

```cpp
#include "Logger.h"
#include "Config.h"

int main() {
    // 1. 先初始化配置模块（会加载 .env 文件）
    auto& config = config::Config::getInstance();
    config.initialize(".env");
    
    // 2. 初始化日志系统（从环境变量读取配置）
    logger::init();
    
    LOG_INFO("Hello");
    return 0;
}
```

**方式 2：直接使用环境变量**

```cpp
#include "Logger.h"

int main() {
    // 如果环境变量已经设置好，直接初始化
    logger::init();
    
    LOG_INFO("Hello");
    return 0;
}
```

**方式 3：使用自定义配置对象**

```cpp
#include "Logger.h"

int main() {
    // 创建自定义配置
    logger::LogConfig config;
    // config 会自动从环境变量读取
    
    // 使用自定义配置初始化
    logger::init(config);
    
    LOG_INFO("Hello");
    return 0;
}
```

### 🎯 变更原因

1. **统一配置管理**：所有配置由配置模块统一管理，避免重复加载 .env 文件
2. **解耦依赖**：日志系统不再直接依赖 dotenv-cpp，降低耦合度
3. **更灵活**：支持多种初始化方式，适应不同使用场景
4. **更清晰**：职责分离，配置加载由配置模块负责

### 📋 迁移指南

#### 步骤 1：更新代码

**旧代码**：
```cpp
#include "Logger.h"

int main() {
    logger::init(".env");
    LOG_INFO("Hello");
    return 0;
}
```

**新代码**：
```cpp
#include "Logger.h"
#include "Config.h"

int main() {
    // 初始化配置模块
    config::Config::getInstance().initialize(".env");
    
    // 初始化日志系统
    logger::init();
    
    LOG_INFO("Hello");
    return 0;
}
```

#### 步骤 2：更新 CMakeLists.txt

确保链接了 config_lib：

```cmake
target_link_libraries(your_target PRIVATE
    config_lib  # 添加配置库
    log_lib
    # ... 其他库
)
```

#### 步骤 3：测试

编译并运行，确保日志正常工作。

### 🔧 技术细节

#### Logger.h 变更

**移除**：
```cpp
#include "dotenv.h"

inline void init(const char* env_file = ".env") {
    dotenv::init(env_file);  // 移除
    LogConfig config;
    LoggerManager::getInstance().initialize(config);
}
```

**新增**：
```cpp
// 方式 1：使用配置对象
inline void init(const LogConfig& config) {
    LoggerManager::getInstance().initialize(config);
}

// 方式 2：使用默认配置（从环境变量）
inline void init() {
    LogConfig config;  // 自动从环境变量读取
    LoggerManager::getInstance().initialize(config);
}
```

#### LogConfig 行为

`LogConfig` 构造函数会自动从环境变量读取配置：

```cpp
LogConfig::LogConfig() {
    // 从环境变量读取
    log_level_ = getEnvString("LOG_LEVEL", "INFO");
    log_pattern_ = getEnvString("LOG_PATTERN", "...");
    // ...
}
```

### ✅ 兼容性

- ✅ **向后兼容**：通过配置模块可以实现相同的功能
- ✅ **更好的设计**：符合单一职责原则
- ✅ **更灵活**：支持多种初始化方式

### 📚 相关文档

- `log/README.md` - 日志系统使用文档
- `log/USAGE.md` - 详细使用指南
- `config/README.md` - 配置模块使用文档

### 🎉 优势

1. **统一管理**：所有配置由配置模块统一管理
2. **避免重复**：不会重复加载 .env 文件
3. **更清晰**：职责分离，代码更清晰
4. **更灵活**：支持多种初始化方式
5. **更易测试**：可以直接传入配置对象进行测试

### 💡 最佳实践

```cpp
#include "Logger.h"
#include "Config.h"

int main() {
    // 1. 初始化配置模块（统一入口）
    auto& config = config::Config::getInstance();
    if (!config.initialize(".env")) {
        std::cerr << "配置初始化失败" << std::endl;
        return 1;
    }
    
    // 2. 初始化日志系统
    logger::init();
    
    // 3. 使用日志
    LOG_INFO("应用启动");
    LOG_INFO("版本: " + config.getAppVersion());
    
    // 4. 使用其他配置
    std::cout << "数据库: " << config.getDbHost() << std::endl;
    
    return 0;
}
```

---

**更新日期**: 2026-02-01  
**版本**: 2.0.0  
**影响范围**: Logger.h, main.cpp

