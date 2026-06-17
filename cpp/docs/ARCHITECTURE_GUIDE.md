# Stock for C++ - 架构重构指南

**版本**: 2.0.0  
**日期**: 2026-06-14  
**状态**: 核心框架已完成，业务逻辑待迁移  

---

## 📋 架构说明

本项目采用**统一核心库 + 三种客户端**架构：

```
stock_core (核心业务库)
    ├── stock_exe (命令行执行客户端) - ✅ 已完成
    ├── stock_tui (FTXUI 终端界面) - ⏳ 待实现
    └── stock_gui (Qt 图形界面) - ⏳ 待重构
```

---

## 🎯 已完成的工作

### ✅ 核心库框架 (stock_core)

- 统一接口设计 (`stock_core.h`)
- 核心上下文实现 (`StockCoreContext`)
- 完善的初始化流程
- 资源清理机制
- 进度回调支持

### ✅ exe 客户端 (stock_exe)

- 命令行参数解析
- 进度条可视化
- 信号处理（优雅关闭）
- 基础业务流程

### ✅ 构建系统

- CMake 模块化构建
- 构建脚本
- 版本管理

---

## 🚀 快速开始

### 1. 构建 exe 客户端

```bash
cd /Users/kirito/Documents/projects/stock_for_cpp/cpp

# 使用构建脚本
./scripts/build_exe.sh

# 或手动构建
mkdir build_exe && cd build_exe
cmake -DBUILD_EXE=ON ..
make -j4
```

### 2. 运行测试

```bash
cd build_exe

# 单次执行（简化版）
./stock_exe --once

# 定时模式
./stock_exe --time 20:00

# 显示帮助
./stock_exe --help
```

---

## ⏳ 待完成的工作

### 🔴 优先级 1: 完善业务逻辑迁移

**目标**: 将 `main.cpp` 中的完整分析流程迁移到 `StockCoreContext::performDailyAnalysis()`

**步骤**:

1. **迁移 analyzeStock() 函数**
   - 源码位置: `cpp/main.cpp` 第 329-530 行
   - 目标位置: `stock_core/StockCoreContext.cpp` 中添加私有方法
   
2. **迁移 performBatchAnalysis() 函数**
   - 源码位置: `cpp/main.cpp` 第 584-700 行
   - 目标位置: `stock_core/StockCoreContext::performDailyAnalysis()`

3. **迁移辅助函数**
   - `calculateAnalysisDate()` - ✅ 已完成
   - `updatePhase2Progress()` - 待迁移
   - `executeProgressUpdate()` - 待迁移

4. **实现进度更新**
   - 在 `performDailyAnalysis()` 中调用进度回调
   - 实时更新进度到客户端

**迁移示例**:

```cpp
// stock_core/StockCoreContext.cpp

void StockCoreContext::analyzeStock(const core::Stock& stock) {
    // 从 main.cpp 的 analyzeStock 函数迁移代码
    LOG_INFO("分析股票: " << stock.ts_code);
    
    // 对每个频率进行分析
    const std::vector<std::string> frequencies = {"d", "w", "m"};
    
    for (const auto& freq : frequencies) {
        // 获取数据
        auto data = dataSource_->getQuoteData(stock.ts_code, "", "", freq);
        
        // 使用策略分析
        auto results = strategyManager_->analyzeAll(stock.ts_code, data);
        
        // 保存结果...
    }
}

void StockCoreContext::performDailyAnalysis(const std::vector<core::Stock>& stocks) {
    // 完整实现
    LOG_INFO("开始分析 " << stocks.size() << " 只股票");
    
    int total = stocks.size();
    int completed = 0;
    int failed = 0;
    
    for (const auto& stock : stocks) {
        try {
            analyzeStock(stock);
            completed++;
            
            if (progressCallback_) {
                progressCallback_(total, completed, failed, stock.ts_code);
            }
        } catch (const std::exception& e) {
            failed++;
            LOG_ERROR("分析失败: " << stock.ts_code << " - " << e.what());
        }
    }
}
```

### 🟡 优先级 2: 实现 TUI 客户端

**步骤**:

1. **集成 FTXUI 库**
   ```bash
   cd cpp/thirdparty
   git clone https://github.com ArthurSonzogni/FTXUI.git ftxui
   ```

2. **创建 TUI 组件**
   - `stock_tui/components/MainScreen.cpp`
   - `stock_tui/components/AnalysisScreen.cpp`
   - `stock_tui/components/ConfigScreen.cpp`

3. **更新 CMakeLists.txt**
   ```cmake
   add_subdirectory(thirdparty/ftxui)
   add_subdirectory(stock_tui)
   ```

### 🟢 优先级 3: 重构 GUI 客户端

