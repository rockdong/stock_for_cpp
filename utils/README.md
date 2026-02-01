# Utils - 工具类模块

## 📋 模块概述

工具类模块提供了一系列常用的工具函数，包括时间处理、字符串操作、数学计算等功能，为整个股票分析系统提供基础支持。

## 🎯 核心功能

### 1. TimeUtil - 时间工具类

提供日期时间的格式化、解析、计算等功能。

**主要功能**：
- ✅ 日期时间格式化
- ✅ 字符串解析为日期
- ✅ 日期计算（加减天数、月数、年数）
- ✅ 日期范围生成
- ✅ 工作日/周末判断
- ✅ 时间戳转换

### 2. StringUtil - 字符串工具类

提供字符串的修剪、分割、连接、转换等功能。

**主要功能**：
- ✅ 字符串修剪（trim、ltrim、rtrim）
- ✅ 字符串分割与连接
- ✅ 大小写转换
- ✅ 查找与替换
- ✅ 类型转换（字符串 ↔ 数值）
- ✅ 字符串判断（数字、字母等）
- ✅ 字符串格式化（填充、重复、截断等）

### 3. MathUtil - 数学工具类

提供统计计算、金融计算等数学功能。

**主要功能**：
- ✅ 基础统计（均值、中位数、方差、标准差等）
- ✅ 相关性分析（协方差、相关系数）
- ✅ 金融计算（收益率、夏普比率、最大回撤等）
- ✅ 数值处理（四舍五入、限制范围等）
- ✅ 数组操作（归一化、标准化、差分等）
- ✅ 移动计算（移动平均、移动标准差等）

## 🚀 快速开始

### 基本使用

```cpp
#include "Utils.h"

int main() {
    // 时间工具
    std::string today = utils::TimeUtil::today();
    std::cout << "今天: " << today << std::endl;
    
    // 字符串工具
    std::string trimmed = utils::StringUtil::trim("  hello  ");
    std::cout << "修剪后: " << trimmed << std::endl;
    
    // 数学工具
    std::vector<double> values = {1.0, 2.0, 3.0, 4.0, 5.0};
    double avg = utils::MathUtil::mean(values);
    std::cout << "平均值: " << avg << std::endl;
    
    return 0;
}
```

## 📖 详细使用指南

### TimeUtil - 时间工具类

#### 1. 格式化时间

```cpp
// 获取当前时间
std::string now = utils::TimeUtil::now();  // "2024-01-01 12:30:45"

// 获取当前日期
std::string today = utils::TimeUtil::today();  // "2024-01-01"

// 自定义格式
std::string custom = utils::TimeUtil::now("%Y年%m月%d日");  // "2024年01月01日"

// 时间戳转字符串
time_t timestamp = 1704067200;
std::string dateStr = utils::TimeUtil::timestampToString(timestamp);
```

#### 2. 解析时间

```cpp
// 字符串转时间戳
time_t timestamp = utils::TimeUtil::stringToTimestamp("2024-01-01");

// 解析日期
std::tm tm = utils::TimeUtil::parseDate("2024-01-01");

// 解析日期时间
std::tm tm2 = utils::TimeUtil::parseDateTime("2024-01-01 12:30:45");
```

#### 3. 日期计算

```cpp
// 计算两个日期之间的天数
int days = utils::TimeUtil::daysBetween("2024-01-01", "2024-12-31");  // 365

// 日期加减
std::string nextWeek = utils::TimeUtil::addDays("2024-01-01", 7);  // "2024-01-08"
std::string nextMonth = utils::TimeUtil::addMonths("2024-01-01", 1);  // "2024-02-01"
std::string nextYear = utils::TimeUtil::addYears("2024-01-01", 1);  // "2025-01-01"
```

#### 4. 日期判断

```cpp
// 判断是否为闰年
bool isLeap = utils::TimeUtil::isLeapYear(2024);  // true

// 判断日期是否有效
bool valid = utils::TimeUtil::isValidDate("2024-02-29");  // true

// 判断是否为工作日
bool weekday = utils::TimeUtil::isWeekday("2024-01-01");  // false (周一)

// 判断是否为周末
bool weekend = utils::TimeUtil::isWeekend("2024-01-06");  // true (周六)
```

#### 5. 获取日期信息

```cpp
// 获取星期几（0=周日，1=周一，...，6=周六）
int dayOfWeek = utils::TimeUtil::getDayOfWeek("2024-01-01");  // 1 (周一)

// 获取一年中的第几天
int dayOfYear = utils::TimeUtil::getDayOfYear("2024-01-01");  // 1

// 获取某月的天数
int days = utils::TimeUtil::getDaysInMonth(2024, 2);  // 29
```

#### 6. 日期范围

