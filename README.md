# 股票分析工具

基于C++开发的高性能股票数据分析和可视化工具。

## 功能特性

- 实时股票数据获取
- 技术指标计算
- K线图绘制
- 历史数据回测
- 自定义策略回测

## 安装方法

```bash
# 克隆项目源码
git clone https://github.com/your-repo/stock-analyzer.git

# 编译安装
cd stock-analyzer
mkdir build
cd build
cmake ..
make -j4
```

## 项目结构

```
stock_for_cpp/
├── src/                 # C++源文件
├── include/             # 头文件
├── data/                # 数据文件目录
├── docs/                # 项目文档
├── tests/               # 测试代码
├── CMakeLists.txt       # 编译配置
└── README.md            # 项目说明文档
```

## 快速使用

```cpp
// 示例代码：加载股票数据并计算技术指标
#include "stock_analyzer.h"

int main() {
    StockAnalyzer analyzer;
    analyzer.loadData("AAPL");
    analyzer.calculateIndicators();
    analyzer.visualize();
    return 0;
}
```

## 使用方法

### 1. 直接编译运行

```bash
# 克隆项目
git clone https://github.com/your-repo/stock-analyzer.git
cd stock-analyzer

# 创建构建目录
mkdir build && cd build

# 配置编译选项
cmake ..

# 编译项目
make -j$(nproc)

# 运行程序
./stock_analyzer --symbol AAPL --period 30d
```

### 2. Docker Compose 启动

```bash
# 使用预构建镜像启动服务
docker-compose up -d

# 启动所有服务（包括辅助工具）
docker-compose --profile tools up -d

# 仅启动核心分析服务
docker-compose up stock-analysis -d
```

**注意事项**：
- 确保在运行前配置好 `.env` 文件中的 API 密钥和相关凭证
- C++ 服务需要 Tushare API 密钥才能获取股票数据
- Node.js 服务需要飞书机器人配置才能启动聊天机器人功能

服务端口映射：
- 主服务：3000 (应用) / 8888 (日志监控) / 8080 (数据库浏览器)

### 3. 命令行使用

```bash
# 分析指定股票
./stock_analyzer --symbol AAPL --period 30d

# 获取实时数据
./stock_analyzer --symbol MSFT --live

# 执行回测
./stock_analyzer --backtest --strategy ma_crossover --symbol TSLA
```

### 4. API 使用

#### 数据获取
```cpp
StockData data = StockAPI::getHistoricalData("AAPL", "1y", "1d");
StockData live_data = StockAPI::getLivePrice("MSFT");
```

#### 技术指标计算
```cpp
// 计算移动平均线
auto ma = TechnicalIndicators::calculateMA(data.close_prices, 20);

// 计算RSI
auto rsi = TechnicalIndicators::calculateRSI(data.close_prices, 14);

// 计算MACD
auto macd = TechnicalIndicators::calculateMACD(data.close_prices);
```

#### 策略回测
```cpp
BacktestEngine engine;
engine.loadStrategy("ma_crossover");
engine.setInitialCapital(10000.0);
engine.runBacktest(data);
BacktestResult result = engine.getResult();
```

### 5. GUI 使用

1. 启动程序：`./stock_analyzer_gui`
2. 在搜索框输入股票代码（如 AAPL）
3. 选择时间范围和周期
4. 选择要显示的技术指标
5. 点击"开始分析"按钮
6. 查看生成的图表和回测报告

### 6. 配置文件

```yaml
api:
  provider: yahoo
  key: your_api_key
  
data:
  cache_dir: ./cache
  refresh_interval: 300  # seconds
  
visualization:
  theme: dark
  width: 1200
  height: 800
```

## 依赖库

- Boost
- Eigen3
- RapidJSON
- Qt5 (GUI部分)
- SQLite3

## 配置参数

- **数据源**: 支持Yahoo Finance、Alpha Vantage等多种数据API
- **时间周期**: 日线、小时线、分钟线等多时间周期数据
- **指标参数**: 移动平均线、RSI、MACD等技术指标

## 许可证

MIT License - 详见 LICENSE 文件

## 更新日志

### v1.0.0
- 实现基本的数据获取功能
- 添加了基础的技术指标计算
- 集成了Qt可视化界面

## 开发计划

- 支持更多数据源
- 优化回测性能
- 添加机器学习预测模块