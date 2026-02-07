# AnalysisResult 数据库集成文档

## 概述

`AnalysisResult` 是一个简化的分析结果数据结构，用于存储策略分析的输出。它已经完全集成到数据库系统中，可以方便地进行持久化存储和查询。

**重要更新**：`analyze()` 方法现在返回 `std::optional<AnalysisResult>`，当数据不足或分析失败时返回 `std::nullopt`。

## 数据结构

### 核心结构：AnalysisResult

```cpp
struct AnalysisResult {
    std::string ts_code;        // 股票代码（如 000001.SZ）
    std::string strategy_name;  // 策略名称（如 MA_CROSS）
    std::string trade_date;     // 分析日期（格式：YYYYMMDD）
    std::string label;          // 标签（如：买入、卖出、持有）
};
```

### 数据库实体：AnalysisResultEntity

```cpp
struct AnalysisResultEntity {
    int id = 0;                     // 数据库主键
    AnalysisResult result;          // 核心分析结果
    std::string created_at;         // 创建时间
    std::string updated_at;         // 更新时间
};
```

## 数据库表结构

```sql
CREATE TABLE IF NOT EXISTS analysis_results (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    ts_code TEXT NOT NULL,
    strategy_name TEXT NOT NULL,
    trade_date TEXT NOT NULL,
    label TEXT NOT NULL,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    UNIQUE(ts_code, strategy_name, trade_date)
);

-- 索引
CREATE INDEX idx_analysis_results_ts_code ON analysis_results(ts_code);
CREATE INDEX idx_analysis_results_strategy ON analysis_results(strategy_name);
CREATE INDEX idx_analysis_results_date ON analysis_results(trade_date);
CREATE INDEX idx_analysis_results_label ON analysis_results(label);
```

## AnalysisResultDAO API

### 基本操作

#### 1. 插入单条记录

```cpp
data::AnalysisResultDAO dao;
core::AnalysisResult result("000001.SZ", "MA_CROSS", "20240101", "买入");

if (dao.insert(result)) {
    std::cout << "插入成功" << std::endl;
}
```

#### 2. 批量插入

```cpp
std::vector<core::AnalysisResult> results = {
    {"000001.SZ", "MA_CROSS", "20240101", "买入"},
    {"000002.SZ", "RSI", "20240101", "持有"},
    {"000003.SZ", "MACD", "20240102", "卖出"}
};

int count = dao.batchInsert(results);
std::cout << "成功插入 " << count << " 条记录" << std::endl;
```

### 查询操作

#### 1. 查询所有记录

```cpp
auto allResults = dao.findAll();
for (const auto& r : allResults) {
    std::cout << r.toString() << std::endl;
}
```

#### 2. 根据 ID 查询

```cpp
auto result = dao.findById(1);
if (result.has_value()) {
    std::cout << result->toString() << std::endl;
}
```

#### 3. 根据股票代码查询

```cpp
auto results = dao.findByTsCode("000001.SZ");
// 返回该股票的所有分析结果，按日期降序排列
```

#### 4. 根据策略名称查询

```cpp
auto results = dao.findByStrategy("MA_CROSS");
// 返回该策略的所有分析结果
```

#### 5. 根据标签查询

```cpp
auto buyResults = dao.findByLabel("买入");
// 返回所有标签为"买入"的分析结果
```

#### 6. 根据股票代码和策略查询

```cpp
auto results = dao.findByTsCodeAndStrategy("000001.SZ", "MA_CROSS");
// 返回特定股票使用特定策略的所有分析结果
```

#### 7. 根据日期范围查询

```cpp
auto results = dao.findByDateRange("20240101", "20240131");
// 返回指定日期范围内的所有分析结果
```

### 更新和删除操作

#### 1. 更新记录

```cpp
core::AnalysisResult newResult("000001.SZ", "MA_CROSS", "20240101", "卖出");
if (dao.update(1, newResult)) {
    std::cout << "更新成功" << std::endl;
}
```

#### 2. 删除单条记录

```cpp
if (dao.remove(1)) {
    std::cout << "删除成功" << std::endl;
}
```

#### 3. 删除股票的所有记录

```cpp
if (dao.removeByTsCode("000001.SZ")) {
    std::cout << "删除股票所有记录成功" << std::endl;
}
```

#### 4. 删除策略的所有记录

```cpp
if (dao.removeByStrategy("MA_CROSS")) {
    std::cout << "删除策略所有记录成功" << std::endl;
}
```

### 统计操作

#### 1. 统计总记录数

```cpp
int total = dao.count();
std::cout << "总记录数: " << total << std::endl;
```

#### 2. 统计股票的记录数

```cpp
int count = dao.countByTsCode("000001.SZ");
std::cout << "000001.SZ 的记录数: " << count << std::endl;
```

## 在策略中使用

### 基本使用示例

```cpp
#include "Core.h"

// 创建策略
core::EMA17TO25Strategy strategy;

// 分析股票数据
auto result = strategy.analyze("000001.SZ", stockData);

// 检查结果是否有效
if (result.has_value()) {
    std::cout << "分析成功: " << result->toString() << std::endl;
    // 可以访问结果字段
    std::cout << "标签: " << result->label << std::endl;
} else {
    std::cout << "分析失败: 数据不足" << std::endl;
}
```

### 完整示例

