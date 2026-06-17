# CSV 数据导入功能实现计划

## 📊 当前进度

### ✅ 已完成

1. **CSV 格式规范**
   - 文件：`cpp/data/sample_stocks.csv`
   - 格式：`ts_code,name,industry,list_date,market`
   - 示例数据：10 只股票（贵州茅台、平安银行、万科A 等）

2. **CSV 解析器**
   - 头文件：`cpp/util/CSVParser.h`
   - 实现文件：`cpp/util/CSVParser.cpp`
   - 功能：
     - 解析 CSV 文件并返回 `std::vector<core::Stock>`
     - 支持字段验证（ts_code 格式检查）
     - 宽松模式（跳过错误行继续解析）

3. **stock_core API 接口定义**
   - 文件：`cpp/stock_core/stock_core.h`
   - 新增 API：`int importStocksFromCSV(const std::string& csvFilePath)`
   - 返回导入的股票数量

---

## 🚧 待完成任务

### 任务 1: 实现 stock_core.cpp 的 API（关键）

**文件位置**：`cpp/stock_core/stock_core.cpp`

**需要在 api 命名空间添加实现**：

```cpp
int importStocksFromCSV(const std::string& csvFilePath) {
    auto* context = getContext();
    if (!context) {
        throw std::runtime_error("核心库未初始化");
    }
    
    // 1. 使用 CSVParser 解析 CSV 文件
    auto stocks = util::CSVParser::parseStockList(csvFilePath);
    
    // 2. 批量保存到数据库（使用 StockDao）
    auto& stockDao = context->getStockDao();
    int importedCount = 0;
    
    for (const auto& stock : stocks) {
        try {
            if (stockDao.save(stock)) {
                importedCount++;
            }
        } catch (const std::exception& e) {
            // 记录失败但继续导入下一只股票
            LOG_WARN("导入股票失败: " << stock.ts_code << " - " << e.what());
        }
    }
    
    LOG_INFO("CSV 导入完成: 成功 " << importedCount << " 只，总计 " << stocks.size() << " 只");
    
    return importedCount;
}
```

**依赖项**：
- 需要包含头文件：`#include "../util/CSVParser.h"`
- 需要 StockCoreContext 提供 `getStockDao()` 方法（检查是否已存在）
- 需要 StockDao 提供 `save()` 方法（检查是否已存在）

---

### 任务 2: TUI 界面添加导入功能

**文件位置**：`cpp/stock_tui/main.cpp`

**添加快捷键 [I] 处理**：

```cpp
// 在 CatchEvent 的事件处理中添加
if (event == Event::Character('i') || event == Event::Character('I')) {
    // 导入 CSV 数据
    try {
        int imported = stock_core::api::importStocksFromCSV("data/sample_stocks.csv");
        main_screen->RefreshData();  // 刷新显示导入后的数据
        // 显示成功消息（可选）
        selected_menu = 0;  // 切换到主界面查看导入结果
        main_screen->SetCurrentMenu(0);
        return true;
    } catch (const std::exception& e) {
        // 显示错误消息（可选）
        std::cerr << "[ERROR] CSV 导入失败: " << e.what() << std::endl;
        return true;
    }
}
```

**更新快捷键提示栏**：

```cpp
// 第 134 行
text("快捷键: [A] 分析  [R] 刷新  [L] 日志  [I] 导入  [Q] 退出  [↑↓] 导航  [Enter] 执行")
```

---

### 任务 3: MainScreen 更新导入提示

**文件位置**：`cpp/stock_tui/components/MainScreen.cpp`

**在 RenderQuickActions() 添加导入提示**：

```cpp
Element MainScreen::RenderQuickActions() {
    return vbox({
        text("快速操作") | bold | color(Color::Magenta),
        separator(),
        vbox({
            text("  [A] 开始批量分析") | color(Color::Green),
            text("      → 分析所有股票的技术指标"),
            separator(),
            text("  [R] 刷新数据") | color(Color::Yellow),
            text("      → 重新加载股票列表和分析状态"),
            separator(),
            text("  [I] 导入 CSV 数据") | color(Color::Cyan),  // 新增
            text("      → 从 CSV 文件导入股票列表"),
            text("      → 文件位置: data/sample_stocks.csv"),
            separator(),
            text("  [L] 查看实时日志") | color(Color::Cyan),
            text("      → 显示 logs/app.log 最新内容"),
            separator(),
            text("  [Q] 退出系统") | color(Color::Red),
            text("      → 安全保存数据并退出"),
        }),
    });
}
```

---

### 任务 4: CMake 配置更新（关键）

**文件位置**：`cpp/CMakeLists.txt`（或 stock_core/CMakeLists.txt）

**需要添加 CSVParser 到编译列表**：

```cmake
# 在 stock_core 的源文件列表中添加
set(STOCK_CORE_SOURCES
    # ... 现有文件
    util/CSVParser.cpp  # 新增
)

# 或者在 util 目录创建独立的库
add_library(util_lib STATIC
    util/CSVParser.cpp
)

target_link_libraries(stock_core PUBLIC util_lib)
```

---

## 🧪 验证步骤

### 步骤 1: 编译验证

```bash
cd cpp/build
cmake --build . --target stock_core 2>&1 | tail -10
cmake --build . --target stock_tui 2>&1 | tail -10
```

