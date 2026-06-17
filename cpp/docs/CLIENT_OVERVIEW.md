# 🎯 Stock for C++ 三客户端总览

**版本**: v1.0  
**更新日期**: 2026-06-18  
**维护者**: Senior Developer Team

---

## 📋 项目概览

Stock for C++ 提供三种客户端，满足不同使用场景：

| 客户端 | 类型 | 适用场景 | 技术栈 |
|--------|------|----------|--------|
| **stock_tui** | TUI（终端界面） | 服务器/终端用户 | FTXUI + stock_core |
| **stock_gui** | GUI（图形界面） | 桌面用户 | Qt6 + stock_core |
| **stock_exe** | CLI（命令行） | 自动化/批处理 | stock_core |

---

## 🖥️ 客户端详解

### 1. stock_tui（FTXUI 终端界面）

**特性**:
- ✅ **交互式终端界面**: 基于 FTXUI 的现代化 TUI
- ✅ **键盘快捷键**: A/R/I/L/Q 快捷键响应
- ✅ **实时日志**: 内嵌日志查看器
- ✅ **标签页导航**: 数据/分析/配置/日志四个页面
- ✅ **CSV导入**: 支持 CSV 数据批量导入

**构建文档**: `docs/TUI_INTERACTIVE_GUIDE.md`

**构建命令**:
```bash
cd build_tui
cmake -DBUILD_TUI=ON ..
make stock_tui
./stock_tui
```

**交互功能**:
| 快捷键 | 功能 |
|--------|------|
| `A` | 分析股票 |
| `R` | 刷新数据 |
| `I` | 导入CSV数据 |
| `L` | 切换到日志标签 |
| `Q` | 退出程序 |

**修复历史**:
- 段错误调试（5个关键问题）: `docs/TUI_SEGFAULT_FIX_REPORT.md`
- 键盘响应实现: `docs/TUI_KEYBOARD_RESPONSE_REPORT.md`
- CSV导入功能: `docs/CSV_IMPORT_COMPLETE_REPORT.md`

---

### 2. stock_gui（Qt 图形界面）

**特性**:
- ✅ **现代化图形界面**: 基于 Qt6 的桌面应用
- ✅ **图表可视化**: Qt Charts 股价走势图
- ✅ **数据适配器**: StockListModel（核心库→Qt模型）
- ✅ **实时日志集成**: LogAdapter 桥接核心库日志
- ✅ **进度可视化**: ProgressAdapter 进度条显示

**构建文档**: `docs/GUI_BUILD_GUIDE.md`

**构建命令**:
```bash
cd build_gui
cmake -DBUILD_GUI=ON ..
make stock_gui
./stock_gui/stock_gui
```

**界面结构**:
```
┌──────────────────────────────────────┐
│  主窗口 - 菜单栏/工具栏/状态栏       │
├──────────────────────────────────────┤
│  标签页导航                          │
│  [分析] [配置] [日志] [状态]        │
│  ┌────────────────────────────┐    │
│  │  股价走势图（Qt Charts）   │    │
│  │  控制按钮/进度条           │    │
│  │  统计信息                  │    │
│  └────────────────────────────┘    │
└──────────────────────────────────────┘
```

**数据适配器**:
| 适配器 | 功能 |
|--------|------|
| StockListModel | 股票列表数据模型（QAbstractListModel） |
| LogAdapter | 日志系统桥接（核心库→Qt信号） |
| ProgressAdapter | 进度回调桥接（核心库→进度条） |

---

### 3. stock_exe（命令行执行）

**特性**:
- ✅ **简单命令行**: 参数驱动的批处理执行
- ✅ **单次执行模式**: `--once` 执行一次分析
- ✅ **定时执行模式**: `--time HH:MM` 定时执行
- ✅ **无人值守运行**: 适合自动化任务
- ✅ **进度显示**: 实时进度条输出

**构建文档**: `docs/TESTING_CHECKLIST.md`

**构建命令**:
```bash
cd build_exe
cmake -DBUILD_EXE=ON ..
make stock_exe
./stock_exe --help
```

**使用示例**:
```bash
# 单次执行
./stock_exe --once

# 定时执行（每天 20:00）
./stock_exe --time 20:00

# 查看帮助
./stock_exe --help
```

---

## 🔧 核心库集成

所有客户端共享同一核心库（stock_core）:

### 核心库架构

