# StrategyManager 使用文档

## 概述

`StrategyManager` 是一个策略管理器，可以从配置文件（`.env`）中读取策略列表并自动注册。这样可以灵活地配置要使用的策略，而无需修改代码。

## 配置方式

### 1. 在 .env 文件中配置

在 `.env` 文件中添加 `STRATEGIES` 配置项：

```bash
# 策略配置 - 使用分号分隔多个策略
STRATEGIES=EMA17TO25;MACD;RSI;BOLL
```

### 2. 支持的策略名称

| 配置名称 | 策略类 | 说明 |
|---------|--------|------|
| `EMA17TO25` | EMA17TO25Strategy | EMA17/EMA25 交叉策略 |
| `MA_CROSS` | MACrossStrategy | 双均线交叉策略 |
| `MACD` | MACDStrategy | MACD 指标策略 |
| `RSI` | RSIStrategy | RSI 指标策略 |
| `BOLL` | BOLLStrategy | 布林带策略 |
| `GRID` | GridStrategy | 网格交易策略 |

**注意**：策略名称不区分大小写，会自动转换为大写进行匹配。

## 使用方法

### 基本使用

```cpp
#include "Core.h"
#include "Config.h"

int main() {
    // 1. 初始化配置系统
    auto& config = config::Config::getInstance();
    config.initialize(".env");
    
    // 2. 初始化策略管理器（从环境变量加载）
    auto& manager = core::StrategyManager::getInstance();
    manager.initializeFromEnv("STRATEGIES");
    
    // 3. 查看已注册的策略
    std::cout << "已注册 " << manager.count() << " 个策略:" << std::endl;
    for (const auto& name : manager.getStrategyNames()) {
        std::cout << "  - " << name << std::endl;
    }
    
    return 0;
}
```

### 从配置字符串初始化

```cpp
auto& manager = core::StrategyManager::getInstance();

// 直接传入配置字符串
manager.initialize("EMA17TO25;MACD;RSI");
```

### 获取和使用策略

```cpp
// 获取单个策略
auto strategy = manager.getStrategy("MACD");
if (strategy) {
    auto result = strategy->analyze(tsCode, data);
    if (result.has_value()) {
        std::cout << "分析结果: " << result->label << std::endl;
    }
}

// 检查策略是否存在
if (manager.hasStrategy("RSI")) {
    std::cout << "RSI 策略已注册" << std::endl;
}
```

### 使用所有策略进行分析

```cpp
// 对所有已注册的策略进行分析
auto results = manager.analyzeAll("000001.SZ", stockData);

// 遍历结果
for (const auto& pair : results) {
    const auto& strategyName = pair.first;
    const auto& result = pair.second;
    
    if (result.has_value()) {
        std::cout << strategyName << ": " << result->label << std::endl;
    } else {
        std::cout << strategyName << ": 数据不足" << std::endl;
    }
}
```

### 与数据库集成

```cpp
#include "Core.h"
#include "Data.h"
#include "Config.h"

int main() {
    // 初始化
    auto& config = config::Config::getInstance();
    config.initialize(".env");
    
    auto& conn = data::Connection::getInstance();
    conn.initialize("stock.db");
    conn.connect();
    
    // 初始化策略管理器
    auto& manager = core::StrategyManager::getInstance();
    manager.initializeFromEnv("STRATEGIES");
    
    // 创建 DAO
    data::AnalysisResultDAO analysisDao;
    data::StockDAO stockDao;
    
    // 获取股票列表
    auto stocks = stockDao.findAll();
    
    // 对每只股票使用所有策略分析
    for (const auto& stock : stocks) {
        auto data = getStockData(stock.ts_code);
        
        // 使用所有策略分析
        auto results = manager.analyzeAll(stock.ts_code, data);
        
        // 保存所有有效结果到数据库
        for (const auto& pair : results) {
            if (pair.second.has_value()) {
                analysisDao.insert(*pair.second);
            }
        }
    }
    
    conn.disconnect();
    return 0;
}
```

## API 参考

### StrategyManager 类

#### 单例获取

```cpp
static StrategyManager& getInstance();
```

#### 初始化方法

```cpp
// 从配置字符串初始化
bool initialize(const std::string& strategies_config);

// 从环境变量初始化
bool initializeFromEnv(const std::string& env_key = "STRATEGIES");
```

#### 查询方法

```cpp
// 获取所有策略
std::vector<StrategyPtr> getStrategies() const;

// 根据名称获取策略
StrategyPtr getStrategy(const std::string& name) const;

// 获取策略名称列表
std::vector<std::string> getStrategyNames() const;

// 检查策略是否存在
bool hasStrategy(const std::string& name) const;

// 获取策略数量
size_t count() const;
```

#### 管理方法

```cpp
// 手动注册策略
bool registerStrategy(StrategyPtr strategy);

// 清除所有策略
void clear();
```

#### 分析方法

```cpp
// 使用所有策略分析
std::map<std::string, std::optional<AnalysisResult>> analyzeAll(
    const std::string& tsCode,
    const std::vector<StockData>& data
) const;
```

## 配置示例

### 示例 1: 使用所有策略

```bash
STRATEGIES=EMA17TO25;MA_CROSS;MACD;RSI;BOLL;GRID
```

### 示例 2: 只使用技术指标策略

```bash
STRATEGIES=MACD;RSI;BOLL
```

### 示例 3: 只使用均线策略

