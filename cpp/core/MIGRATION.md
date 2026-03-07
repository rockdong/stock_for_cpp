# Core 模块迁移指南

## 📋 迁移概述

本文档说明如何将现有代码迁移到使用统一的 Core 模块数据结构。

## 🎯 迁移目标

将分散在各个模块中的数据结构定义统一到 Core 模块，实现：
- ✅ 单一数据源
- ✅ 类型统一
- ✅ 减少转换
- ✅ 易于维护

## 🔄 迁移步骤

### 步骤 1: 更新 Network 模块

#### 1.1 修改 `network/IDataSource.h`

**之前**：
```cpp
namespace network {

struct StockBasic {
    std::string ts_code;
    std::string symbol;
    std::string name;
    // ... 其他字段
};

struct StockData {
    std::string ts_code;
    std::string trade_date;
    double open;
    // ... 其他字段
};

class IDataSource {
public:
    virtual std::vector<StockBasic> getStockList() = 0;
    virtual std::vector<StockData> getDailyData(...) = 0;
};

} // namespace network
```

**之后**：
```cpp
#include "../core/Stock.h"

namespace network {

// 使用核心模块的数据结构
using Stock = core::Stock;
using StockData = core::StockData;

class IDataSource {
public:
    virtual std::vector<Stock> getStockList() = 0;
    virtual std::vector<StockData> getDailyData(...) = 0;
};

} // namespace network
```

#### 1.2 修改 `network/TushareDataSource.h`

**需要修改的地方**：
- 将 `StockBasic` 改为 `Stock`
- 返回类型从 `std::vector<StockBasic>` 改为 `std::vector<Stock>`

**之前**：
```cpp
class TushareDataSource : public IDataSource {
public:
    std::vector<StockBasic> getStockList() override;
};
```

**之后**：
```cpp
class TushareDataSource : public IDataSource {
public:
    std::vector<Stock> getStockList() override;
};
```

#### 1.3 修改 `network/TushareDataSource.cpp`

**需要修改的地方**：
- 将 `StockBasic` 改为 `Stock`
- JSON 解析代码保持不变（字段名相同）

**之前**：
```cpp
std::vector<StockBasic> TushareDataSource::getStockList() {
    std::vector<StockBasic> stocks;
    // ... 解析代码
    StockBasic stock;
    stock.ts_code = item[0];
    stock.symbol = item[1];
    // ...
    stocks.push_back(stock);
    return stocks;
}
```

**之后**：
```cpp
std::vector<Stock> TushareDataSource::getStockList() {
    std::vector<Stock> stocks;
    // ... 解析代码
    Stock stock;
    stock.ts_code = item[0];
    stock.symbol = item[1];
    // ...
    stocks.push_back(stock);
    return stocks;
}
```

### 步骤 2: 更新 Data 模块

#### 2.1 修改 `data/database/StockDAO.h`

**之前**：
```cpp
namespace data {

struct Stock {
    int id = 0;
    std::string ts_code;
    std::string symbol;
    std::string name;
    // ... 其他字段
    std::string created_at;
    std::string updated_at;
};

class StockDAO {
public:
    bool insert(const Stock& stock);
    std::vector<Stock> findAll();
};

} // namespace data
```

**之后**：
```cpp
#include "../../core/Stock.h"

namespace data {

// 使用核心模块的数据结构
using Stock = core::Stock;
using StockEntity = core::StockEntity;

class StockDAO {
public:
    bool insert(const Stock& stock);
    std::vector<Stock> findAll();
};

} // namespace data
```

#### 2.2 修改 `data/database/StockDAO.cpp`

**需要注意的地方**：
- 数据库操作中，如果需要 `id`、`created_at`、`updated_at`，使用 `StockEntity`
- 对外接口使用 `Stock`

