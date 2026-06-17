# Stock TUI 交互功能完整指南

## 📊 系统概览

**Stock Analysis System TUI** 是一个基于 FTXUI 构建的终端交互式股票分析系统，提供完整的用户交互体验。

---

## 🎯 界面布局

### 主界面结构

```
┌─────────────────────────────────────────────────────────────────────┐
│                Stock Analysis System v1.0.0                          │
│                     (终端交互模式)                                    │
├─────────────────────────────────────────────────────────────────────┤
│  [左侧导航菜单]        [右侧主内容区]                                 │
│  ┌──────────────┐    ┌────────────────────────────────────────────┐│
│  │ 导航菜单     │    │ 系统概览                                     ││
│  ├──────────────┤    ├────────────────────────────────────────────┤│
│  │ ➤ 主界面     │    │ [股票统计]  │  [分析状态]                   ││
│  │   分析       │    │ 总数: 0     │  最后分析: 未分析             ││
│  │   配置       │    │ 已分析: 0   │  进度: 0/0                    ││
│  │   日志       │    │ 失败: 0     │  状态: 降级模式               ││
│  │   状态       │    └────────────────────────────────────────────┘│
│  │   退出       │    │ 提示: 配置 API_KEY 启用完整功能             ││
│  └──────────────┘    └────────────────────────────────────────────┘│
├─────────────────────────────────────────────────────────────────────┤
│  快捷键: [A] 分析  [R] 刷新  [L] 日志  [Q] 退出  [↑↓] 导航  [Enter]  │
└─────────────────────────────────────────────────────────────────────┘
```

---

## 🔧 交互功能说明

### 1. 导航菜单操作

**操作方式**: 使用 `↑` 和 `↓` 箭头键切换菜单项，按 `Enter` 执行

**菜单项说明**:

| 菜单项 | 功能说明 | 当前实现状态 |
|--------|---------|-------------|
| **主界面** | 系统概览 + 标签切换（概览/股票列表/快速操作） | ✅ 已实现 |
| **分析** | 批量分析股票数据 | ⏳ 待实现（需数据源） |
| **配置** | 系统配置管理 | ⏳ 待实现 |
| **日志** | 实时日志查看 | ⏳ 待实现 |
| **状态** | 系统运行状态监控 | ⏳ 待实现 |
| **退出** | 安全退出系统 | ✅ 已实现 |

### 2. 快捷键功能

| 快捷键 | 功能 | 实现状态 |
|--------|------|---------|
| **[A]** | 开始批量分析 | ⏳ 待实现 |
| **[R]** | 刷新数据（重新加载股票列表） | ✅ 已实现 |
| **[L]** | 查看实时日志 | ⏳ 待实现 |
| **[Q]** | 安全退出系统 | ✅ 已实现 |
| **[Esc]** | 返回上一级 / 退出 | ✅ 已实现 |
| **[↑↓]** | 菜单导航 | ✅ 已实现 |
| **[Enter]** | 执行当前菜单项 | ✅ 已实现 |

### 3. 标签切换（主界面）

在"主界面"菜单下，有 3 个子标签：

1. **概览**: 显示系统整体状态（股票统计、分析进度）
2. **股票列表**: 显示股票数据列表（待数据源启用）
3. **快速操作**: 快捷操作入口（分析、刷新、日志）

**切换方式**: 使用鼠标点击或键盘焦点切换（待完善）

---

## 🛠️ 技术实现

### 核心组件结构

```cpp
// main.cpp - 主程序入口
class MainScreen {
    // 组件生命周期管理
    std::unique_ptr<MainScreen> main_screen;
    auto menu = stock_tui::CreateMenu(main_screen);
    
    // 布局组合
    auto renderer = Renderer([&] {
        return vbox({
            标题栏,
            hbox({ 左侧菜单, 右侧内容 }),
            快捷键提示栏,
        });
    });
    
    screen.Loop(renderer);  // 启动事件循环
}
```

### MenuComponent.cpp - 导航菜单

```cpp
Component CreateMenu(std::unique_ptr<MainScreen>& mainScreen) {
    // 使用 static 确保生命周期
    static int selected = 0;
    static std::vector<std::string> menuEntries = {
        "主界面", "分析", "配置", "日志", "状态", "退出"
    };
    
    auto menu = Menu(&menuEntries, &selected);
    
    // 添加事件处理：菜单切换时同步状态
    auto menuWithEvent = CatchEvent(menu, [&](Event event) {
        if (event == Event::ArrowUp || event == Event::ArrowDown) {
            mainScreen->SetCurrentMenu(selected);
            return false;  // 继续传递事件
        }
        return false;
    });
    
    return Renderer(menuWithEvent, [=] {
        return vbox({ 标题, separator(), menu->Render() });
    });
}
```

### MainScreen.cpp - 主内容区