```bash
STRATEGIES=EMA17TO25;MA_CROSS
```

### 示例 4: 单个策略

```bash
STRATEGIES=MACD
```

## 完整示例

```cpp
#include <iostream>
#include "Core.h"
#include "Config.h"
#include "Data.h"
#include "Logger.h"

int main() {
    // 1. 初始化日志系统
    logger::init();
    
    // 2. 初始化配置系统
    auto& config = config::Config::getInstance();
    if (!config.initialize(".env")) {
        LOG_ERROR("配置初始化失败");
        return 1;
    }
    
    LOG_INFO("配置的策略: " + config.getStrategies());
    
    // 3. 初始化数据库
    auto& conn = data::Connection::getInstance();
    conn.initialize("stock.db");
    conn.connect();
    
    // 4. 初始化策略管理器
    auto& manager = core::StrategyManager::getInstance();
    if (!manager.initializeFromEnv("STRATEGIES")) {
        LOG_ERROR("策略管理器初始化失败");
        return 1;
    }
    
    LOG_INFO("已注册 " + std::to_string(manager.count()) + " 个策略");
    
    // 5. 创建 DAO
    data::AnalysisResultDAO analysisDao;
    data::StockDAO stockDao;
    
    // 6. 获取股票列表
    auto stocks = stockDao.findAll();
    LOG_INFO("找到 " + std::to_string(stocks.size()) + " 只股票");
    
    // 7. 批量分析
    int totalResults = 0;
    int savedResults = 0;
    
    for (const auto& stock : stocks) {
        // 获取股票数据
        auto data = getStockData(stock.ts_code);
        
        if (data.empty()) {
            LOG_WARN("股票 " + stock.ts_code + " 数据为空");
            continue;
        }
        
        // 使用所有策略分析
        auto results = manager.analyzeAll(stock.ts_code, data);
        
        // 保存结果
        for (const auto& pair : results) {
            totalResults++;
            
            if (pair.second.has_value()) {
                if (analysisDao.insert(*pair.second)) {
                    savedResults++;
                }
            }
        }
        
        LOG_INFO("完成分析: " + stock.ts_code + " (" + stock.name + ")");
    }
    
    LOG_INFO("分析完成: " + std::to_string(savedResults) + "/" + 
             std::to_string(totalResults) + " 条结果已保存");
    
    // 8. 查询买入信号
    auto buySignals = analysisDao.findByLabel("买入");
    LOG_INFO("发现 " + std::to_string(buySignals.size()) + " 个买入信号");
    
    for (const auto& signal : buySignals) {
        LOG_INFO("买入信号: " + signal.toString());
    }
    
    // 9. 清理
    conn.disconnect();
    logger::LoggerManager::getInstance().shutdown();
    
    return 0;
}
```

## 最佳实践

### 1. 配置验证

在启动时验证配置的策略是否有效：

```cpp
auto& manager = core::StrategyManager::getInstance();
if (!manager.initializeFromEnv("STRATEGIES")) {
    LOG_ERROR("策略配置无效，请检查 .env 文件");
    return 1;
}

if (manager.count() == 0) {
    LOG_WARN("没有配置任何策略");
}
```

### 2. 动态切换策略

可以在运行时动态切换策略配置：

```cpp
// 清除现有策略
manager.clear();

// 加载新的策略配置
manager.initialize("MACD;RSI");
```

### 3. 手动注册自定义策略

```cpp
// 创建自定义策略
auto customStrategy = std::make_shared<MyCustomStrategy>();

// 注册到管理器
manager.registerStrategy(customStrategy);
```

### 4. 批量处理优化

使用批量插入提高性能：

```cpp
std::vector<core::AnalysisResult> allResults;

for (const auto& stock : stocks) {
    auto data = getStockData(stock.ts_code);
    auto results = manager.analyzeAll(stock.ts_code, data);
    
    for (const auto& pair : results) {
        if (pair.second.has_value()) {
            allResults.push_back(*pair.second);
        }
    }
}

// 批量插入
analysisDao.batchInsert(allResults);
```

## 注意事项

1. **策略名称不区分大小写**：`MACD`、`macd`、`Macd` 都会被识别为同一个策略
2. **分号分隔**：多个策略使用分号（`;`）分隔
3. **空格会被忽略**：`EMA17TO25 ; MACD ; RSI` 和 `EMA17TO25;MACD;RSI` 效果相同
4. **无效策略会被跳过**：如果配置了不存在的策略名称，会记录警告日志但不会中断初始化
5. **单例模式**：`StrategyManager` 是单例，全局只有一个实例

## 文件清单

### 核心文件
- `core/StrategyManager.h` - 策略管理器头文件
- `core/StrategyManager.cpp` - 策略管理器实现
- `config/Config.h` - 配置管理器（已更新）
- `config/Config.cpp` - 配置管理器实现（已更新）

### 示例文件
- `examples/strategy_manager_example.cpp` - 使用示例

### 配置文件
- `.env` - 环境配置文件（添加 `STRATEGIES` 配置项）

## 总结

`StrategyManager` 提供了一个灵活的方式来管理和使用多个策略：

- ✅ 从配置文件动态加载策略
- ✅ 支持批量分析
- ✅ 与数据库无缝集成
- ✅ 单例模式，全局统一管理
- ✅ 支持手动注册自定义策略
- ✅ 简单易用的 API

通过配置文件管理策略，可以在不修改代码的情况下灵活调整分析策略组合。