**示例**：
```cpp
bool StockDAO::insert(const Stock& stock) {
    // 内部可以创建 StockEntity
    StockEntity entity;
    entity.stock = stock;
    entity.created_at = getCurrentTime();
    entity.updated_at = getCurrentTime();
    
    // 插入数据库...
    return true;
}

std::vector<Stock> StockDAO::findAll() {
    std::vector<Stock> stocks;
    
    // 从数据库读取...
    // 只返回业务数据，不返回数据库元信息
    
    return stocks;
}
```

#### 2.3 修改 `data/Data.h` 和 `data/Data.cpp`

**如果有转换函数，需要删除或更新**：

**之前**：
```cpp
// data/Data.h
namespace data {
    Stock convertFromStockBasic(const network::StockBasic& basic);
}

// data/Data.cpp
Stock convertFromStockBasic(const network::StockBasic& basic) {
    Stock stock;
    stock.ts_code = basic.ts_code;
    stock.symbol = basic.symbol;
    // ... 复制所有字段
    return stock;
}
```

**之后**：
```cpp
// 不再需要转换函数，直接删除
// 因为 network::Stock 和 data::Stock 是同一个类型
```

### 步骤 3: 更新 CMakeLists.txt

**添加 core_lib**：

```cmake
# 添加核心模块库（只有头文件）
add_library(core_lib INTERFACE)

target_include_directories(core_lib INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/core
)

# 更新 network_lib
target_link_libraries(network_lib PUBLIC
    httplib::httplib
    nlohmann_json::nlohmann_json
    core_lib  # 添加依赖
    log_lib
    config_lib
)

# 更新 data_lib
target_link_libraries(data_lib PUBLIC
    sqlpp11::sqlpp11
    sqlite3
    core_lib  # 添加依赖
    log_lib
    config_lib
)
```

### 步骤 4: 更新业务代码

#### 4.1 修改 `main.cpp`

**之前**：
```cpp
#include "Network.h"
#include "Data.h"

int main() {
    auto dataSource = network::DataSourceFactory::createFromConfig();
    auto networkStocks = dataSource->getStockList();
    
    // 需要转换
    data::StockDAO stockDao;
    for (const auto& networkStock : networkStocks) {
        auto dataStock = data::convertFromStockBasic(networkStock);
        stockDao.insert(dataStock);
    }
    
    return 0;
}
```

**之后**：
```cpp
#include "Core.h"
#include "Network.h"
#include "Data.h"

int main() {
    auto dataSource = network::DataSourceFactory::createFromConfig();
    auto stocks = dataSource->getStockList();  // 返回 std::vector<core::Stock>
    
    // 无需转换，直接使用
    data::StockDAO stockDao;
    for (const auto& stock : stocks) {
        stockDao.insert(stock);  // 直接插入
    }
    
    return 0;
}
```

## 🔍 常见问题

### Q1: 为什么要创建 StockEntity？

**A**: `StockEntity` 用于数据库持久化，包含数据库特定字段（id, created_at, updated_at）。而 `Stock` 是纯业务数据，不包含数据库元信息。

**使用场景**：
- 对外接口：使用 `Stock`
- 数据库内部：使用 `StockEntity`

```cpp
// 对外接口
bool insert(const Stock& stock);
std::vector<Stock> findAll();

// 内部实现
bool insert(const Stock& stock) {
    StockEntity entity;
    entity.stock = stock;
    entity.id = generateId();
    entity.created_at = getCurrentTime();
    // 插入数据库...
}
```

### Q2: 如何处理数据库查询返回的 id 字段？

**A**: 如果业务逻辑需要 id，可以返回 `std::pair<int, Stock>` 或创建专门的结果类型。

```cpp
// 方案 1: 使用 pair
std::vector<std::pair<int, Stock>> findAllWithId();

// 方案 2: 使用 StockEntity
std::vector<StockEntity> findAllEntities();

// 方案 3: 只返回业务数据（推荐）
std::vector<Stock> findAll();  // 不返回 id
```

### Q3: 旧代码中的 StockBasic 怎么办？

**A**: 使用 using 声明保持兼容性：

