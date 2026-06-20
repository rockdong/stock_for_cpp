# Qt UI 界面架构设计文档

## 问题分析

当前 `MainWindow` 违反单一职责原则,承担了以下职责:

### 当前职责列表(19项,过于臃肿)
1. 股票列表显示和管理(左侧DockWidget)
2. K线图表显示(右侧DockWidget)
3. 策略面板(底部DockWidget)
4. 任务管理面板(右侧DockWidget)
5. 配置面板(右侧DockWidget)
6. 查询面板(右侧DockWidget)
7. MySQL同步面板(右侧DockWidget)
8. 菜单栏管理(文件、设置、视图、帮助)
9. 工具栏管理(刷新、导出、主题切换、任务管理等)
10. 状态栏管理
11. **数据库连接初始化**(initializeCoreLib)
12. **股票数据加载**(loadStockList)
13. **K线数据加载**(loadCandlestickData)
14. **导出功能**(CSV/PNG导出)
15. **主题切换和QSS样式管理**
16. **布局管理**(保存、重置、多图表窗口)
17. **后台进程管理**(QProcess任务执行)
18. **MySQL连接测试和同步逻辑**
19. **各种查询逻辑**(股票查询、价格查询、分析查询)

---

## 新架构设计

### 核心原则
- **单一职责**: 每个类只负责一件事
- **界面独立**: 每个功能界面有独立的窗口类
- **导航集中**: ApplicationManager作为界面协调器

---

### 新架构组成

#### 1. ApplicationManager (界面协调器)
**职责**: 统一管理所有界面窗口,提供导航功能

```cpp
class ApplicationManager {
public:
    // 初始化所有界面窗口
    void initialize();
    
    // 显示指定界面窗口
    void showTaskWindow();      // 显示任务管理窗口
    void showConfigWindow();     // 显示配置窗口
    void showQueryWindow();      // 显示查询窗口
    void showSyncWindow();       // 显示MySQL同步窗口
    void showChartWindow();      // 显示图表窗口(可选)
    
    // 关闭所有窗口
    void closeAllWindows();
    
private:
    MainWindow* m_mainWindow = nullptr;  // 入口窗口(导航)
    TaskWindow* m_taskWindow = nullptr;
    ConfigWindow* m_configWindow = nullptr;
    QueryWindow* m_queryWindow = nullptr;
    SyncWindow* m_syncWindow = nullptr;
};
```

---

#### 2. MainWindow (入口窗口 - 简化版)
**职责**: 仅提供导航入口,移除所有业务逻辑

```cpp
class MainWindow : public QMainWindow {
    // 只保留导航功能
    // - 顶部:应用标题和快捷导航按钮
    // - 左侧:股票列表(可选保留或移到独立窗口)
    // - 无业务逻辑界面(DockWidget全部移除)
    
private slots:
    void onOpenTaskWindow();    // 打开任务管理窗口
    void onOpenConfigWindow();  // 打开配置窗口
    void onOpenQueryWindow();   // 打开查询窗口
    void onOpenSyncWindow();    // 打开同步窗口
    
    // 移除以下职责:
    // - 数据库初始化(移到ApplicationManager)
    // - 数据加载(移到各自的功能窗口)
    // - 导出功能(移到各个功能窗口内部)
    // - 主题切换(移到ApplicationManager全局管理)
    // - 后台进程管理(移到TaskWindow内部)
};
```

---

#### 3. TaskWindow (任务管理独立窗口)
**职责**: 任务管理功能 - 启动/停止/监控后台任务

```cpp
class TaskWindow : public QMainWindow {
    // 独立窗口,包含:
    // - 任务列表显示
    // - 任务状态监控(运行中/已完成/失败)
    // - 任务日志输出
    // - 启动/停止按钮
    // - 任务历史记录
    
private:
    QProcess* m_taskProcess = nullptr;  // 后台进程管理(从MainWindow移入)
    TaskHistoryManager* m_historyManager = nullptr;
    
private slots:
    void onStartTask();
    void onStopTask();
    void onTaskProcessFinished(int exitCode);
    void onTaskProcessError(QProcess::ProcessError error);
};
```

