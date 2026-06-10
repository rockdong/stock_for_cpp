# Stock for C++ Qt UI 模块

## 📋 概述

Qt 6 GUI 版本，提供本地桌面股票分析终端体验。

## 🛠️ 构建要求

- Qt 6.2+（推荐 Qt 6.4 或更高）
- CMake 3.16+
- C++17 编译器

## 🚀 构建步骤

### 1. 安装 Qt 6

**macOS (Homebrew)**:
```bash
brew install qt@6
```

**Windows**:
- 从 [Qt 官网](https://www.qt.io/download) 下载 Qt 6 安装包
- 或使用 MSVC + vcpkg: `vcpkg install qt6-base qt6-charts`

**Linux (Ubuntu/Debian)**:
```bash
sudo apt install qt6-base-dev qt6-charts-dev
```

### 2. 配置 CMake

```bash
cd cpp
mkdir build-ui && cd build-ui
cmake -DBUILD_QT_UI=ON ..
```

**Qt 路径指定（如果 CMake 无法自动找到）**:
```bash
cmake -DBUILD_QT_UI=ON -DCMAKE_PREFIX_PATH=/path/to/Qt/6.x.x ..
```

**macOS Homebrew 示例**:
```bash
cmake -DBUILD_QT_UI=ON -DCMAKE_PREFIX_PATH=$(brew --prefix qt@6) ..
```

### 3. 构建

```bash
make stock_ui
```

### 4. 运行

```bash
./stock_ui
```

## 📁 模块结构

```
ui/
├── CMakeLists.txt        # Qt 构建配置
├── README.md             # 本文档
├── src/
│   ├── main.cpp          # Qt 应用入口
│   ├── MainWindow.h/cpp  # 主窗口
│   ├── adapters/         # 数据适配层
│   │   └── StockListModel.h/cpp
│   ├── widgets/          # 自定义控件（Phase 2）
│   └── dialogs/          # 配置对话框（Phase 3）
└── resources/
    ├── icons.qrc         # 图标资源
    └── main.qss          # 样式表
```

## 🔧 当前状态（Phase 1）

✅ 已完成：
- 目录结构创建
- CMake 配置
- MainWindow 基础框架
- StockListModel 数据适配器

🚧 待实现（Phase 2）：
- K线图表组件 (CandlestickWidget)
- 策略参数面板 (StrategyPanel)
- ChartDataAdapter 数据转换
- 核心库集成（链接 core_lib）

## 📖 相关文档

- [设计文档](../docs/plans/2026-06-10-qt-ui-version-design.md)
- [Qt 6 CMake 集成](https://doc.qt.io/qt-6/cmake-get-started.html)
- [Qt Charts 文档](https://doc.qt.io/qt-6/qtcharts-index.html)