```cpp
#include "Data.h"
#include "Core.h"
#include "Logger.h"

int main() {
    // 1. 初始化日志和数据库
    logger::init();
    
    auto& conn = data::Connection::getInstance();
    conn.initialize("stock.db");
    conn.connect();
    
    // 2. 创建 DAO
    data::AnalysisResultDAO analysisDao;
    data::StockDAO stockDao;
    
    // 3. 获取股票列表
    auto stocks = stockDao.findAll();
    
    // 4. 创建策略
    core::EMA17TO25Strategy emaStrategy;
    core::MACrossStrategy maStrategy;
    core::RSIStrategy rsiStrategy;
    
    // 5. 对每只股票进行分析
    for (const auto& stock : stocks) {
        // 获取股票数据（从数据源）
        auto data = getStockData(stock.ts_code);
        
        // 使用多个策略分析
        auto emaResult = emaStrategy.analyze(stock.ts_code, data);
        auto maResult = maStrategy.analyze(stock.ts_code, data);
        auto rsiResult = rsiStrategy.analyze(stock.ts_code, data);
        
        // 保存分析结果（只保存有效结果）
        if (emaResult.has_value()) {
            analysisDao.insert(*emaResult);
        }
        if (maResult.has_value()) {
            analysisDao.insert(*maResult);
        }
        if (rsiResult.has_value()) {
            analysisDao.insert(*rsiResult);
        }
        
        LOG_INFO("完成股票分析: " + stock.ts_code);
    }
    
    // 6. 查询买入信号
    auto buySignals = analysisDao.findByLabel("买入");
    LOG_INFO("发现 " + std::to_string(buySignals.size()) + " 个买入信号");
    
    for (const auto& signal : buySignals) {
        LOG_INFO("买入信号: " + signal.toString());
    }
    
    // 7. 断开连接
    conn.disconnect();
    
    return 0;
}
```

## 最佳实践

### 1. 检查分析结果

始终检查 `analyze()` 返回的 `std::optional` 是否有值：

```cpp
auto result = strategy.analyze(tsCode, data);

if (result.has_value()) {
    // 分析成功，使用结果
    dao.insert(*result);
    LOG_INFO("分析成功: " + result->toString());
} else {
    // 分析失败（通常是数据不足）
    LOG_WARN("分析失败: " + tsCode + " 数据不足");
}
```

### 2. 批量操作

当需要插入大量记录时，使用批量插入可以提高性能：

```cpp
std::vector<core::AnalysisResult> results;

for (const auto& stock : stocks) {
    auto data = getStockData(stock.ts_code);
    auto result = strategy.analyze(stock.ts_code, data);
    
    // 只添加有效结果
    if (result.has_value()) {
        results.push_back(*result);
    }
}

// 批量插入
dao.batchInsert(results);
```

### 3. 事务处理

对于关键操作，使用事务确保数据一致性：

```cpp
auto& conn = data::Connection::getInstance();

conn.beginTransaction();
try {
    auto result1 = strategy1.analyze(tsCode, data);
    auto result2 = strategy2.analyze(tsCode, data);
    auto result3 = strategy3.analyze(tsCode, data);
    
    if (result1.has_value()) dao.insert(*result1);
    if (result2.has_value()) dao.insert(*result2);
    if (result3.has_value()) dao.insert(*result3);
    
    conn.commit();
} catch (const std::exception& e) {
    conn.rollback();
    LOG_ERROR("事务失败: " + std::string(e.what()));
}
```

### 4. 定期清理

定期清理旧数据，避免数据库过大：

```cpp
// 删除30天前的数据
std::string oldDate = "20231201";
auto oldResults = dao.findByDateRange("20000101", oldDate);
for (const auto& r : oldResults) {
    // 根据需要删除
}
```

### 5. 查询优化

利用索引进行高效查询：

```cpp
// 好的做法：使用索引字段查询
auto results = dao.findByTsCode("000001.SZ");

// 避免：全表扫描
auto allResults = dao.findAll();
// 然后在内存中过滤
```

## 注意事项

1. **返回值检查**：`analyze()` 返回 `std::optional<AnalysisResult>`，必须检查 `has_value()` 后才能使用
2. **数据不足处理**：当数据不足时，`analyze()` 返回 `std::nullopt`，不会抛出异常
3. **唯一约束**：同一股票、同一策略、同一日期的记录是唯一的
4. **日期格式**：trade_date 必须使用 YYYYMMDD 格式
5. **标签规范**：建议使用统一的标签（买入、卖出、持有）
6. **策略名称**：使用策略的 getName() 方法获取标准名称

## 文件清单

### 核心文件
- `core/AnalysisResult.h` - 分析结果数据结构
- `core/AnalysisResult.cpp` - 分析结果实现

### 数据库文件
- `data/database/AnalysisResultTable.h` - 表结构定义
- `data/database/AnalysisResultDAO.h` - DAO 接口
- `data/database/AnalysisResultDAO.cpp` - DAO 实现
- `data/database/Connection.cpp` - 包含表创建语句

### 示例文件
- `examples/analysis_result_dao_example.cpp` - 使用示例

## 总结

`AnalysisResult` 提供了一个简洁、高效的方式来存储和查询策略分析结果。通过 `AnalysisResultDAO`，你可以轻松地：

- ✅ 保存策略分析结果到数据库
- ✅ 按多种条件查询历史分析结果
- ✅ 统计和分析交易信号
- ✅ 支持批量操作提高性能
- ✅ 完整的 CRUD 操作支持

这个设计遵循了与 `Stock` 相同的模式，保持了代码的一致性和可维护性。

