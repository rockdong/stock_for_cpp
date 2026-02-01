# Tushare 行情数据接口更新说明

## 更新内容

为 Tushare 数据源添加了周线和月线行情数据接口，支持通过统一的 API 获取日线、周线、月线数据。

## 新增接口

### 1. TushareClient 层

在 `network/http/TushareClient.h` 和 `TushareClient.cpp` 中新增：

```cpp
// 获取周线行情
TushareResponse getWeeklyQuote(
    const std::string& ts_code = "",
    const std::string& trade_date = "",
    const std::string& start_date = "",
    const std::string& end_date = ""
);

// 获取月线行情
TushareResponse getMonthlyQuote(
    const std::string& ts_code = "",
    const std::string& trade_date = "",
    const std::string& start_date = "",
    const std::string& end_date = ""
);
```

### 2. IDataSource 接口层

在 `network/IDataSource.h` 中新增：

```cpp
// 统一的行情数据获取接口
virtual std::vector<StockData> getQuoteData(
    const std::string& ts_code,
    const std::string& start_date,
    const std::string& end_date,
    const std::string& freq = "d"  // d=日线, w=周线, m=月线
) = 0;
```

### 3. TushareDataSource 实现层

在 `network/TushareDataSource.h` 和 `TushareDataSource.cpp` 中实现：

```cpp
std::vector<StockData> getQuoteData(
    const std::string& ts_code,
    const std::string& start_date,
    const std::string& end_date,
    const std::string& freq = "d"
) override;
```

## 使用方式

### 简单示例

```cpp
auto dataSource = network::DataSourceFactory::createFromConfig();

// 获取日线数据
auto daily = dataSource->getQuoteData("000001.SZ", "20240101", "20240131", "d");

// 获取周线数据
auto weekly = dataSource->getQuoteData("000001.SZ", "20240101", "20240331", "w");

// 获取月线数据
auto monthly = dataSource->getQuoteData("000001.SZ", "20230101", "20231231", "m");
```

## 频率参数说明

| 参数 | 说明 | Tushare API |
|------|------|-------------|
| `d` 或 `D` | 日线数据 | `daily` |
| `w` 或 `W` | 周线数据 | `weekly` |
| `m` 或 `M` | 月线数据 | `monthly` |

## 技术特点

1. **统一接口**: 通过 `freq` 参数统一日线、周线、月线的调用方式
2. **大小写不敏感**: 频率参数支持大小写（d/D, w/W, m/M）
3. **类型安全**: 使用 C++ 强类型系统，编译时检查
4. **错误处理**: 完善的日志记录和异常处理
5. **向后兼容**: 保留原有的 `getDailyData()` 方法

## 文件修改清单

### 新增文件
- `docs/API_USAGE.md` - API 使用文档

### 修改文件
- `network/http/TushareClient.h` - 添加周线和月线接口声明
- `network/http/TushareClient.cpp` - 实现周线和月线接口
- `network/IDataSource.h` - 添加统一的 `getQuoteData()` 接口
- `network/TushareDataSource.h` - 添加 `getQuoteData()` 声明
- `network/TushareDataSource.cpp` - 实现 `getQuoteData()` 方法
- `main.cpp` - 添加测试代码

## 测试

编译并运行：

```bash
cd /Users/workspace/Documents/CppProjects/stock_for_cpp
cmake --build cmake-build-debug --target stock_for_cpp
./cmake-build-debug/stock_for_cpp
```

## 参考文档

- [Tushare Pro 日线行情 API](https://tushare.pro/document/2?doc_id=27)
- [Tushare Pro 周线行情 API](https://tushare.pro/document/2?doc_id=144)
- [Tushare Pro 月线行情 API](https://tushare.pro/document/2?doc_id=145)

## 后续计划

可以继续扩展以下功能：
- 分钟级行情数据（1分钟、5分钟、15分钟、30分钟、60分钟）
- 复权行情数据
- 实时行情数据
- 指数行情数据

