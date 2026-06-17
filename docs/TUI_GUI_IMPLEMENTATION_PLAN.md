# TUI 和 GUI 客户端实现计划

**日期**: 2026-06-16  
**状态**: TUI 基础框架已创建，等待依赖解决  
**负责人**: Senior Developer

---

## 📊 当前进展

### TUI 客户端 (FTXUI) - 基础框架已完成

| 组件 | 状态 | 文件 |
|------|------|------|
| **CMakeLists.txt** | ✅ 完成 | stock_tui/CMakeLists.txt |
| **main.cpp** | ✅ 完成 | stock_tui/main.cpp |
| **MainScreen** | ✅ 完成 | components/MainScreen.h/cpp |
| **AnalysisScreen** | ✅ 完成 | components/AnalysisScreen.h/cpp |
| **ConfigScreen** | ✅ 完成 | components/ConfigScreen.h/cpp |
| **LogScreen** | ✅ 完成 | components/LogScreen.h/cpp |
| **StatusScreen** | ✅ 完成 | components/StatusScreen.h/cpp |
| **MenuComponent** | ✅ 完成 | components/MenuComponent.h/cpp |
| **ProgressAdapter** | ✅ 完成 | adapters/ProgressAdapter.h/cpp |
| **LogAdapter** | ✅ 完成 | adapters/LogAdapter.h/cpp |

### GUI 客户端 (Qt) - 待实现

| 组件 | 状态 |
|------|------|
| CMakeLists.txt | ⏳ 待创建 |
| main.cpp | ⏳ 待创建 |
| MainWindow | ⏳ 待创建 |
| AnalysisWidget | ⏳ 待创建 |
| ConfigWidget | ⏳ 待创建 |

---

## 🔧 当前阻塞问题

### FTXUI 依赖下载失败

**问题**: 网络连接失败，无法从 GitHub 克隆 FTXUI 库
```
fatal: unable to access 'https://github.comArthurSonzogni/FTXUI.git/': 
SSL_ERROR_SYSCALL in connection to github.comArthurSonzogni:443
```

**解决方案**:

#### 选项 1: 手动下载 FTXUI（推荐）
```bash
# 1. 手动克隆 FTXUI 到 thirdparty 目录
cd cpp/thirdparty
git clone https://github.comArthurSonzogni/FTXUI.git ftxui

# 2. 修改 CMakeLists.txt 使用本地路径
# stock_tui/CMakeLists.txt
add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/ftxui)
```

#### 选项 2: 使用系统包管理器（macOS）
```bash
# 尝试使用 brew 安装（如果有）
brew search ftxui
```

#### 选项 3: 使用镜像源
```bash
# 使用 GitCode 或其他镜像
GIT_REPOSITORY https://gitcode.comArthurSonzogni/FTXUI.git
```

---

## 📝 TUI 客户端架构设计

### 文件结构（已实现）

```
cpp/stock_tui/
├── CMakeLists.txt          ✅ 构建配置
├── main.cpp                ✅ FTXUI 主入口
├── components/
│   ├── MainScreen.h/cpp    ✅ 主界面（概览/股票列表/快速操作）
│   ├── AnalysisScreen.h/cpp ✅ 分析界面（进度/统计）
│   ├── ConfigScreen.h/cpp  ✅ 配置界面
│   ├── LogScreen.h/cpp     ✅ 日志查看
│   ├── StatusScreen.h/cpp  ✅ 状态监控
│   └── MenuComponent.h/cpp ✅ 导航菜单
└── adapters/
    ├── ProgressAdapter.h/cpp ✅ 进度适配器
    └── LogAdapter.h/cpp     ✅ 日志适配器
```

### 核心功能

#### 1. MainScreen（主界面）
- **概览标签**: 系统统计、分析状态、进度条
- **股票列表标签**: 显示股票列表、筛选、搜索
- **快速操作标签**: 开始分析、刷新数据、查看日志按钮

#### 2. AnalysisScreen（分析界面）
- 实时进度显示（gauge 进度条）
- 完成数/失败数统计
- 开始/停止控制按钮

#### 3. ProgressAdapter（进度适配器）
- 将 `stock_core::ProgressCallback` 适配到 FTXUI
- 提供回调函数给核心库注册