```cpp
namespace network {
    using Stock = core::Stock;
    using StockBasic = Stock;  // 兼容旧代码
}
```

### Q4: 如何处理不同模块的特殊需求？

**A**: 通过组合而不是修改核心结构：

```cpp
// ✅ 正确：组合
namespace data {
    struct StockWithCache {
        core::Stock stock;
        std::time_t cache_time;
    };
}

// ❌ 错误：修改核心结构
namespace core {
    struct Stock {
        // ... 业务字段
        std::time_t cache_time;  // 错误！缓存信息不应该在核心结构中
    };
}
```

## ✅ 迁移检查清单

### Network 模块
- [ ] 删除 `network::StockBasic` 定义
- [ ] 删除 `network::StockData` 定义
- [ ] 添加 `#include "../core/Stock.h"`
- [ ] 添加 `using Stock = core::Stock;`
- [ ] 添加 `using StockData = core::StockData;`
- [ ] 更新所有函数签名
- [ ] 更新实现代码
- [ ] 编译测试

### Data 模块
- [ ] 删除 `data::Stock` 定义
- [ ] 添加 `#include "../../core/Stock.h"`
- [ ] 添加 `using Stock = core::Stock;`
- [ ] 添加 `using StockEntity = core::StockEntity;`
- [ ] 删除转换函数
- [ ] 更新所有函数签名
- [ ] 更新实现代码
- [ ] 编译测试

### CMakeLists.txt
- [ ] 添加 `core_lib` 定义
- [ ] 更新 `network_lib` 依赖
- [ ] 更新 `data_lib` 依赖
- [ ] 更新 include 目录
- [ ] 编译测试

### 业务代码
- [ ] 删除类型转换代码
- [ ] 更新 include 语句
- [ ] 简化数据流
- [ ] 编译测试
- [ ] 功能测试

## 🎯 迁移后的优势

### 1. 代码简化

**之前**：
```cpp
// 需要转换
auto networkStocks = dataSource->getStockList();
std::vector<data::Stock> dataStocks;
for (const auto& ns : networkStocks) {
    dataStocks.push_back(convertToDataStock(ns));
}
```

**之后**：
```cpp
// 直接使用
auto stocks = dataSource->getStockList();
// stocks 可以直接传递给任何模块
```

### 2. 类型安全

**之前**：
```cpp
// 运行时错误风险
data::Stock stock = convertToDataStock(networkStock);
// 如果转换函数有 bug，运行时才能发现
```

**之后**：
```cpp
// 编译期类型检查
core::Stock stock = dataSource->getStockInfo("000001.SZ");
stockDao.insert(stock);  // 编译期保证类型正确
```

### 3. 维护简化

**之前**：
```cpp
// 添加新字段需要修改多处
// 1. network::StockBasic
// 2. data::Stock
// 3. 转换函数
// 4. 所有使用的地方
```

**之后**：
```cpp
// 添加新字段只需修改一处
// 1. core::Stock
// 所有模块自动同步
```

## 📊 迁移进度跟踪

| 模块 | 状态 | 完成日期 | 备注 |
|------|------|----------|------|
| Core 模块创建 | ✅ 完成 | 2026-02-04 | 创建核心数据结构 |
| Network 模块迁移 | ✅ 完成 | 2026-02-04 | 使用 core::Stock |
| Data 模块迁移 | ✅ 完成 | 2026-02-04 | 使用 core::Stock |
| CMakeLists 更新 | ✅ 完成 | 2026-02-04 | 添加 core_lib |
| 编译测试 | ⏳ 待测试 | - | 需要编译验证 |
| 功能测试 | ⏳ 待测试 | - | 需要功能验证 |

## 🚀 下一步

1. **编译测试**：确保所有模块编译通过
2. **功能测试**：验证数据流正确
3. **文档更新**：更新相关文档
4. **代码审查**：团队审查迁移代码

---

**版本**: 1.0.0  
**创建日期**: 2026-02-04  
**最后更新**: 2026-02-04  
**维护者**: Development Team

