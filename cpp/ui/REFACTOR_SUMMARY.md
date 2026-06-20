# UI 界面重构总结

## 已完成工作 ✅

### 1. 架构设计文档
创建了 `ARCHITECTURE.md`,详细说明:
- 当前MainWindow违反单一职责的19项职责
- 新架构设计(ApplicationManager + 6个独立窗口)
- 重构步骤和优势

### 2. ApplicationManager(界面协调器)
创建了:
- `ApplicationManager.h/cpp`
- 职责:数据库初始化、窗口管理、全局主题管理、导航功能
- 从MainWindow移入:数据库初始化、主题管理逻辑

### 3. 独立窗口类(符合单一职责)
创建了以下独立窗口(每个窗口只做一件事):

#### TaskWindow - 任务管理窗口
- 职责:启动/停止后台任务、监控状态、显示日志、记录历史
- 从MainWindow移入:QProcess管理、任务逻辑
- 文件:`windows/TaskWindow.h/cpp`

#### ConfigWindow - 配置管理窗口
- 职责:编辑.env、策略参数、主题切换、保存配置
- 从MainWindow移入:配置编辑逻辑
- 文件:`windows/ConfigWindow.h/cpp`

#### QueryWindow - 数据查询窗口
- 职责:股票查询、价格查询、分析结果查询、导出结果
- 从MainWindow移入:查询逻辑(需要完善DAO集成)
- 文件:`windows/QueryWindow.h/cpp`

#### SyncWindow - 数据库同步窗口
- 职责:MySQL连接配置、测试连接、执行同步、显示进度
- 从MainWindow移入:MySQL同步逻辑
- 文件:`windows/SyncWindow.h/cpp`

#### ChartWindow - K线图表窗口
- 职责:显示K线图表、时间范围选择、导出PNG
- 从MainWindow移入:图表显示逻辑
- 文件:`windows/ChartWindow.h/cpp`

---

## 下一步需要完成 🚧

### 1. 重构 MainWindow 为简化版入口窗口
**需要修改**:
- 移除所有业务逻辑界面(DockWidget)
- 只保留导航按钮和菜单栏
- 添加信号:openTaskWindowRequested、openConfigWindowRequested等
- 移除数据库初始化(已移到ApplicationManager)
- 移除后台进程管理(已移到TaskWindow)

**简化版MainWindow职责**:
- 提供应用入口
- 显示导航按钮(打开各个独立窗口)
- 保留菜单栏(文件/设置/帮助)
- 不包含任何业务逻辑

### 2. 更新 CMakeLists.txt
添加新文件到编译列表:

```cmake
qt_add_library(ui_lib STATIC
    src/ApplicationManager.cpp
    src/ApplicationManager.h
    src/windows/TaskWindow.cpp
    src/windows/TaskWindow.h
    src/windows/ConfigWindow.cpp
    src/windows/ConfigWindow.h
    src/windows/QueryWindow.cpp
    src/windows/QueryWindow.h
    src/windows/SyncWindow.cpp
    src/windows/SyncWindow.h
    src/windows/ChartWindow.cpp
    src/windows/ChartWindow.h
    # 其他现有文件...
)
```

### 3. 修改 main.cpp 使用 ApplicationManager
```cpp
#include "ApplicationManager.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // 创建应用程序管理器
    ApplicationManager appManager;
    appManager.initialize();

    // 显示入口窗口
    appManager.showMainWindow();

    return app.exec();
}
```

### 4. 编译测试
执行:
```bash
cd cpp/ui/build
cmake ..
make
```

---

## 重构优势

1. **符合单一职责**:每个类只负责一件事
2. **界面真正独立**:每个功能窗口可以独立打开/关闭
3. **易于维护**:修改某个功能不影响其他功能
4. **易于扩展**:新增功能只需创建新的窗口类
5. **代码清晰**:职责分离,避免MainWindow臃肿(从19项职责减少到1项)

---

## 架构对比

### 旧架构(违反单一职责)
```
MainWindow (臃肿,19项职责)
├── 股票列表 DockWidget
├── K线图表 DockWidget
├── 策略面板 DockWidget
├── 任务管理 DockWidget
├── 配置面板 DockWidget
├── 查询面板 DockWidget
├── MySQL同步 DockWidget
└── 所有业务逻辑都在MainWindow
```

### 新架构(符合单一职责)
```
ApplicationManager (协调器)
├── MainWindow (入口窗口,只负责导航)
├── TaskWindow (任务管理,独立)
├── ConfigWindow (配置管理,独立)
├── QueryWindow (数据查询,独立)
├── SyncWindow (数据库同步,独立)
└── ChartWindow (K线图表,独立)
```

---

## 注意事项

1. **DAO对象共享**:ApplicationManager创建DAO对象,通过setDAOs()方法传递给QueryWindow和ChartWindow
2. **主题全局管理**:ApplicationManager统一管理主题,ConfigWindow发送信号切换主题
3. **窗口导航**:MainWindow通过信号通知ApplicationManager打开各个独立窗口
4. **编译依赖**:确保所有新文件添加到CMakeLists.txt

---

## 文件清单

### 新创建文件(10个)
- `cpp/ui/ARCHITECTURE.md` - 架构设计文档
- `cpp/ui/src/ApplicationManager.h/cpp` - 界面协调器
- `cpp/ui/src/windows/TaskWindow.h/cpp` - 任务管理窗口
- `cpp/ui/src/windows/ConfigWindow.h/cpp` - 配置窗口
- `cpp/ui/src/windows/QueryWindow.h/cpp` - 查询窗口
- `cpp/ui/src/windows/SyncWindow.h/cpp` - 同步窗口
- `cpp/ui/src/windows/ChartWindow.h/cpp` - 图表窗口

### 需要重构文件(3个)
- `cpp/ui/src/MainWindow.h/cpp` - 简化为入口窗口
- `cpp/ui/src/main.cpp` - 使用ApplicationManager
- `cpp/ui/CMakeLists.txt` - 添加新文件