```cpp
// 生成日期范围
auto dates = utils::TimeUtil::getDateRange("2024-01-01", "2024-01-07");
// ["2024-01-01", "2024-01-02", ..., "2024-01-07"]

// 获取本周的开始和结束日期
std::string weekStart = utils::TimeUtil::getWeekStart();  // 本周一
std::string weekEnd = utils::TimeUtil::getWeekEnd();      // 本周日

// 获取本月的开始和结束日期
std::string monthStart = utils::TimeUtil::getMonthStart();  // 本月1日
std::string monthEnd = utils::TimeUtil::getMonthEnd();      // 本月最后一天

// 获取本年的开始和结束日期
std::string yearStart = utils::TimeUtil::getYearStart();  // 1月1日
std::string yearEnd = utils::TimeUtil::getYearEnd();      // 12月31日
```

### StringUtil - 字符串工具类

#### 1. 字符串修剪

```cpp
// 去除两侧空白
std::string trimmed = utils::StringUtil::trim("  hello  ");  // "hello"

// 去除左侧空白
std::string ltrimmed = utils::StringUtil::ltrim("  hello");  // "hello"

// 去除右侧空白
std::string rtrimmed = utils::StringUtil::rtrim("hello  ");  // "hello"

// 去除指定字符
std::string custom = utils::StringUtil::trim("##hello##", "#");  // "hello"
```

#### 2. 分割与连接

```cpp
// 分割字符串
auto parts = utils::StringUtil::split("a,b,c", ",");
// ["a", "b", "c"]

// 分割并跳过空字符串
auto parts2 = utils::StringUtil::split("a,,b,c", ",", true);
// ["a", "b", "c"]

// 连接字符串
std::string joined = utils::StringUtil::join({"a", "b", "c"}, ",");
// "a,b,c"
```

#### 3. 大小写转换

```cpp
// 转换为大写
std::string upper = utils::StringUtil::toUpper("hello");  // "HELLO"

// 转换为小写
std::string lower = utils::StringUtil::toLower("HELLO");  // "hello"

// 首字母大写
std::string cap = utils::StringUtil::capitalize("hello");  // "Hello"
```

#### 4. 查找与替换

```cpp
// 判断前缀
bool starts = utils::StringUtil::startsWith("hello world", "hello");  // true

// 判断后缀
bool ends = utils::StringUtil::endsWith("hello world", "world");  // true

// 判断包含
bool contains = utils::StringUtil::contains("hello world", "lo wo");  // true

// 替换所有
std::string replaced = utils::StringUtil::replaceAll("hello hello", "hello", "hi");
// "hi hi"

// 替换第一个
std::string replaced2 = utils::StringUtil::replaceFirst("hello hello", "hello", "hi");
// "hi hello"
```

#### 5. 字符串判断

```cpp
// 判断是否为空或只包含空白
bool blank = utils::StringUtil::isBlank("   ");  // true

// 判断是否为数字
bool numeric = utils::StringUtil::isNumeric("123.45");  // true

// 判断是否为整数
bool integer = utils::StringUtil::isInteger("123");  // true

// 判断是否为浮点数
bool floatNum = utils::StringUtil::isFloat("123.45");  // true

// 判断是否只包含字母
bool alpha = utils::StringUtil::isAlpha("hello");  // true

// 判断是否只包含字母和数字
bool alphaNum = utils::StringUtil::isAlphaNumeric("hello123");  // true
```

#### 6. 类型转换

```cpp
// 字符串转数值
int i = utils::StringUtil::toInt("123");  // 123
long l = utils::StringUtil::toLong("123456789");  // 123456789
double d = utils::StringUtil::toDouble("123.45");  // 123.45
bool b = utils::StringUtil::toBool("true");  // true

// 数值转字符串
std::string s1 = utils::StringUtil::toString(123);  // "123"
std::string s2 = utils::StringUtil::toString(123.456, 2);  // "123.46"
std::string s3 = utils::StringUtil::toString(true);  // "true"
```

#### 7. 字符串格式化

```cpp
// 左侧填充
std::string padded = utils::StringUtil::padLeft("123", 5, '0');  // "00123"

// 右侧填充
std::string padded2 = utils::StringUtil::padRight("123", 5, '0');  // "12300"

// 重复字符串
std::string repeated = utils::StringUtil::repeat("ab", 3);  // "ababab"

// 反转字符串
std::string reversed = utils::StringUtil::reverse("hello");  // "olleh"

// 截断字符串
std::string truncated = utils::StringUtil::truncate("hello world", 8);
// "hello..."
```

### MathUtil - 数学工具类

#### 1. 基础统计

