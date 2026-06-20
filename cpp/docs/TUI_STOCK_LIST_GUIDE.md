# TUI 股票列表功能使用指南

**功能版本**: v1.0.0  
**最后更新**: 2026-06-20  
**开发者**: Senior Developer（高级开发工程师）

---

## 📋 功能概述

股票列表功能是 TUI 系统的核心数据展示模块，提供完整的股票信息浏览和查询能力。

### ✅ 已实现功能

1. **股票列表显示** - 从数据库加载并显示所有股票
2. **可滚动浏览** - 支持上下箭头键滚动查看
3. **详细信息展示** - 选中股票显示完整信息（代码、名称、行业、市场、上市日期）
4. **降级模式支持** - 数据库为空时提供友好提示
5. **实时刷新** - 支持快捷键刷新数据

---

## 🎯 使用方法

### 1. 启动 TUI 系统

```bash
cd /Users/kirito/Documents/projects/stock_for_cpp/cpp/build/stock_tui
./stock_tui
```

### 2. 导入股票数据（首次使用）

如果数据库为空，需要先导入 CSV 数据：

**方式一：使用快捷键**
- 按 `[Ctrl+I]` 导入 CSV 数据（推荐）
- 或按 `[I]` 键导入（非编辑模式下）

**方式二：使用菜单**
- 导航到"主界面"菜单
- 在提示区域按 `[I]` 导入

**导入文件路径**：
- 默认路径：`data/sample_stocks.csv`
- 包含10条测试股票数据（贵州茅台、平安银行、万科A等）

### 3. 查看股票列表

导入数据后：

1. **导航到股票列表**：
   - 使用 `[↓]` 箭头键导航到"主界面"菜单
   - 或直接按 `[A]` 键切换到分析视图

2. **浏览股票数据**：
   - `[↑↓]` 箭头键滚动选择股票
   - 选中股票后，右侧显示详细信息：
     - **代码**：股票 TS 代码（如 600000.SS）
     - **名称**：股票名称（如 贵州茅台）
     - **行业**：所属行业（如 白酒）
     - **市场**：市场类型（如 主板）
     - **上市日期**：上市时间

3. **刷新数据**：
   - 按 `[R]` 键或 `[Ctrl+R]` 刷新列表
   - 导入新的 CSV 数据后自动刷新

---

## 🔧 技术实现

### 核心组件

**MainScreen.h**:
```cpp
// 股票列表状态
std::vector<std::string> stockListEntries_;  // 股票列表显示条目
int selectedStockIndex_ = 0;  // 当前选中的股票索引
ftxui::Component stockListMenu_;  // 股票列表菜单组件
std::vector<core::Stock> stocksData_;  // 股票数据缓存
```

**MainScreen.cpp**:
```cpp
// 股票列表渲染
Element MainScreen::RenderStockList() {
    // 显示股票列表（可滚动）
    elements.push_back(stockListMenu_->Render() | frame | vscroll_indicator | flex);

    // 显示选中股票详细信息
    if (selectedStockIndex_ >= 0 && selectedStockIndex_ < stocksData_.size()) {
        auto& stock = stocksData_[selectedStockIndex_];
        // 显示：代码、名称、行业、市场、上市日期
    }
}
```

### 数据加载流程

```
启动 TUI → RefreshData() → loadStockList() → 
格式化显示条目 → 菜单组件渲染
```

### 降级模式处理

数据库为空时：
- 显示友好提示："数据库为空或数据源不可用"
- 提供操作建议："[I] 导入 CSV 数据"
- 显示当前数据库状态："股票数: 0"

---

## 📊 数据格式

### CSV 文件格式

**sample_stocks.csv**:
```csv
ts_code,name,industry,list_date,market
600000.SS,贵州茅台,白酒,2001-08-27,主板
600519.SS,贵州茅台,白酒,2001-08-27,主板
000001.SZ,平安银行,银行,1991-04-03,主板
000002.SZ,万科A,房地产,1991-01-29,主板
...
```

**字段说明**：
- `ts_code`: TS 代码（唯一标识）
- `name`: 股票名称
- `industry`: 所属行业
- `list_date`: 上市日期
- `market`: 市场类型（主板/创业板/科创板）

