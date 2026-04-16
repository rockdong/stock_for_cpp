# 股票分析系统

基于 C++ 开发的高性能股票数据分析系统，集成飞书机器人推送服务，支持多种技术策略分析和买点预测。

## 功能特性

### 核心功能
- 🚀 **高性能分析引擎** - C++ 实现的技术指标计算
- 📊 **多策略支持** - EMA 交叉、MACD、RSI、布林带等
- 📈 **基本面筛选** - 4维度评分模型筛选优质股票
- 🤖 **飞书机器人** - 实时推送分析结果到飞书群
- 📈 **买点预测** - EMA17/EMA25 黄金交叉预测
- 🔄 **自适应学习** - 根据历史结果自动优化预测参数
- ⏰ **定时调度** - 自动执行每日分析任务
- 🌐 **Web 前端** - 分析过程数据可视化展示 + Tab 导航
- 📱 **微信小程序** - 移动端查看分析结果

### 基本面筛选功能

基于财务指标的4维度评分模型，自动筛选优质股票：

| 维度 | 权重 | 评分标准 |
|------|------|----------|
| 估值 | 25分 | PE<30(10分) + PB<5(8分) + PEG<1.5(7分) |
| 盈利 | 35分 | ROE≥10(15分) + 毛利率≥20(10分) + 净利率>10(5分) |
| 增长 | 25分 | 营收增长≥5(10分) + 利润增长≥5(10分) |
| 健康 | 15分 | 负债率<60(8分) + 流动比率≥1(4分) + 现金流健康(3分) |

**评级标准**: A(≥80分) B(≥60分) C(≥40分) D(<40分)  
**筛选阈值**: 总分≥60 且 ROE≥10

### 支持的策略
| 策略 | 说明 |
|------|------|
| EMA25_GREATER_17_PRICE_MATCH | EMA25 > EMA17 价格匹配 |
| EMA17TO25 | EMA17/EMA25 黄金交叉 |
| EMA17_BREAKOUT | EMA17 突破策略 |
| EMA_CONVERGENCE | EMA 收敛策略 |
| EMA25_CROSSOVER | EMA25 交叉策略 |

## 快速开始

### Docker 部署（推荐）

```bash
# 1. 克隆项目
git clone https://github.com/rockdong/stock_for_cpp.git
cd stock_for_cpp

# 2. 配置环境变量
cp env/.env.example env/.env
vim env/.env  # 填写 Tushare API Key 和飞书配置

# 3. 启动服务
docker-compose up -d

# 4. 查看日志
docker-compose logs -f stock-analysis
```

### 本地编译

```bash
# 安装依赖
# macOS
brew install cmake sqlite3 openssl

# Ubuntu
sudo apt-get install build-essential cmake libsqlite3-dev libssl-dev

# 编译
cd cpp
mkdir build && cd build
cmake ..
make -j$(nproc)

# 运行
./stock_for_cpp
```

## 项目结构

```
stock_for_cpp/
├── cpp/                        # C++ 分析引擎
│   ├── main.cpp               # 主程序入口
│   ├── core/                  # 核心业务模块
│   │   ├── strategies/        # 策略实现
│   │   ├── PredictionHistory.h    # 预测历史管理
│   │   ├── AccuracyAnalyzer.h     # 准确率分析
│   │   └── ParameterOptimizer.h   # 参数优化器
│   ├── analysis/              # 技术指标计算
│   ├── network/               # 数据获取
│   ├── data/                  # 数据存储
│   │   └── database/          # 数据库访问
│   │       └── migrations/    # 数据库迁移脚本
│   ├── scheduler/             # 任务调度
│   └── utils/                 # 工具类
├── nodejs/                    # 飞书推送服务 + REST API
│   └── src/
│       ├── index.js           # 服务入口
│       ├── api.js             # REST API
│       ├── feishu.js          # 飞书 API
│       └── config.js          # 配置管理
├── web-frontend/              # Web 前端
│   ├── src/
│   │   ├── components/        # 组件
│   │   ├── pages/             # 页面
│   │   ├── services/          # API 服务
│   │   └── types/             # TypeScript 类型
│   └── package.json
├── miniprogram/               # 微信小程序
├── env/                       # 环境变量配置
│   └── .env.example           # 配置模板
├── .github/workflows/         # GitHub Actions CI/CD
├── docker-compose.yml         # Docker 编排
├── Dockerfile                 # Docker 镜像构建
└── README.md
```