```cpp
std::vector<double> values = {1.0, 2.0, 3.0, 4.0, 5.0};

// 平均值
double avg = utils::MathUtil::mean(values);  // 3.0

// 中位数
double med = utils::MathUtil::median(values);  // 3.0

// 方差
double var = utils::MathUtil::variance(values);  // 2.5

// 标准差
double std = utils::MathUtil::stddev(values);  // 1.58...

// 总和
double s = utils::MathUtil::sum(values);  // 15.0

// 最小值和最大值
double minVal = utils::MathUtil::min(values);  // 1.0
double maxVal = utils::MathUtil::max(values);  // 5.0

// 范围
double r = utils::MathUtil::range(values);  // 4.0

// 百分位数
double p50 = utils::MathUtil::percentile(values, 50);  // 3.0 (中位数)
double p95 = utils::MathUtil::percentile(values, 95);  // 4.8
```

#### 2. 相关性分析

```cpp
std::vector<double> x = {1.0, 2.0, 3.0, 4.0, 5.0};
std::vector<double> y = {2.0, 4.0, 6.0, 8.0, 10.0};

// 协方差
double cov = utils::MathUtil::covariance(x, y);  // 5.0

// 相关系数
double corr = utils::MathUtil::correlation(x, y);  // 1.0 (完全正相关)
```

#### 3. 金融计算

```cpp
// 收益率
double ret = utils::MathUtil::returnRate(100.0, 110.0);  // 10.0%

// 对数收益率
double logRet = utils::MathUtil::logReturn(100.0, 110.0);  // 0.0953...

// 年化收益率
double annRet = utils::MathUtil::annualizedReturn(0.5, 3);  // 0.1447... (14.47%)

// 夏普比率
std::vector<double> returns = {0.01, 0.02, -0.01, 0.03, 0.01};
double sharpe = utils::MathUtil::sharpeRatio(returns, 0.0);

// 最大回撤
std::vector<double> prices = {100, 110, 105, 120, 115, 130};
double maxDD = utils::MathUtil::maxDrawdown(prices);  // 4.55% (从110跌到105)

// 波动率（年化）
double vol = utils::MathUtil::volatility(returns, 252);

// 复合年增长率（CAGR）
double cagr = utils::MathUtil::cagr(100.0, 150.0, 3);  // 14.47%
```

#### 4. 数值处理

```cpp
// 四舍五入
double rounded = utils::MathUtil::round(123.456, 2);  // 123.46

// 向上取整
double ceiled = utils::MathUtil::ceil(123.451, 2);  // 123.46

// 向下取整
double floored = utils::MathUtil::floor(123.459, 2);  // 123.45

// 限制范围
double clamped = utils::MathUtil::clamp(150, 0, 100);  // 100

// 判断相等（考虑精度）
bool eq = utils::MathUtil::equals(0.1 + 0.2, 0.3);  // true

// 判断是否为零
bool zero = utils::MathUtil::isZero(0.0000001);  // true
```

#### 5. 数组操作

```cpp
std::vector<double> values = {1.0, 2.0, 3.0, 4.0, 5.0};

// 归一化（Min-Max）
auto normalized = utils::MathUtil::normalize(values, 0.0, 1.0);
// [0.0, 0.25, 0.5, 0.75, 1.0]

// 标准化（Z-score）
auto standardized = utils::MathUtil::standardize(values);
// [-1.41, -0.71, 0.0, 0.71, 1.41]

// 累积和
auto cumsum = utils::MathUtil::cumsum(values);
// [1.0, 3.0, 6.0, 10.0, 15.0]

// 差分
auto diff = utils::MathUtil::diff(values);
// [1.0, 1.0, 1.0, 1.0]

// 百分比变化
auto pctChange = utils::MathUtil::pctChange(values);
// [100.0, 50.0, 33.33, 25.0]
```

#### 6. 移动计算

```cpp
std::vector<double> values = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0};

// 移动平均
auto ma = utils::MathUtil::movingAverage(values, 3);
// [2.0, 3.0, 4.0, 5.0, 6.0]

// 移动标准差
auto mstd = utils::MathUtil::movingStddev(values, 3);

// 移动最大值
auto mmax = utils::MathUtil::movingMax(values, 3);
// [3.0, 4.0, 5.0, 6.0, 7.0]

// 移动最小值
auto mmin = utils::MathUtil::movingMin(values, 3);
// [1.0, 2.0, 3.0, 4.0, 5.0]
```

## 🎨 实际应用示例

### 示例1：股票数据处理

```cpp
#include "Utils.h"
#include <iostream>

int main() {
    // 生成交易日期范围
    auto dates = utils::TimeUtil::getDateRange("2024-01-01", "2024-01-31");
    
    // 过滤出工作日
    std::vector<std::string> tradingDays;
    for (const auto& date : dates) {
        if (utils::TimeUtil::isWeekday(date)) {
            tradingDays.push_back(date);
        }
    }
    
    std::cout << "交易日数量: " << tradingDays.size() << std::endl;
    
    return 0;
}
```

### 示例2：收益率分析