**步骤**:

1. 调整 `ui` 目录为 `stock_gui`
2. 更新 CMakeLists.txt 链接 `stock_core`
3. 测试 Qt 功能

---

## 📊 架构优势

| 特性 | 旧架构 | 新架构 |
|------|--------|--------|
| 业务逻辑复用 | ❌ 紧耦合 | ✅ 统一核心库 |
| 多客户端支持 | ❌ 无 | ✅ 三种客户端 |
| 灵活部署 | ❌ 单一 | ✅ 服务器/桌面 |
| 易于测试 | ❌ 集成测试 | ✅ 单元测试 |
| 易于扩展 | ❌ 需重构 | ✅ 链接核心库 |

---

## 🔧 构建选项

```bash
# 只构建核心库
cmake -DBUILD_CORE=ON -DBUILD_TUI=OFF -DBUILD_GUI=OFF -DBUILD_EXE=OFF ..

# 构建 exe 客户端（默认）
cmake -DBUILD_EXE=ON ..

# 构建 TUI 客户端
cmake -DBUILD_TUI=ON ..

# 构建 GUI 客户端
cmake -DBUILD_GUI=ON ..

# 构建全部
cmake -DBUILD_CORE=ON -DBUILD_TUI=ON -DBUILD_GUI=ON -DBUILD_EXE=ON ..

# 兼容旧版（临时）
cmake -DBUILD_LEGACY=ON ..
```

---

## 📁 文件结构

```
cpp/
├── stock_core/          # 核心库
│   ├── stock_core.h
│   ├── stock_core.cpp
│   ├── StockCoreContext.h
│   ├── StockCoreContext.cpp
│   └── CMakeLists.txt
│
├── stock_exe/           # exe 客户端 ✅
│   ├── main.cpp
│   ├── CliOptions.h/cpp
│   ├── CliProgress.h/cpp
│   ├── SignalHandler.h/cpp
│   └── CMakeLists.txt
│
├── stock_tui/           # TUI 客户端 ⏳
│   └── (待实现)
│
├── stock_gui/           # GUI 客户端 ⏳
│   └── (待重构)
│
├── scripts/             # 构建脚本
│   └ build_exe.sh
│   └ build_tui.sh (待创建)
│   └ build_gui.sh (待创建)
│
├── docs/                # 文档
│   ├── plans/2026-06-14-architecture-refactor-design.md
│   └ ARCHITECTURE_GUIDE.md
│
└── CMakeLists.txt       # 主构建文件
```

---

## 🛠️ 技术栈

### 核心库
- C++17
- spdlog (日志)
- sqlpp11 (数据库)
- nlohmann_json (JSON)
- httplib (HTTP)
- ta-lib (技术分析)

### exe 客户端
- 命令行参数解析
- 进度条显示
- 信号处理

### TUI 客户端
- FTXUI (现代终端 UI)
- 组件化设计
- 键盘导航

### GUI 客户端
- Qt6
- Charts
- Widgets

---

## 📝 迁移注意事项

### 1. 头文件依赖

核心库需要包含所有模块的头文件：

```cpp
// StockCoreContext.h
#include "../config/Config.h"
#include "../log/Logger.h"
#include "../data/database/Connection.h"
#include "../network/DataSourceFactory.h"
#include "../core/StrategyManager.h"
#include "../scheduler/Scheduler.h"
```

### 2. 单例模式

保持单例模式的一致性：

```cpp
// 获取实例
auto& context = StockCoreContext::getInstance();

// 初始化
context.initialize(".env");

// 访问模块
auto& config = context.getConfig();
```

### 3. 异常处理

所有模块访问都要检查初始化状态：

```cpp
if (!initialized_) {
    throw std::runtime_error("核心库未初始化");
}
```

### 4. 进度回调

使用回调机制通知客户端：

```cpp
if (progressCallback_) {
    progressCallback_(total, completed, failed, status);
}
```

---

## 🎓 学习建议

### 对于团队成员

1. **阅读架构设计文档**
   - `docs/plans/2026-06-14-architecture-refactor-design.md`
   
2. **理解核心库接口**
   - `stock_core/stock_core.h`
   - `stock_core/StockCoreContext.h`

3. **分析 exe 客户端实现**
   - 学习如何使用核心库 API
   - 理解进度回调机制

4. **参考 main.cpp**
   - 理解原有业务逻辑
   - 学习迁移方法

5. **实践迁移**
   - 先迁移简单函数
   - 再迁移复杂逻辑
   - 最后测试验证

---

## 📞 技术支持

如有疑问，请参考：
- 架构设计文档
- 本指南
- 或联系 Senior Developer Team

---

**维护者**: Senior Developer Team  
**更新时间**: 2026-06-14