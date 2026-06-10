# Stock for C++ Qt UI 版本设计方案

> **创建日期**: 2026-06-10  
> **状态**: 设计完成，待实施  
> **版本**: v1.0

---

## 📋 概述

### 背景

现有 C++ 分析核心通过 Docker 版本提供服务，缺少本地桌面 UI 版本。本项目旨在新增 Qt 6 GUI 版本，提供完整的股票交易终端体验。

### 目标

- ✅ **本地桌面应用**：跨平台（Win/Mac/Linux）独立可执行文件
- ✅ **直接调用核心库**：共享 `core_lib` / `analysis_lib`，无网络开销
- ✅ **专业版功能**：K线图表、策略参数调整、实时计算、多窗口布局
- ✅ **与现代 Web 前端并存**：同一核心库，两种前端形态

---

## 🏗️ 敶体架构

### 架构图

```
┌─────────────────────────────────────────────────────┐
│           stock_ui (Qt6 可执行文件)                   │
├─────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐  │
│  │  主窗口      │  │  K线图表     │  │  策略面板    │  │
│  │  QMainWindow│  │  Qt Charts  │  │  参数配置    │  │
│  └─────────────┘  └─────────────┘  └─────────────┘  │
├─────────────────────────────────────────────────────┤
│           Qt 适配层 (新模块 ui_lib)                  │
│  - 数据模型转换 (AnalysisResult → QAbstractListModel)│
│  - 信号槽绑定 (策略更新 → UI 刷新)                    │
│  - 配置持久化 (QSettings)                            │
├─────────────────────────────────────────────────────┤
│           现有核心库 (直接链接)                       │
│  core_lib     analysis_lib    data_lib    network_lib│
└─────────────────────────────────────────────────────┘
```

### 关键设计点

1. **ui_lib 适配层**：隔离 Qt 类型与核心库业务逻辑
2. **核心库纯净性**：`core_lib` 保持无 Qt 依赖
3. **编译产物并存**：`stock_ui` + `ui_lib` 与现有 `stock_for_cpp` CLI/Docker 版本并存

---

## 📁 目录结构

### 新增 UI 模块目录

```
cpp/
├── ui/                     # 新增 UI 模块
│   ├── CMakeLists.txt      # ui_lib 构建配置
│   ├── src/
│   │   ├── main.cpp        # Qt 应用入口
│   │   ├── MainWindow.cpp  # 主窗口实现
│   │   ├── adapters/       # 适配层（核心库 → Qt 数据模型）
│   │   │   ├── StockListModel.cpp
│   │   │   ├── StrategyResultModel.cpp
│   │   │   └── ChartDataAdapter.cpp
│   │   ├── widgets/        # 自定义控件
│   │   │   ├── StockListView.cpp
│   │   │   ├── CandlestickWidget.cpp
│   │   │   └── StrategyPanel.cpp
│   │   └── dialogs/        # 配置对话框
│   │       └── SettingsDialog.cpp
│   └── resources/          # Qt 资源文件
│       ├── icons.qrc
│       └── styles.qrc      # QSS 样式表
```

---

## 🔧 CMake 配置

### ui/CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.16)
project(ui_lib)

# 查找 Qt6 包
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Charts QuickControls2)

# 构建静态库 ui_lib
qt_add_library(ui_lib STATIC
    src/adapters/StockListModel.cpp
    src/adapters/StrategyResultModel.cpp
    src/adapters/ChartDataAdapter.cpp
    src/widgets/CandlestickWidget.cpp
)