#### 4. LogAdapter（日志适配器）
- 捕获核心库日志并显示在 TUI
- 支持滚动查看、清空日志

---

## 🎨 GUI 客户端设计（待实现）

### 文件结构（建议）

```
cpp/stock_gui/
├── CMakeLists.txt          # Qt 构建配置
├── main.cpp                # Qt 主入口
├── ui/
│   ├── MainWindow.h/cpp    # 主窗口
│   ├── AnalysisWidget.h/cpp # 分析界面
│   ├── ConfigWidget.h/cpp  # 配置界面
│   ├── LogWidget.h/cpp     # 日志界面
│   └── StatusWidget.h/cpp  # 状态界面
└── adapters/
    ├── ProgressAdapter.h/cpp # Qt 进度适配器
    └── LogAdapter.h/cpp      # Qt 日志适配器
```

### 核心功能

#### 1. MainWindow（主窗口）
- 顶部菜单栏（文件/分析/配置/帮助）
- 左侧导航树
- 中央内容区域（可切换界面）
- 底部状态栏

#### 2. AnalysisWidget（分析界面）
- QProgressBar 进度显示
- QTableView 结果表格
- QChartView 图表展示（Qt Charts）
- 开始/停止/导出按钮

#### 3. 图表功能（Qt Charts）
- 股价走势图
- 技术指标图（MA, MACD, RSI）
- 分析结果统计图

---

## 🚀 实施建议

### 优先级 1: 解决 FTXUI 依赖问题

**立即执行**:
1. 手动下载 FTXUI 到 `thirdparty/ftxui`
2. 修改 CMakeLists.txt 使用本地路径
3. 编译验证 TUI 客户端

### 优先级 2: 完善 TUI 组件

**后续执行**:
1. 实现股票列表显示（使用 ftxui::Table）
2. 实现实时日志滚动显示
3. 添加键盘快捷键支持
4. 美化 UI 样式（颜色、边框）

### 优先级 3: 实现 GUI 客户端

**最后执行**:
1. 创建 Qt 项目基础框架
2. 实现主窗口和基础界面
3. 集成 Qt Charts 图表功能
4. 添加数据导出功能（CSV/PDF）

---

## 📦 依赖管理建议

### FTXUI（TUI 客户端）

**推荐方案**: 手动克隆到 thirdparty
```bash
cd cpp/thirdparty
git clone --depth=1 https://github.comArthurSonzogni/FTXUI.git ftxui
```

**优点**:
- 网络问题可控
- 版本固定，不依赖实时下载
- 构建速度快

### Qt（GUI 客户端）

**系统安装**: macOS 使用 Homebrew
```bash
brew install qt
```

**CMake 配置**:
```cmake
find_package(Qt6 COMPONENTS Core Widgets Charts REQUIRED)
```

---

## 🧪 测试计划

### TUI 测试项

| 测试项 | 验证方法 |
|--------|----------|
| 启动界面 | 运行 `stock_tui` 查看主界面 |
| 导航菜单 | 按键切换不同界面 |
| 数据加载 | 检查股票列表是否显示 |
| 分析功能 | 点击开始分析，观察进度条 |
| 日志显示 | 检查日志滚动是否正常 |

### GUI 测试项

| 测试项 | 验证方法 |
|--------|----------|
| 启动窗口 | 运行 `stock_gui` 查看主窗口 |
| 图表显示 | 检查股价走势图是否正常 |
| 数据导出 | 导出 CSV 检查格式 |
| 配置保存 | 修改配置，重启验证 |

---

## 📚 相关文档

- 架构设计: `docs/plans/2026-06-14-architecture-refactor-design.md`
- 编译修复: `docs/COMPILATION_FIX_REPORT.md`
- 错误诊断: `docs/COMPILATION_ERROR_QUICK_REFERENCE.md`

---

## 🔄 下次实施步骤

1. **解决依赖**: 手动下载 FTXUI 到 thirdparty
2. **编译验证**: 编译 TUI 客户端确保基础框架正常
3. **功能完善**: 实现股票列表、日志滚动等核心功能
4. **GUI 实现**: 创建 Qt GUI 客户端基础框架
5. **集成测试**: 验证所有客户端功能完整性

---

**更新**: 2026-06-16 00:50  
**下一步**: 解决 FTXUI 依赖问题后继续编译