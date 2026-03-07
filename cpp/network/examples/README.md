# 网络层模块 - 使用示例

本目录包含网络层模块的使用示例代码。

## 📋 示例列表

### network_example.cpp

完整的网络层使用示例，包含5个场景：

1. **基础使用** - 创建数据源、测试连接、获取股票列表
2. **获取日线数据** - 获取指定股票的历史行情数据
3. **获取最新行情** - 批量获取多只股票的最新行情
4. **直接调用 API** - 使用 TushareClient 直接调用 API
5. **HTTP 客户端** - 使用底层 HttpClient 发送请求

## 🚀 编译运行

### 方式1: 使用 CMake（推荐）

```bash
# 在项目根目录
mkdir build && cd build
cmake ..
make

# 运行示例
./network_example
```

### 方式2: 手动编译

```bash
g++ -std=c++17 \
    network_example.cpp \
    ../http/HttpClient.cpp \
    ../http/TushareClient.cpp \
    ../TushareDataSource.cpp \
    ../DataSourceFactory.cpp \
    ../../log/*.cpp \
    ../../config/Config.cpp \
    -I.. -I../http -I../../log -I../../config \
    -I../../thirdparty/cpp-httplib \
    -I../../thirdparty/json/include \
    -I../../thirdparty/spdlog/include \
    -I../../thirdparty/dotenv-cpp/include \
    -lspdlog -lpthread \
    -o network_example

./network_example
```

## ⚙️ 配置要求

运行示例前，请确保：

1. 已在 `.env` 文件中配置 Tushare API Token：
   ```env
   DATA_SOURCE_API_KEY=your_tushare_token_here
   ```

2. 已配置其他必要参数：
   ```env
   DATA_SOURCE_URL=http://api.waditu.com
   DATA_SOURCE_TIMEOUT=30
   DATA_SOURCE_RETRY_TIMES=3
   ```

## 📖 示例说明

### 示例1: 基础使用

```cpp
// 创建数据源（从配置文件）
auto source = network::DataSourceFactory::createFromConfig();

// 测试连接
if (source->testConnection()) {
    LOG_INFO("连接成功");
}

// 获取股票列表
auto stocks = source->getStockList();
```

### 示例2: 获取日线数据

```cpp
// 获取平安银行2024年1月的数据
auto data = source->getDailyData("000001.SZ", "20240101", "20240131");

for (const auto& d : data) {
    std::cout << d.trade_date << " " << d.close << std::endl;
}
```

### 示例3: 获取最新行情

```cpp
// 获取单只股票最新行情
auto quote = source->getLatestQuote("000001.SZ");
std::cout << "最新价: " << quote.close << std::endl;
```

### 示例4: 直接调用 API

```cpp
// 创建 Tushare 客户端
network::TushareClient client(token);

// 获取交易日历
auto response = client.getTradeCal("20240101", "20240131", "SSE");

if (response.isSuccess()) {
    // 处理数据...
}
```

### 示例5: HTTP 客户端

```cpp
// 创建 HTTP 客户端
network::HttpClient client("http://api.waditu.com", 30);

// 发送 POST 请求
auto response = client.post("/", json_body, "application/json");
```

## 📊 预期输出

```
========== 示例1: 基础使用 ==========

✅ Tushare 连接成功
获取到 5000+ 只股票

前10只股票信息：
000001.SZ | 平安银行 | 银行 | 深圳
000002.SZ | 万科A | 房地产 | 深圳
...

========== 示例2: 获取日线数据 ==========

获取到 20 条数据

平安银行(000001.SZ) 2024年1月行情：
日期       | 开盘   | 最高   | 最低   | 收盘   | 涨跌幅  | 成交量(手)
-----------|--------|--------|--------|--------|---------|------------
20240102 | 10.50 | 10.65 | 10.45 | 10.60 | +1.92% | 1234567
...
```

## 🐛 常见问题

### 1. 编译错误

**问题**: 找不到头文件

**解决**: 确保包含路径正确，检查第三方库是否已安装

### 2. 运行时错误

**问题**: API Token 未配置

**解决**: 在 `.env` 文件中配置 `DATA_SOURCE_API_KEY`

**问题**: 连接超时

**解决**: 检查网络连接，增加 `DATA_SOURCE_TIMEOUT` 值

### 3. API 错误

**问题**: 返回错误码

**解决**: 
- 检查 Token 是否有效
- 检查 API 调用频率是否超限
- 查看 Tushare 官方文档确认参数格式

## 📚 更多资源

- [网络层 README](../README.md)
- [Tushare Pro 文档](https://tushare.pro/document/1?doc_id=130)
- [项目设计文档](../../DESIGN.md)

---

**提示**: 首次运行建议先执行示例1测试连接，确保配置正确后再运行其他示例。