```
stock_core/
├── stock_core.h/cpp          # API 入口
├── StockCoreContext.h/cpp    # 核心上下文
├── api/                      # API 接口
├── core/                     # 核心数据结构
├── data/                     # 数据访问层
├── analysis/                 # 分析引擎
├── network/                  # 数据源
├── scheduler/                # 定时任务
├── log/                      # 日志系统
├── config/                   # 配置管理
└── utils/                    # 工具类
```

### API 使用示例

```cpp
// 初始化核心库
stock_core::api::initialize();

// 加载股票列表
auto stocks = stock_core::api::loadStockList();

// 执行分析
stock_core::api::performDailyAnalysis();

// 导入CSV数据
int imported = stock_core::api::importStocksFromCSV("data/sample_stocks.csv");

// 关闭核心库
stock_core::api::shutdown();
```

---

## 📊 构建对比

| 客户端 | 构建时间 | 可执行文件大小 | 依赖 |
|--------|---------|--------------|------|
| stock_tui | ~2分钟 | ~2.5MB | FTXUI |
| stock_gui | ~2分钟 | ~2.7MB | Qt6 |
| stock_exe | ~1分钟 | ~1.5MB | 无额外依赖 |

---

## 🧪 测试状态

| 客户端 | 构建测试 | 功能测试 | 集成测试 |
|--------|---------|---------|---------|
| stock_tui | ✅ 通过 | ✅ 交互功能已验证 | ⏳ 待完整测试 |
| stock_gui | ✅ 通过 | ⏳ 待功能完善 | ⏳ 待完整测试 |
| stock_exe | ✅ 通过 | ⏳ 待环境配置 | ⏳ 待完整测试 |

---

## 📁 项目目录结构

```
stock_for_cpp/cpp/
├── stock_core/          # 核心库（共享）
├── stock_tui/           # TUI 客户端
├── stock_gui/           # GUI 客户端
├── stock_exe/           # CLI 客户端
├── build_tui/           # TUI 构建产物
├── build_gui/           # GUI 构建产物
├── build_exe/           # exe 构建产物
├── docs/                # 文档目录
│   ├── CLIENT_OVERVIEW.md           # 本文档
│   ├── TUI_INTERACTIVE_GUIDE.md     # TUI 指南
│   ├── GUI_BUILD_GUIDE.md           # GUI 指南
│   ├── TESTING_CHECKLIST.md         # exe 测试清单
│   ├── CSV_IMPORT_COMPLETE_REPORT.md # CSV 导入报告
│   └── ARCHITECTURE_GUIDE.md        # 架构指南
├── data/                # 数据目录
│   └── sample_stocks.csv # 示例股票数据
└── logs/                # 日志目录
```

---

## 🚀 快速开始

### 选择适合的客户端

**场景 1: 服务器自动化**
```bash
# 使用 stock_exe（命令行）
cd build_exe/stock_exe
./stock_exe --once  # 单次分析
```

**场景 2: 终端交互**
```bash
# 使用 stock_tui（TUI）
cd build_tui/stock_tui
./stock_tui         # 启动交互界面
按 A 分析股票       # 键盘快捷键
按 I 导入CSV数据    # 导入数据
```

**场景 3: 桌面图形**
```bash
# 使用 stock_gui（GUI）
cd build_gui/stock_gui
./stock_gui         # 启动图形界面
点击"开始分析"      # 图形操作
```

---

## 🔗 相关文档链接

- **架构设计**: `docs/ARCHITECTURE_GUIDE.md`
- **核心库API**: `docs/API_USAGE.md`
- **TUI交互指南**: `docs/TUI_INTERACTIVE_GUIDE.md`
- **GUI构建指南**: `docs/GUI_BUILD_GUIDE.md`
- **exe测试清单**: `docs/TESTING_CHECKLIST.md`
- **CSV导入报告**: `docs/CSV_IMPORT_COMPLETE_REPORT.md`
- **重构完成报告**: `docs/REFACTORING_COMPLETE.md`

---

## 🎯 下一步计划

### 完善三个客户端

**TUI客户端**:
- [x] 段错误修复完成
- [x] 键盘响应实现
- [x] CSV导入功能
- [ ] 完整业务逻辑集成

**GUI客户端**:
- [x] 基本框架构建
- [x] 数据适配器实现
- [ ] 股票列表显示
- [ ] 分析功能集成
- [ ] 图表数据加载

**exe客户端**:
- [x] 命令行参数实现
- [ ] 完整环境配置测试
- [ ] 定时执行验证

---

**三个客户端已构建成功，按需选择使用！**

| 使用场景 | 推荐客户端 |
|----------|------------|
| **自动化/批处理** | `stock_exe` |
| **服务器终端** | `stock_tui` |
| **桌面可视化** | `stock_gui` |