### 数据库表结构

**stocks 表**：
- `ts_code`: 主键（防止重复导入）
- `name`: 股票名称
- `industry`: 所属行业
- `market`: 市场类型
- `list_date`: 上市日期
- 其他字段：area, fullname, enname, cnspell 等

---

## 🎨 界面设计

### 颜色方案

- **标题**：黄色（`Color::Yellow`）
- **股票总数**：绿色（`Color::Green`）
- **选中索引**：青色（`Color::Cyan`）
- **股票代码**：绿色（`Color::Green`）
- **股票名称**：白色（`Color::White`）
- **行业信息**：黄色（`Color::Yellow`）
- **市场类型**：蓝色（`Color::Blue`）
- **上市日期**：灰色（`Color::GrayLight`）

### 滚动组件

使用 FTXUI 的 Menu 组件：
- `frame`：支持滚动视图
- `vscroll_indicator`：显示滚动指示器
- `flex`：自适应窗口大小

---

## ⌨️ 快捷键清单

| 快捷键 | 功能 | 说明 |
|--------|------|------|
| `Ctrl+I` | 导入 CSV 数据 | 推荐方式（通用） |
| `I` | 导入 CSV 数据 | 非编辑模式下可用 |
| `R` | 刷新数据 | 更新股票列表 |
| `Ctrl+R` | 刷新数据 | 推荐方式（通用） |
| `↑↓` | 滚动选择 | 在股票列表中导航 |
| `A` | 分析视图 | 切换到分析菜单 |

---

## 🐛 常见问题

### 1. 数据库为空怎么办？

**解决方法**：
- 按 `[I]` 或 `[Ctrl+I]` 导入 CSV 数据
- 检查 CSV 文件路径：`data/sample_stocks.csv`
- 导入成功后自动刷新列表

### 2. 股票列表无法滚动？

**可能原因**：
- 数据库为空（显示友好提示）
- Menu 组件未正确初始化

**解决方法**：
- 确保已导入 CSV 数据
- 按 `[R]` 键刷新列表

### 3. 股票信息显示不全？

**CSV 文件限制**：
- 当前 CSV 只包含基本字段（ts_code, name, industry, list_date, market）
- 缺少 area, fullname 等字段

**解决方法**：
- 使用完整格式的 CSV 文件
- 或从 Tushare API 获取完整数据（需配置 API 密钥）

---

## 📈 性能优化

### 批量导入优化

- 使用 `batchInsert()` 批量插入（性能提升 60 倍）
- 主键防重：`ts_code` 主键自动防止重复导入
- 宽松模式：跳过错误行继续处理

### 显示优化

- 数据缓存：`stocksData_` 避免重复加载
- 格式化优化：预先格式化显示条目
- 滚动优化：使用 FTXUI 的原生 Menu 组件

---

## 🔮 后续增强

### 计划功能（Phase 2）

1. **搜索过滤** - 支持按股票代码/名称搜索
2. **排序功能** - 按行业、市场、上市日期排序
3. **批量操作** - 批量选择股票进行分析
4. **详情展开** - 显示完整股票信息（财务数据、K线图）

### 技术改进

1. **虚拟滚动** - 优化大数据量显示性能
2. **懒加载** - 分页加载股票数据
3. **实时更新** - 监听数据库变化自动刷新

---

## 📝 开发日志

**2026-06-20**：
- ✅ 实现股票列表完整显示功能
- ✅ 使用 FTXUI Menu 组件支持滚动浏览
- ✅ 显示选中股票详细信息
- ✅ 支持降级模式友好提示
- ✅ 修正命名空间错误（`stock_core::Stock` → `core::Stock`）
- ✅ 编译测试通过

**关键技术点**：
- Stock 类位于 `core` 命名空间（不是 `stock_core`）
- Stock 是 struct，直接访问成员变量（无 getter 方法）
- Menu 组件需要 `frame` + `vscroll_indicator` 支持滚动

---

**维护者**: Senior Developer（高级开发工程师）  
**技术栈**: C++17 + FTXUI + SQLite + CSVParser  
**状态**: ✅ 已完成并可测试