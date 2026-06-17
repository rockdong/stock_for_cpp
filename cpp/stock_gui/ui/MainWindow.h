#pragma once

#include <QMainWindow>
#include <QTabWidget>
#include <QStatusBar>
#include <QMenuBar>
#include <QToolBar>

namespace stock_gui {

/**
 * @brief 主窗口 - Qt GUI 客户端核心界面
 *
 * 功能:
 * - 标签页导航（分析/配置/日志/状态）
 * - 菜单栏（文件/编辑/视图/帮助）
 * - 工具栏（快速操作）
 * - 状态栏（实时状态显示）
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private:
    void SetupUI();
    void SetupMenuBar();
    void SetupToolBar();
    void SetupStatusBar();
    void SetupTabWidget();

    // UI 组件
    QTabWidget* tabWidget_;
    QStatusBar* statusBar_;
    QMenuBar* menuBar_;
    QToolBar* toolBar_;

    // 标签页页面（将在对应的 Widget 文件中定义）
    QWidget* analysisPage_;
    QWidget* configPage_;
    QWidget* logPage_;
    QWidget* statusPage_;
};

}  // namespace stock_gui