```cpp
#include "Utils.h"
#include <iostream>

int main() {
    // 股票价格序列
    std::vector<double> prices = {100.0, 102.0, 101.0, 105.0, 103.0, 108.0};
    
    // 计算收益率
    auto returns = utils::MathUtil::pctChange(prices);
    
    // 统计分析
    double avgReturn = utils::MathUtil::mean(returns);
    double volatility = utils::MathUtil::stddev(returns);
    double maxDD = utils::MathUtil::maxDrawdown(prices);
    
    std::cout << "平均收益率: " << utils::StringUtil::toString(avgReturn, 2) << "%" << std::endl;
    std::cout << "波动率: " << utils::StringUtil::toString(volatility, 2) << "%" << std::endl;
    std::cout << "最大回撤: " << utils::StringUtil::toString(maxDD, 2) << "%" << std::endl;
    
    return 0;
}
```

### 示例3：数据清洗

```cpp
#include "Utils.h"
#include <iostream>

int main() {
    // CSV 数据行
    std::string line = "  000001.SZ , 平安银行 , 12.34 , 1000000  ";
    
    // 分割并清理
    auto parts = utils::StringUtil::split(line, ",");
    for (auto& part : parts) {
        part = utils::StringUtil::trim(part);
    }
    
    // 提取数据
    std::string symbol = parts[0];
    std::string name = parts[1];
    double price = utils::StringUtil::toDouble(parts[2]);
    long volume = utils::StringUtil::toLong(parts[3]);
    
    std::cout << "股票代码: " << symbol << std::endl;
    std::cout << "股票名称: " << name << std::endl;
    std::cout << "价格: " << price << std::endl;
    std::cout << "成交量: " << volume << std::endl;
    
    return 0;
}
```

## 📊 API 参考

### TimeUtil API

| 方法 | 说明 | 返回值 |
|------|------|--------|
| `now()` | 获取当前时间字符串 | string |
| `today()` | 获取当前日期字符串 | string |
| `daysBetween()` | 计算两个日期之间的天数 | int |
| `addDays()` | 日期加减天数 | string |
| `isWeekday()` | 判断是否为工作日 | bool |
| `getDateRange()` | 生成日期范围 | vector<string> |

### StringUtil API

| 方法 | 说明 | 返回值 |
|------|------|--------|
| `trim()` | 去除两侧空白 | string |
| `split()` | 分割字符串 | vector<string> |
| `join()` | 连接字符串 | string |
| `toUpper()` | 转换为大写 | string |
| `toLower()` | 转换为小写 | string |
| `toInt()` | 字符串转整数 | int |
| `toDouble()` | 字符串转浮点数 | double |

### MathUtil API

| 方法 | 说明 | 返回值 |
|------|------|--------|
| `mean()` | 计算平均值 | double |
| `median()` | 计算中位数 | double |
| `stddev()` | 计算标准差 | double |
| `correlation()` | 计算相关系数 | double |
| `returnRate()` | 计算收益率 | double |
| `maxDrawdown()` | 计算最大回撤 | double |
| `normalize()` | 归一化 | vector<double> |

## 🔧 编译和使用

### 编译

工具类模块已集成到项目的 CMake 构建系统中：

```bash
mkdir -p build
cd build
cmake ..
make
```

### 在代码中使用

```cpp
// 包含统一头文件
#include "Utils.h"

// 或者单独包含
#include "TimeUtil.h"
#include "StringUtil.h"
#include "MathUtil.h"
```

## 📝 注意事项

1. **时间工具**：
   - 所有日期字符串默认格式为 "YYYY-MM-DD"
   - 时间戳单位为秒，除非特别说明
   - 星期几：0=周日，1=周一，...，6=周六

2. **字符串工具**：
   - 类型转换失败时返回默认值，不抛出异常
   - 空字符串判断使用 `isBlank()` 而不是 `empty()`

3. **数学工具**：
   - 空数组或无效输入返回 NaN
   - 金融计算中的百分比已乘以 100
   - 移动计算返回的数组长度会减少

## 🎯 最佳实践

1. **使用命名空间**：
```cpp
using namespace utils;
std::string today = TimeUtil::today();
```

2. **错误处理**：
```cpp
double value = StringUtil::toDouble(str, 0.0);  // 提供默认值
if (std::isnan(value)) {
    // 处理无效值
}
```

3. **性能优化**：
```cpp
// 对于大量字符串操作，考虑预分配空间
std::vector<std::string> parts;
parts.reserve(100);
```

## 📚 相关文档

- [SUMMARY.md](SUMMARY.md) - 开发总结
- [DESIGN.md](../DESIGN.md) - 系统设计文档
- [README.md](../README.md) - 项目主文档

---

**模块版本**: 1.0.0  
**最后更新**: 2026-02-01  
**维护者**: Development Team