target_include_directories(ui_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(ui_lib PUBLIC
    Qt6::Core
    Qt6::Widgets
    Qt6::Charts
    core_lib        # 现有核心库
    analysis_lib    # 分析库
)

# 构建可执行文件 stock_ui
qt_add_executable(stock_ui
    src/main.cpp
    src/MainWindow.cpp
    src/widgets/StockListView.cpp
    src/widgets/StrategyPanel.cpp
    src/dialogs/SettingsDialog.cpp
)

target_link_libraries(stock_ui PRIVATE
    ui_lib
    Qt6::Charts
    Qt6::QuickControls2
)

# 安装规则
install(TARGETS stock_ui
    BUNDLE DESTINATION .
    RUNTIME DESTINATION bin
)
```

### cpp/CMakeLists.txt 修改

```cmake
# 在现有 cpp/CMakeLists.txt 中添加
option(BUILD_UI "Build Qt UI version" OFF)

if(BUILD_UI)
    add_subdirectory(ui)
endif()
```

---

## 🔄 数据流与交互逻辑

### 数据流设计（股票列表 → K线图表）

```
用户点击股票
    ↓
MainWindow::onStockSelected(ts_code)
    ↓
ui_lib::StockListModel (Qt 数据模型)
    → 调用 core_lib::StockManager::getStockInfo(ts_code)
    → 返回 std::vector<AnalysisResult>
    ↓
ui_lib::ChartDataAdapter::convertToQtSeries()
    → 将 AnalysisResult.candles 转换为 Qt Charts 的 QLineSeries/QCandlestickSeries
    ↓
widgets::CandlestickWidget::updateChart(series)
    → 渲染 K线 + EMA 线 + 买卖信号标记
```

### 核心交互逻辑（策略参数调整）

```cpp
// MainWindow.cpp - 策略参数面板交互
void MainWindow::onStrategyParamChanged(const QString& param, double value) {
    // 1. 更新核心库策略配置
    core::StrategyConfig config = m_strategyManager->getConfig();
    config.setParam(param.toStdString(), value);
    m_strategyManager->updateConfig(config);
    
    // 2. 重新计算当前选中股票的策略信号
    auto result = m_analysisEngine->analyze(m_currentTsCode, config);
    
    // 3. 通过信号槽更新 UI
    emit strategyResultUpdated(result);
}

// 信号槽绑定（在 MainWindow 构造函数中）
connect(m_strategyPanel, &StrategyPanel::paramChanged,
        this, &MainWindow::onStrategyParamChanged);
connect(this, &MainWindow::strategyResultUpdated,
        m_candlestickWidget, &CandlestickWidget::onResultUpdated);
```

### 关键设计点

1. **数据转换边界**：`ui_lib/adapters/` 负责将 C++ 核心库数据结构转换为 Qt 数据模型
2. **信号槽驱动**：用户操作 → 调用核心库 → 通过 Qt 信号槽更新 UI
3. **实时计算**：参数变更立即触发 `analyze()`，无需等待批量调度

---

## ⚡ 性能优化

### 关键优化策略

| 优化点 | 实现方式 |
|-------|---------|
| **数据预加载** | 启动时预加载热门股票数据到内存缓存 |
| **异步计算** | 策略参数变更时，使用 `QThreadPool` 后台计算 |
| **图表数据裁剪** | K线图表只渲染最近 200 根，滚动加载更多 |
| **信号槽优化** | 批量数据更新时，使用 `emit dataChanged(begin, end)` |

### 异步计算示例

```cpp
// 使用 QThreadPool 后台计算策略
void MainWindow::runAsyncAnalysis(const std::string& tsCode) {
    QFuture<core::AnalysisResult> future = QtConcurrent::run([this, tsCode]() {
        return m_analysisEngine->analyze(tsCode);
    });
    
    QFutureWatcher<core::AnalysisResult>* watcher = new QFutureWatcher(this);
    connect(watcher, &QFutureWatcher::finished, this, [this, watcher]() {
        auto result = watcher->result();
        emit strategyResultUpdated(result);
        watcher->deleteLater();
    });
    
    watcher->setFuture(future);
}
```

---

## 🧪 测试策略

### 测试层级

| 测试层级 | 内容 | 工具 |
|---------|------|------|
| **单元测试** | adapters 数据转换逻辑 | Qt Test + GoogleTest |
| **集成测试** | UI 与 core_lib 交互 | Qt Test 手动模拟信号 |
| **UI 自动化测试** | 关键交互路径（选股、调参、导出） | Squish（可选） |
| **性能基准** | 图表渲染帧率、数据加载耗时 | Qt Benchmark |

### 单元测试示例

```cpp
// tests/adapters/StockListModelTest.cpp
class StockListModelTest : public QObject {
    Q_OBJECT
    
private Q_SLOTS:
    void testDataConversion() {
        // 模拟核心库数据
        std::vector<core::Stock> stocks = {{"000001.SZ", "平安银行"}};
        
        // 测试适配器转换
        StockListModel model;
        model.setStocks(stocks);
        
        QCOMPARE(model.rowCount(), 1);
        QCOMPARE(model.data(model.index(0), Qt::DisplayRole).toString(), 
                 QString("平安银行"));
    }
};
```

---

## ⚠️ 错误处理

### 异常捕获策略

```cpp
// ui_lib/adapters/StockListModel.cpp
QVariant StockListModel::data(const QModelIndex& index, int role) const {
    try {
        const auto& stock = m_stocks.at(index.row());
        // 数据转换逻辑...
    } catch (const std::exception& e) {
        // 1. 记录错误日志（使用现有 logger）
        logger::error("StockListModel data access failed: {}", e.what());
        
        // 2. 返回空值，避免 UI 崩溃
        return QVariant();
    }
}

// MainWindow.cpp - 全局异常捕获
void MainWindow::setupExceptionHandler() {
    // 异常信号槽：核心库 → UI 显示错误对话框
    connect(m_coreBridge, &CoreBridge::errorOccurred,
            this, &MainWindow::showErrorDialog);
}
```

---

## 📦 部署与打包

### 平台部署

| 平台 | 打包工具 | 输出格式 |
|------|---------|---------|
| **macOS** | `macdeployqt` | `.app` bundle + `.dmg` 安装包 |
| **Windows** | `windeployqt` | 可执行文件 + NSIS 安装脚本 |
| **Linux** | AppImage / Flatpak | 单文件可执行包（可选） |

### macOS 部署示例

```bash
# 构建
cd cpp/build
cmake -DBUILD_UI=ON ..
make stock_ui

# 打包
macdeployqt stock_ui.app -dmg -qmldir=../ui/resources

# 输出：stock_ui.dmg
```

---

## 🚀 实施路线图

### Phase 1：基础框架（Week 1）

- [ ] 创建 `ui/` 目录结构
- [ ] 配置 CMake Qt 6 支持
- [ ] 实现 `MainWindow` 基础框架
- [ ] 实现 `StockListModel` 数据适配器

### Phase 2：核心功能（Week 2-3）

- [ ] 实现 `CandlestickWidget` K线图表
- [ ] 实现 `StrategyPanel` 参数配置面板
- [ ] 实现 `ChartDataAdapter` 数据转换
- [ ] 完成选股 → 图表渲染完整数据流

### Phase 3：高级功能（Week 4）

- [ ] 多窗口布局（浮动窗口 / MDI）
- [ ] 实时行情推送（可选）
- [ ] 导出功能（CSV / 图片）
- [ ] 主题切换（深色 / 浅色）

### Phase 4：测试与部署（Week 5）

- [ ] 单元测试 + 集成测试
- [ ] 性能基准测试
- [ ] 跨平台打包
- [ ] 文档完善

---

## 📚 参考资源

### Qt 6 官方文档

- [Qt 6 CMake Integration](https://doc.qt.io/qt-6/cmake-get-started.html)
- [Qt Charts Module](https://doc.qt.io/qt-6/qtcharts-index.html)
- [Qt Quick Controls 2](https://doc.qt.io/qt-6/qtquickcontrols2-index.html)

### 现有项目文档

- [cpp/DESIGN.md](../DESIGN.md) - 核心库架构
- [cpp/README.md](../README.md) - CLI 版本使用指南

---

## ✅ 设计决策记录

| 决策 | 理由 |
|-----|------|
| **选择 Qt 6** | 现代 CMake 支持、跨平台、功能强大、官方推荐 CMake 作为构建系统 |
| **直接调用核心库** | 性能最优、无网络开销、共享内存调用 |
| **ui_lib 静态库** | 方便单元测试、隔离 Qt 与核心库、降低耦合 |
| **Qt Charts 用于 K线** | 官方支持、性能可靠、易集成（优于第三方库） |

---

**下一步行动**：开始 Phase 1 实施 → 创建 `ui/` 目录结构并配置 CMake。