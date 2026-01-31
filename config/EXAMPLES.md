# 配置模块测试示例

## 编译和运行

```bash
# 创建 .env 文件
cp env.example .env

# 编译
mkdir -p build && cd build
cmake ..
make

# 运行
./stock_for_cpp
```

## 测试用例

### 1. 基础配置读取

```cpp
#include "Config.h"
#include <iostream>

int main() {
    auto& config = config::Config::getInstance();
    config.initialize(".env");
    
    std::cout << "应用名称: " << config.getAppName() << std::endl;
    std::cout << "应用版本: " << config.getAppVersion() << std::endl;
    std::cout << "运行环境: " << config.getAppEnv() << std::endl;
    
    return 0;
}
```

### 2. 数据库配置

```cpp
#include "Config.h"
#include <iostream>

int main() {
    auto& config = config::Config::getInstance();
    config.initialize(".env");
    
    std::cout << "数据库连接信息:" << std::endl;
    std::cout << "  主机: " << config.getDbHost() << std::endl;
    std::cout << "  端口: " << config.getDbPort() << std::endl;
    std::cout << "  数据库: " << config.getDbName() << std::endl;
    std::cout << "  用户: " << config.getDbUser() << std::endl;
    std::cout << "  连接字符串: " << config.getDbConnectionString() << std::endl;
    
    return 0;
}
```

### 3. 条件配置

```cpp
#include "Config.h"
#include <iostream>

int main() {
    auto& config = config::Config::getInstance();
    config.initialize(".env");
    
    if (config.isCacheEnabled()) {
        std::cout << "缓存已启用" << std::endl;
        std::cout << "  大小: " << config.getCacheSize() << std::endl;
        std::cout << "  TTL: " << config.getCacheTtl() << "秒" << std::endl;
    } else {
        std::cout << "缓存已禁用" << std::endl;
    }
    
    if (config.isDebugMode()) {
        std::cout << "调试模式已启用" << std::endl;
    }
    
    return 0;
}
```

### 4. 配置重载

```cpp
#include "Config.h"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    auto& config = config::Config::getInstance();
    config.initialize(".env");
    
    std::cout << "初始日志级别: " << config.getLogLevel() << std::endl;
    
    std::cout << "请修改 .env 文件中的 LOG_LEVEL，然后按回车..." << std::endl;
    std::cin.get();
    
    config.reload(".env");
    std::cout << "重载后日志级别: " << config.getLogLevel() << std::endl;
    
    return 0;
}
```

### 5. 多线程测试

```cpp
#include "Config.h"
#include <iostream>
#include <thread>
#include <vector>

void worker(int id) {
    auto& config = config::Config::getInstance();
    
    for (int i = 0; i < 5; ++i) {
        std::cout << "线程 " << id << ": " 
                  << config.getAppName() << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main() {
    auto& config = config::Config::getInstance();
    config.initialize(".env");
    
    std::vector<std::thread> threads;
    for (int i = 0; i < 3; ++i) {
        threads.emplace_back(worker, i);
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    return 0;
}
```

## 预期输出

运行 `./stock_for_cpp` 应该看到类似以下输出：

```
[2026-01-31 10:30:45.123] [INFO] [12345] ========================================
[2026-01-31 10:30:45.124] [INFO] [12345] 应用名称: Stock Analysis System
[2026-01-31 10:30:45.124] [INFO] [12345] 应用版本: 1.0.0
[2026-01-31 10:30:45.124] [INFO] [12345] 运行环境: development
[2026-01-31 10:30:45.124] [INFO] [12345] ========================================
[2026-01-31 10:30:45.125] [INFO] [12345] 数据库配置:
[2026-01-31 10:30:45.125] [INFO] [12345]   主机: localhost
[2026-01-31 10:30:45.125] [INFO] [12345]   端口: 3306
[2026-01-31 10:30:45.125] [INFO] [12345]   数据库: stock_db
[2026-01-31 10:30:45.125] [INFO] [12345]   连接池大小: 10
[2026-01-31 10:30:45.126] [INFO] [12345] 数据源配置:
[2026-01-31 10:30:45.126] [INFO] [12345]   URL: https://api.example.com
[2026-01-31 10:30:45.126] [INFO] [12345]   超时: 10秒
[2026-01-31 10:30:45.126] [INFO] [12345]   重试次数: 3
[2026-01-31 10:30:45.127] [INFO] [12345] 缓存配置:
[2026-01-31 10:30:45.127] [INFO] [12345]   启用: 是
[2026-01-31 10:30:45.127] [INFO] [12345]   大小: 1000
[2026-01-31 10:30:45.127] [INFO] [12345]   TTL: 300秒
[2026-01-31 10:30:45.128] [INFO] [12345] 输出配置:
[2026-01-31 10:30:45.128] [INFO] [12345]   输出目录: output
[2026-01-31 10:30:45.128] [INFO] [12345]   导出格式: csv
[2026-01-31 10:30:45.128] [INFO] [12345]   图表: 禁用
[2026-01-31 10:30:45.129] [INFO] [12345] 分析配置:
[2026-01-31 10:30:45.129] [INFO] [12345]   MA周期: 20
[2026-01-31 10:30:45.129] [INFO] [12345]   MACD快线: 12
[2026-01-31 10:30:45.129] [INFO] [12345]   MACD慢线: 26
[2026-01-31 10:30:45.129] [INFO] [12345]   RSI周期: 14
[2026-01-31 10:30:45.130] [INFO] [12345] ========================================
[2026-01-31 10:30:45.130] [INFO] [12345] 数据库模块初始化完成
[2026-01-31 10:30:45.130] [INFO] [12345] 分析模块初始化完成
[2026-01-31 10:30:45.131] [INFO] [12345] 应用程序正常退出
```

## 常见问题

### Q: 配置初始化失败？
A: 检查 .env 文件是否存在，如果不存在，复制 env.example：
```bash
cp env.example .env
```

### Q: 某些配置值不生效？
A: 确保 .env 文件中的配置项名称正确，没有拼写错误。

### Q: 如何在运行时修改配置？
A: 修改 .env 文件后，调用 `config.reload(".env")` 重新加载。

### Q: 配置是否线程安全？
A: 是的，配置模块使用互斥锁保证线程安全。

