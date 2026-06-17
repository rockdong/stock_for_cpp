# Stock Analysis System - 用户指南

> **版本**: v1.0.0  
> **更新日期**: 2026-06-16  
> **适用对象**: 最终用户、运维人员、开发团队

---

## 📋 目录

1. [系统简介](#系统简介)
2. [系统要求](#系统要求)
3. [快速开始](#快速开始)
4. [客户端使用](#客户端使用)
5. [配置说明](#配置说明)
6. [常见问题](#常见问题)
7. [调试方法](#调试方法)

---

## 🎯 系统简介

Stock Analysis System 是一个统一核心库 + 三种客户端架构的股票分析系统：

### 架构概览

```
stock_core (统一核心库)
    ├── 业务逻辑：股票分析、数据处理、策略执行
    ├── 数据源：Tushare API 集成
    ├── 数据库：SQLite/MySQL 支持
    └── 调度器：定时任务执行
    ↓
├── stock_exe (命令行客户端)
│   适用场景：服务器自动化、脚本调用、CI/CD集成
│
├── stock_tui (FTXUI终端客户端)
│   适用场景：SSH远程操作、服务器交互、实时监控
│
└── stock_gui (Qt图形客户端)
    适用场景：桌面环境、可视化分析、数据导出
```

### 核心功能

- ✅ 股票数据获取（Tushare API）
- ✅ 技术指标分析（EMA、涨跌信号）
- ✅ 批量并发处理（ThreadPool）
- ✅ 定时调度执行
- ✅ 数据持久化（SQLite/MySQL）
- ✅ 实时进度监控
- ✅ 日志追踪

---

## 💻 系统要求

### 硬件要求

| 项目 | 最低配置 | 推荐配置 |
|------|---------|---------|
| CPU | 2核 | 4核+ |
| 内存 | 4GB | 8GB+ |
| 存储 | 1GB | 5GB+ |

### 软件要求

#### 必需依赖

- **操作系统**: macOS 10.15+ / Linux (Ubuntu 20.04+)
- **编译器**: Clang 10+ / GCC 9+ (支持 C++17)
- **构建工具**: CMake 3.16+
- **数据库**: SQLite 3.30+

#### 客户端特定依赖

| 客户端 | 额外依赖 |
|--------|---------|
| **stock_exe** | 无 |
| **stock_tui** | FTXUI (已包含在 thirdparty/) |
| **stock_gui** | Qt 6.2+ (Qt Charts 模块) |

---

## 🚀 快速开始

### 步骤 1: 配置环境变量

**必需配置**:

```bash
# 设置 Tushare API 密钥（必需）
export DATA_SOURCE_API_KEY="your_tushare_api_key_here"

# 验证配置
echo $DATA_SOURCE_API_KEY
```

**获取 API 密钥**:
1. 访问 [Tushare Pro](https://tushare.pro)
2. 注册账号并登录
3. 在"用户中心"获取 API Token
4. 将 Token 设置到环境变量

### 步骤 2: 检查数据库

```bash
cd cpp/data

# 检查数据库文件是否存在
ls -lh stock.db

# 查看表结构
sqlite3 stock.db ".tables"

# 如果数据库不存在，创建基础结构
sqlite3 stock.db "CREATE TABLE IF NOT EXISTS stocks (
    ts_code TEXT PRIMARY KEY,
    symbol TEXT,
    name TEXT,
    industry TEXT,
    list_date TEXT
);"
```

### 步骤 3: 运行客户端

#### stock_exe (命令行)

```bash
cd cpp/build

# 查看帮助
./stock_exe/stock_exe --help

# 单次执行模式
./stock_exe/stock_exe --once

# 定时执行模式
./stock_exe/stock_exe --time 20:00
```

#### stock_tui (终端界面)

```bash
cd cpp/build

# 运行 TUI（需要交互式终端）
./stock_tui/stock_tui

# 操作说明：
# - 使用方向键导航
# - Enter 键选择菜单
# - Tab 键切换标签
# - Ctrl+C 退出
```

#### stock_gui (图形界面)

```bash
cd cpp/build

# 运行 GUI（需要图形环境）
./stock_gui/stock_gui

# macOS 可能需要：
open stock_gui/stock_gui
```

---

## 📖 客户端使用

### stock_exe - 命令行客户端

#### 参数说明

```bash
用法: ./stock_exe/stock_exe [选项]

选项:
  --once, -o       单次执行模式（立即执行一次分析）
  --time, -t TIME  定时执行模式（设置每天执行时间，格式 HH:MM）
  --help, -h       显示帮助信息
```

#### 使用示例

**单次执行**:
```bash
# 立即执行一次股票分析
./stock_exe/stock_exe --once

# 输出：
# [2026-06-16 22:40:01] [INFO] Stock Core v1.0.0 初始化开始
# [2026-06-16 22:40:02] [INFO] 配置系统初始化成功
# [2026-06-16 22:40:03] [INFO] 数据源连接成功
# [2026-06-16 22:40:10] [INFO] 分析完成：100只股票，成功95，失败5
```

**定时执行**:
```bash
# 每天晚上 20:00 自动执行
./stock_exe/stock_exe --time 20:00

# 输出：
# [2026-06-16 22:40:01] [INFO] 调度器启动，执行时间：20:00
# [2026-06-16 20:00:01] [INFO] 开始执行分析任务...
```

#### 适用场景

- ✅ **服务器自动化**: 通过 systemd/cron 集成
- ✅ **CI/CD 集成**: 作为构建步骤执行
- ✅ **脚本调用**: 通过 shell 脚本批量处理
- ✅ **容器化部署**: Docker 容器内执行

---

### stock_tui - FTXUI 终端客户端

#### 界面布局

```
┌─────────────────────────────────────────┐
│ Stock Analysis System v1.0.0            │
├─────────────────────────────────────────┤
│ [概览] [股票列表] [快速操作]             │
├─────────────────────────────────────────┤
│                                         │
│  股票统计                               │
│  ├─ 总数: 100                           │
│  ├─ 待分析: 50                          │
│  └─ 已完成: 45                          │
│                                         │
│  分析进度                               │
│  [████████░░░░░░░░░░] 45%              │
│  完成数: 45  失败数: 5                  │
│                                         │
├─────────────────────────────────────────┤
│ 状态: 运行中                            │
└─────────────────────────────────────────┘
```

#### 操作说明

**导航菜单**:
- ↑/↓ 方向键：选择菜单项
- Enter：确认选择
- Tab：切换标签页

**快捷键**:
- `Ctrl+C`: 退出程序
- `Ctrl+R`: 刷新数据
- `Ctrl+L`: 查看日志

#### 功能标签

**概览标签**:
- 股票统计信息
- 分析进度条
- 完成数/失败数

**股票列表标签**:
- 股票代码表格
- 状态筛选
- 搜索功能

**快速操作标签**:
- 开始分析按钮
- 刷新数据按钮
- 查看日志按钮

#### 适用场景

- ✅ **SSH 远程操作**: 服务器远程管理
- ✅ **实时监控**: 分析进度实时查看
- ✅ **运维调试**: 快速诊断和调试
- ✅ **终端环境**: 无图形界面的服务器

---

### stock_gui - Qt 图形客户端

#### 主窗口布局

```
┌─────────────────────────────────────────────────────┐
│ Stock Analysis System v1.0.0                        │
├─────────────────────────────────────────────────────┤
│ 文件  编辑  视图  分析  工具  帮助                   │
├─────────────────────────────────────────────────────┤
│ [分析] [配置] [日志] [状态]                         │
├─────────────────────────────────────────────────────┤
│                                                     │
│  ┌────────────────────────────────────────────┐   │
│  │ 股价走势图表                               │   │
│  │ [实时数据可视化]                           │   │
│  └────────────────────────────────────────────┘   │
│                                                     │
│  ┌──────────────┐ ┌──────────────┐                │
│  │ 进度: 45%    │ │ 完成: 95     │                │
│  └──────────────┘ └──────────────┘                │
│                                                     │
├─────────────────────────────────────────────────────┤
│ 状态: 运行中 | 数据源: Tushare | 数据库: SQLite    │
└─────────────────────────────────────────────────────┘
```

#### 功能标签

**分析标签**:
- Qt Charts 股价走势图
- 进度条和统计信息
- 开始/停止分析按钮

**配置标签**:
- API 配置编辑
- 策略选择
- 调度时间设置

**日志标签**:
- 实时日志显示
- 日志级别筛选
- 清空日志按钮

**状态标签**:
- 系统状态监控
- 数据库连接状态
- API 响应状态

#### 特色功能

- 🎨 **可视化图表**: Qt Charts 股价走势
- 🎨 **数据导出**: CSV/PDF 报告导出
- 🎨 **配置编辑**: 可视化参数设置
- 🎨 **主题切换**: 深色/浅色主题

#### 适用场景

- ✅ **桌面环境**: 本地桌面使用
- ✅ **可视化分析**: 图表展示和数据分析
- ✅ **数据导出**: 报告生成和数据导出
- ✅ **用户交互**: 图形界面友好操作

---

## ⚙️ 配置说明

### .env 配置文件

**位置**: `cpp/.env`

**配置项详解**:

```ini
# ==================== 日志配置 ====================
LOG_LEVEL=DEBUG
# 日志级别: DEBUG, INFO, WARN, ERROR, FATAL

LOG_PATTERN=[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%t] %v
# 日志格式: 时间戳 + 日志级别 + 线程ID + 消息

LOG_FILE_PATH=logs/app.log
# 日志文件路径

LOG_CONSOLE_ENABLED=true
# 是否输出到控制台

LOG_FILE_ENABLED=true
# 是否输出到文件

LOG_MAX_FILE_SIZE=10485760
# 最大文件大小 (10MB)

LOG_MAX_FILES=3
# 最大日志文件数量

# ==================== 数据源配置 ====================
DATA_SOURCE_URL=http://api.tushare.pro
# Tushare API 地址

# DATA_SOURCE_API_KEY 通过环境变量设置，不要在此硬编码
# 设置方法: export DATA_SOURCE_API_KEY=your_api_key

# ==================== 限流配置 ====================
TUSHARE_RATE_LIMIT=8
# 每秒最多请求数 (Tushare 限制 500次/分钟 = 8.33次/秒)

TUSHARE_BURST_SIZE=10
# 突发容量 (允许短暂超速的请求数)

# ==================== 策略配置 ====================
STRATEGIES=EMA25_GREATER_17_PRICE_MATCH,SURGE_SIGNAL
# 启用的策略列表（逗号分隔）

# ==================== 调度配置 ====================
SCHEDULER_EXECUTE_TIME=20:00
# 每天执行时间 (格式 HH:MM)

ANALYSIS_DATE_SWITCH_TIME=02:00
# 分析日期切换时间 (格式 HH:MM)
# 当前时间 < 切换时间 → 分析昨天
# 当前时间 >= 切换时间 → 分析今天

# ==================== 数据库配置 ====================
DATABASE_PATH=data/stock.db
# SQLite 数据库路径

DATABASE_TYPE=sqlite
# 数据库类型: sqlite, mysql
```

### 环境变量配置

**必需环境变量**:

```bash
# Tushare API 密钥（必需）
export DATA_SOURCE_API_KEY="your_tushare_api_key"
```

**可选环境变量**:

```bash
# 覆盖 .env 配置
export LOG_LEVEL=INFO
export SCHEDULER_EXECUTE_TIME=18:00
export DATABASE_PATH=/custom/path/stock.db
```

---

## 🔧 常见问题

### Q1: 运行时出现段错误 (Segmentation fault)

**原因分析**:
- 数据库文件不存在
- API 密钥未配置
- 配置文件缺失必需项

**解决方案**:

```bash
# 1. 创建数据库
cd cpp/data
sqlite3 stock.db "CREATE TABLE IF NOT EXISTS stocks (
    ts_code TEXT PRIMARY KEY,
    symbol TEXT,
    name TEXT
);"

# 2. 配置 API 密钥
export DATA_SOURCE_API_KEY="your_api_key"

# 3. 使用调试器定位问题
cd build
lldb ./stock_exe/stock_exe
(lldb) run --once
# 段错误后查看调用栈
(lldb) bt
(lldb) frame select 0
```

---

### Q2: Tushare API 连接失败

**错误信息**:
```
[ERROR] 数据源连接失败: HTTP 401 Unauthorized
```

**解决方案**:

```bash
# 1. 检查 API 密钥
echo $DATA_SOURCE_API_KEY

# 2. 验证 API 密钥有效性
curl -X POST http://api.tushare.pro \
  -H "Content-Type: application/json" \
  -d '{"api_name': 'daily', 'token': 'your_api_key'}'

# 3. 检查网络连接
ping api.tushare.pro
```

---

### Q3: 数据库写入失败

**错误信息**:
```
[ERROR] 数据库写入失败: database is locked
```

**解决方案**:

```bash
# 1. 检查数据库文件权限
ls -la data/stock.db

# 2. 检查是否有其他进程占用
lsof data/stock.db

# 3. 修复权限
chmod 644 data/stock.db

# 4. 如果锁定，删除锁定文件
rm data/stock.db-wal data/stock.db-shm
```

---

### Q4: Qt GUI 无法启动

**错误信息**:
```
This application failed to start because no Qt platform plugin could be initialized.
```

**解决方案 (macOS)**:

```bash
# 1. 检查 Qt 安装
brew list qt6

# 2. 设置 Qt 平台插件
export QT_QPA_PLATFORM_PLUGIN_PATH=/opt/homebrew/Cellar/qt/6.11.1/lib/QtPlugins

# 3. 使用 open 命令启动
open stock_gui/stock_gui
```

**解决方案 (Linux)**:

```bash
# 1. 安装 Qt 平台插件
sudo apt install qt6-qpa-plugins

# 2. 设置环境变量
export QT_QPA_PLATFORM=xcb

# 3. 运行
./stock_gui/stock_gui
```

---

### Q5: 限流配置导致请求失败

**错误信息**:
```
[ERROR] API请求限流: Rate limit exceeded
```

**解决方案**:

```bash
# 1. 调整 .env 配置
# 降低每秒请求数
TUSHARE_RATE_LIMIT=5

# 2. 等待一段时间后重试
# Tushare 限制: 500次/分钟

# 3. 查看当前请求统计
tail logs/app.log | grep "API请求"
```

---

## 🐛 调试方法

### 使用 lldb 调试段错误

**完整步骤**:

```bash
cd build

# 1. 启动调试器
lldb ./stock_exe/stock_exe

# 2. 运行程序
(lldb) run --once

# 3. 段错误发生后，查看调用栈
(lldb) bt

# 输出示例：
# * thread #1, stop reason = signal SIGSEGV
#   * frame #0: 0x0000000100abc123 stock_exe`main(argc=2, argv=...) at main.cpp:30
#     frame #1: 0x0000000100def456 stock_core`StockCoreContext::initialize() at StockCoreContext.cpp:50
#     ...

# 4. 查看具体代码位置
(lldb) frame select 0

# 5. 查看变量值
(lldb) p config_
(lldb) p database_

# 6. 查看源代码
(lldb) l

# 7. 继续调试
(lldb) c
```

### 查看详细日志

```bash
# 1. 设置日志级别为 DEBUG
# 在 .env 中设置：
LOG_LEVEL=DEBUG

# 2. 运行程序并查看日志
./stock_exe/stock_exe --once

# 3. 查看日志文件
tail -f logs/app.log

# 4. 查看错误日志
grep "ERROR" logs/app.log

# 5. 查看特定时间段日志
grep "2026-06-16 22:" logs/app.log
```

### 使用诊断工具

```bash
# 1. 检查依赖库
ldd stock_exe/stock_exe  # Linux
otool -L stock_exe/stock_exe  # macOS

# 2. 检查内存使用
ps aux | grep stock_exe

# 3. 检查打开的文件
lsof -p $(pgrep stock_exe)

# 4. 检查系统调用
strace -o trace.log ./stock_exe/stock_exe --once  # Linux
dtruss ./stock_exe/stock_exe --once  # macOS
```

---

## 📚 参考文档

### 技术文档

- [架构设计文档](docs/plans/2026-06-14-architecture-refactor-design.md)
- [编译修复报告](docs/COMPILATION_FIX_REPORT.md)
- [客户端测试报告](docs/CLIENT_TEST_REPORT.md)
- [团队培训材料](docs/TEAM_TRAINING_COMPILATION_FIXES.md)

### API 文档

- [Tushare Pro API 文档](https://tushare.pro/document/2)
- [FTXUI 文档](https://github.comArthurSonzogni/FTXUI)
- [Qt 6 文档](https://doc.qt.io/qt-6/)

### 配置示例

- [.env 配置示例](cpp/.env)
- [数据库初始化脚本](scripts/init_db.sh)

---

## 🆘 获取帮助

### 问题反馈

如果遇到问题，请按以下步骤反馈：

1. **收集信息**:
   - 操作系统版本
   - 错误信息和日志
   - 配置文件内容

2. **创建问题报告**:
   ```markdown
   **问题描述**: [简述问题]
   
   **环境信息**:
   - 操作系统: macOS 12.6
   - 编译器: Clang 14.0
   - Qt版本: 6.11.1
   
   **错误日志**:
   ```
   [粘贴错误日志]
   ```
   
   **已尝试的解决方案**:
   - [列出已尝试的方法]
   ```

3. **提交问题**: 
   - 通过团队问题跟踪系统提交
   - 或联系开发团队

---

## 📝 更新历史

| 日期 | 版本 | 更新内容 |
|------|------|---------|
| 2026-06-16 | v1.0.0 | 初始版本，包含三种客户端完整指南 |
| 2026-06-14 | v0.1.0 | 架构设计文档初稿 |

---

**维护团队**: Senior Developer (高级开发工程师)  
**文档状态**: ✅ 已验证  
**最后更新**: 2026-06-16 22:40