**预期结果**：
- 无编译错误
- CSVParser.cpp 成功编译
- stock_core 和 stock_tui 链接成功

---

### 步骤 2: 功能验证

```bash
cd cpp/build/stock_tui
./stock_tui
```

**测试流程**：
1. 系统启动 → 显示降级模式提示（无数据）
2. 按 [I] → 触发 CSV 导入
3. 再次按 [R] → 刷新数据，应显示导入的股票数量
4. 切换到"股票列表"标签 → 应显示导入的 10 只股票

**预期结果**：
- 导入成功：显示"成功导入 10 只股票"
- 数据刷新：totalStocks_ 从 0 变为 10
- 数据持久化：重启程序后数据仍在（SQLite 存储）

---

## 🔍 潜在问题排查

### 问题 1: StockDao 的 save() 方法不存在

**检查文件**：`cpp/data/StockDao.h` 和 `cpp/data/StockDao.cpp`

**如果不存在，需要实现**：

```cpp
// StockDao.h
bool save(const core::Stock& stock);

// StockDao.cpp
bool StockDao::save(const core::Stock& stock) {
    try {
        // SQL 插入或更新逻辑
        std::string sql = "INSERT OR REPLACE INTO stocks (ts_code, name, industry, list_date, market) "
                         "VALUES (?, ?, ?, ?, ?)";
        
        // 使用 SQLite 执行 SQL（参考现有 DAO 实现）
        // ...
        
        return true;
    } catch (...) {
        return false;
    }
}
```

---

### 问题 2: getStockDao() 方法不存在

**检查文件**：`cpp/stock_core/StockCoreContext.h`

**如果不存在，需要添加**：

```cpp
// StockCoreContext.h
public:
    data::StockDao& getStockDao();

// StockCoreContext.cpp
data::StockDao& StockCoreContext::getStockDao() {
    return *stockDao_;
}
```

---

### 问题 3: 数据库表 stocks 不存在

**检查文件**：`cpp/data/init.sql`

**需要确认表结构**：

```sql
CREATE TABLE IF NOT EXISTS stocks (
    ts_code TEXT PRIMARY KEY,
    name TEXT,
    industry TEXT,
    list_date TEXT,
    market TEXT
);
```

**如果不存在，添加到 init.sql 并重新初始化数据库**：

```bash
cd cpp/data
sqlite3 stock.db < init.sql
```

---

## 📝 完整实现检查清单

✅ **基础文件**：
- [ ] `cpp/data/sample_stocks.csv` - CSV 示例数据
- [ ] `cpp/util/CSVParser.h` - 解析器头文件
- [ ] `cpp/util/CSVParser.cpp` - 解析器实现

✅ **核心 API**：
- [ ] `cpp/stock_core/stock_core.h` - API 接口定义（已添加 importStocksFromCSV）
- [ ] `cpp/stock_core/stock_core.cpp` - API 实现（待添加）
- [ ] 包含头文件：`#include "../util/CSVParser.h"`

✅ **依赖检查**：
- [ ] `cpp/data/StockDao.h` - 检查 save() 方法是否存在
- [ ] `cpp/stock_core/StockCoreContext.h` - 检查 getStockDao() 方法是否存在
- [ ] `cpp/data/init.sql` - 检查 stocks 表是否存在

✅ **TUI 集成**：
- [ ] `cpp/stock_tui/main.cpp` - 快捷键 [I] 处理
- [ ] `cpp/stock_tui/components/MainScreen.cpp` - 快捷键提示更新

✅ **CMake 配置**：
- [ ] `cpp/CMakeLists.txt` 或 `cpp/stock_core/CMakeLists.txt` - 添加 CSVParser 编译

---

## 🚀 下一步工作流程

### Phase 1: 依赖检查与补全（30 分钟）

1. 检查 StockDao 是否有 save() 方法
2. 检查 StockCoreContext 是否有 getStockDao() 方法
3. 检查数据库表 stocks 是否存在
4. 补全缺失的依赖实现

---

### Phase 2: API 实现与编译（20 分钟）

1. 在 stock_core.cpp 实现 importStocksFromCSV()
2. 更新 CMakeLists.txt 添加 CSVParser
3. 编译验证（stock_core + stock_tui）

---

### Phase 3: TUI 集成与测试（15 分钟）

1. 在 main.cpp 添加快捷键 [I]
2. 更新快捷键提示栏
3. 运行测试（导入 + 刷新 + 查看结果）

---

### Phase 4: 文档与验证（10 分钟）

1. 创建 CSV 导入功能使用指南
2. 验证导入流程（10 只股票全部成功）
3. 验证数据持久化（重启后数据仍在）

---

## 📄 参考文档

- **CSV 解析器设计**：`cpp/util/CSVParser.h`（已实现）
- **stock_core API**：`cpp/stock_core/stock_core.h`（已定义接口）
- **TUI 快捷键实现**：`cpp/stock_tui/main.cpp`（已有架构，需添加 [I]）
- **数据库设计**：`cpp/data/init.sql`（需检查 stocks 表）

---

**文档版本**: v1.0.0  
**创建时间**: 2026-06-18  
**作者**: Senior Developer (高级开发工程师)  
**状态**: 🚧 进行中（已完成基础架构，待实现核心 API + TUI 集成）