```cpp
Element MainScreen::Render() {
    // 根据当前菜单索引切换内容
    switch (currentMenu_) {
        case 0:  return RenderOverview();
        case 1:  return RenderAnalysis();
        case 2:  return RenderConfiguration();
        case 3:  return RenderLogs();
        case 4:  return RenderStatus();
        case 5:  return RenderExit();
        default: return RenderOverview();
    }
}

Element MainScreen::RenderOverview() {
    // 安全计算进度（防止除零）
    float progress = (totalStocks_ > 0) ? analyzedStocks_ / totalStocks_ : 0.0f;
    
    return vbox({
        标题栏,
        分隔符,
        hbox({
            股票统计面板,
            分析状态面板,
        }),
        降级模式提示,
    });
}
```

---

## 🚨 关键修复点（段错误排查）

### 修复历史

从 **5 个段错误问题** 逐步修复，最终实现稳定交互：

| 问题 | 根因 | 修复方案 |
|------|------|---------|
| **1. 初始化顺序错误** | Logger 未初始化时使用 LOG_INFO | Logger 优先初始化 |
| **2. 降级模式缺失** | dataSource_ nullptr 导致崩溃 | 添加空指针检查 |
| **3. 策略配置错误** | 依赖环境变量 STRATEGIES（未设置） | 从 Config 读取 + 默认策略 |
| **4. FTXUI 生命周期** | menuEntries vector 销毁后 Menu 崩溃 | 使用 static 存储 |
| **5. vector 长度错误** | gauge(0/0) 除零错误 + Fullscreen 模式 | 安全计算 + TerminalOutput |

### 最终稳定架构

```cpp
// 稳定的组件组合模式
auto renderer = Renderer([&] {
    return vbox({
        标题栏,
        hbox({
            左侧导航（static 数据）,
            separator(),
            右侧内容（动态渲染）,
        }),
        快捷键提示,
    });
});

screen.Loop(renderer);  // ✅ 稳定运行
```

---

## 📊 降级模式说明

### 当前运行模式

**降级模式**: 系统在无数据源的情况下安全运行，显示基础状态。

```
提示: 系统运行在降级模式（无数据源）
请配置 DATA_SOURCE_API_KEY 以启用完整功能
```

### 启用完整功能

在 `.env` 文件中配置：

```bash
DATA_SOURCE_API_KEY=your_api_key_here
STRATEGIES=EMA25_GREATER_17_PRICE_MATCH,SURGE_SIGNAL
```

**完整功能**:
- ✅ 批量股票分析
- ✅ 实时数据获取
- ✅ 策略信号生成
- ✅ 数据库存储
- ✅ 日志实时查看

---

## 🎯 后续开发计划

### P1（高优先级）

1. **实现快捷键响应**
   - [A] 开始分析 → 调用 `stock_core::api::performBatchAnalysis()`
   - [R] 刷新数据 → 调用 `MainScreen::RefreshData()`
   - [L] 查看日志 → 实时显示 logs/app.log 内容

2. **完善菜单切换逻辑**
   - 菜单切换时更新右侧内容
   - 添加过渡动画（FTXUI 支持）

3. **数据导入功能**
   - 支持从 CSV 导入股票列表
   - 绕过 API 依赖

### P2（中优先级）

1. **FTXUI 组件重构**
   - 避免 static 依赖（使用智能指针管理）
   - 添加主题切换（light/dark）

2. **实时数据流**
   - 使用 FTXUI 的 `Loop` + `Post` 实现异步数据刷新
   - 实时显示分析进度

3. **配置向导**
   - 首次运行引导设置 API_KEY
   - 交互式配置界面

---

## 🔍 测试验证

### 编译与运行

```bash
# 编译
cd cpp/build
cmake --build . --target stock_tui

# 运行
cd stock_tui
./stock_tui
```

### 验证清单

✅ **基础功能**:
- 系统启动无段错误
- 核心库初始化成功
- 界面完整渲染

✅ **交互功能**:
- 导航菜单显示（6 个菜单项）
- 标签切换（概览/股票列表/快速操作）
- 快捷键提示栏

⏳ **待完善**:
- 快捷键实际响应逻辑
- 菜单切换内容联动
- 实时数据刷新

---

## 📝 开发者注意事项

### FTXUI 使用规范

1. **生命周期管理**: static 或智能指针管理组件依赖的数据
2. **异常安全**: 所有渲染函数添加 try-catch
3. **除零防护**: gauge/progress 计算前检查 divisor
4. **事件传递**: CatchEvent 返回 false 继续传递，true 拦截

### 调试技巧

```cpp
// 添加调试输出（仅开发阶段）
std::cerr << "[DEBUG] 当前菜单: " << currentMenu_ << std::endl;

// 验证组件状态
if (!menu) {
    LOG_ERROR("Menu 组件未创建");
    return;
}
```

---

## 🎓 学习资源

- **FTXUI 官方文档**: https://github.com ArthurSonzogni/FTXUI
- **组件示例**: https://github.com ArthurSonzogni/FTXUI/tree/main/examples
- **事件处理**: https://arthursonzogni.github.io/FTXUI/component.html

---

**文档版本**: v1.0.0  
**最后更新**: 2026-06-17  
**维护者**: Senior Developer (高级开发工程师)