## 配置说明

### 环境变量配置

复制配置模板并填写实际值：

```bash
cp env/.env.example env/.env
```

主要配置项：

```bash
# 数据源配置 (Tushare Pro API)
DATA_SOURCE_URL=http://api.tushare.pro
DATA_SOURCE_API_KEY=your_tushare_token_here

# 飞书机器人配置
FEISHU_APP_ID=your_app_id_here
FEISHU_APP_SECRET=your_app_secret_here
FEISHU_VERIFICATION_TOKEN=your_verification_token_here

# 策略配置
STRATEGIES=EMA25_GREATER_17_PRICE_MATCH

# 调度配置
SCHEDULER_EXECUTE_TIME=20:00
```

### 策略配置

在 `env/.env` 中配置要启用的策略（逗号分隔）：

```bash
STRATEGIES=EMA25_GREATER_17_PRICE_MATCH,EMA17TO25,EMA17_BREAKOUT
```

## Docker 服务

### 服务端口

| 服务 | 端口 | 说明 |
|------|------|------|
| stock-analysis | 3000 | 主分析服务 / REST API |
| web-frontend | 5173 | Web 前端（分析过程展示） |
| sqlite-browser | 8080 | 数据库浏览器 |
| dozzle | 8888 | 日志监控（需 `--profile tools`） |

### 启动命令

```bash
# 启动核心服务
docker-compose up -d

# 启动所有服务（包括工具）
docker-compose --profile tools --profile db up -d

# 开发模式（挂载本地代码）
docker-compose -f docker-compose.yml -f docker-compose.dev.yml up -d
```

## 买点预测系统

### 自适应学习机制

系统会根据历史预测结果自动优化参数：

```
预测信号 → 记录结果 → 收盘验证 → 分析准确率 → 优化参数 → 更新算法
```

### 触发优化的条件
- 连续 3 次预测失败
- 周准确率 < 70%
- 月准确率 < 80%

### 预测信号输出

预测结果会通过飞书推送，包含股票信息、技术分析、预测结论等。

## 技术栈

### C++ 后端
- **CMake** - 构建系统
- **spdlog** - 日志库
- **nlohmann/json** - JSON 处理
- **TA-Lib** - 技术指标计算
- **SQLite** - 数据存储
- **cpp-httplib** - HTTP 客户端

### Node.js 服务
- **Node.js 20** - 运行时
- **lark-api** - 飞书 SDK
- **dotenv** - 环境变量管理

## 开发指南

### 添加新策略

1. 在 `cpp/core/strategies/` 创建策略类
2. 继承 `Strategy` 基类
3. 实现 `analyze()` 方法
4. 在 `StrategyFactory` 注册策略
5. 更新 `CMakeLists.txt`

### 本地开发

```bash
# 编译 C++
cd cpp && mkdir build && cd build && cmake .. && make -j4

# 运行 Node.js 服务
cd nodejs && npm install && npm start
```

## GitHub Actions

CI/CD 流程：
1. 编译 C++ (Ubuntu + macOS)
2. 构建 Node.js 项目
3. 构建 Docker 镜像
4. 推送到 Docker Hub

触发条件：
- Push 到 `master` 分支
- 发布 `v*.*.*` 标签

## 许可证

MIT License

## 更新日志

### v2.0.0
- 新增买点预测系统
- 添加自适应学习机制
- 集成飞书机器人推送
- 优化 Docker 部署流程
- 简化配置管理（统一 env 目录）

### v1.0.0
- 基础技术指标计算
- 多策略支持
- 定时任务调度