---

#### 4. ConfigWindow (配置管理独立窗口)
**职责**: 系统配置管理 - 环境变量、策略参数、主题设置

```cpp
class ConfigWindow : public QMainWindow {
    // 独立窗口,包含:
    // - 环境配置面板(.env编辑)
    // - 策略参数配置
    // - 主题切换(浅色/深色)
    // - 配置保存和加载
    
private slots:
    void onSaveConfig();
    void onToggleTheme();
    void onLoadDefaultConfig();
};
```

---

#### 5. QueryWindow (数据查询独立窗口)
**职责**: 数据查询功能 - 股票查询、价格查询、分析结果查询

```cpp
class QueryWindow : public QMainWindow {
    // 独立窗口,包含:
    // - 股票查询面板(按关键词搜索)
    // - 价格查询面板(指定股票代码和日期范围)
    // - 分析结果查询(指定策略和时间范围)
    // - 查询结果显示表格
    // - 导出查询结果按钮
    
private:
    StockDAO* m_stockDAO = nullptr;  // 数据访问对象
    PriceDAO* m_priceDAO = nullptr;
    
private slots:
    void onStockQuery();
    void onPriceQuery();
    void onAnalysisQuery();
    void onExportResults();
};
```

---

#### 6. SyncWindow (数据库同步独立窗口)
**职责**: MySQL数据库同步功能

```cpp
class SyncWindow : public QMainWindow {
    // 独立窗口,包含:
    // - MySQL连接配置面板
    // - 连接测试按钮
    // - 同步进度显示
    // - 同步日志输出
    // - 开始同步按钮
    
private slots:
    void onTestConnection();
    void onStartSync();
    void onSyncProgressUpdated(int progress);
};
```

---

#### 7. ChartWindow (图表独立窗口 - 可选)
**职责**: K线图表显示和分析

```cpp
class ChartWindow : public QMainWindow {
    // 独立窗口,包含:
    // - K线图表显示(CandlestickWidget)
    // - 股票选择器
    // - 时间范围选择
    // - 图表导出按钮
    
private:
    CandlestickWidget* m_chartWidget = nullptr;
    PriceDAO* m_priceDAO = nullptr;
    
private slots:
    void onLoadChart();
    void onExportChart();
};
```

---

## 重构步骤

### Phase 1: 创建ApplicationManager
1. 创建 `ApplicationManager.h` 和 `ApplicationManager.cpp`
2. 实现界面窗口管理逻辑
3. 实现导航功能(showTaskWindow等)

### Phase 2: 创建独立窗口类
1. 创建 `TaskWindow.h/cpp`
2. 创建 `ConfigWindow.h/cpp`
3. 创建 `QueryWindow.h/cpp`
4. 创建 `SyncWindow.h/cpp`
5. (可选)创建 `ChartWindow.h/cpp`

### Phase 3: 重构MainWindow
1. 移除所有业务逻辑界面(DockWidget)
2. 移除数据库初始化逻辑(移到ApplicationManager)
3. 移除后台进程管理(移到TaskWindow)
4. 只保留导航按钮和菜单栏

### Phase 4: 迁移业务逻辑
1. TaskWindow内部管理QProcess
2. QueryWindow内部管理DAO和数据查询
3. ConfigWindow内部管理配置和主题
4. SyncWindow内部管理MySQL同步逻辑

---

## 架构优势

1. **符合单一职责原则**: 每个类只负责一件事
2. **界面真正独立**: 每个功能窗口可以独立打开/关闭
3. **易于维护**: 修改某个功能不影响其他功能
4. **易于扩展**: 新增功能只需创建新的窗口类
5. **代码清晰**: 职责分离,避免MainWindow臃肿