/**
 * @file MainWindow.h (简化版)
 * @brief Qt UI 主窗口头文件 - 入口窗口 (单一职责: 导航)
 * @author StockLens Team
 * @version 2.0.0
 */

#pragma once

#include <QMainWindow>
#include <QWidget>
#include <QPushButton>
#include <QString>

/**
 * @class MainWindow
 * @brief 入口窗口 - 单一职责: 导航功能
 * 
 * 负责功能:
 * - 显示应用标题和快捷导航按钮
 * - 打开各个独立功能窗口
 * - 保留菜单栏(文件/设置/帮助)
 * - 不包含任何业务逻辑界面(DockWidget全部移除)
 * 
 * 重构说明:
 * - 从 19项职责简化为 1项职责(导航)
 * - 所有业务逻辑界面移到独立窗口
 * - 数据库初始化移到 ApplicationManager
 * - 后台进程管理移到 TaskWindow
 * - 主题管理移到 ApplicationManager
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() = default;

signals:
    /**
     * @brief 打开任务管理窗口请求信号
     */
    void openTaskWindowRequested();

    /**
     * @brief 打开配置管理窗口请求信号
     */
    void openConfigWindowRequested();

    /**
     * @brief 打开数据查询窗口请求信号
     */
    void openQueryWindowRequested();

    /**
     * @brief 打开数据库同步窗口请求信号
     */
    void openSyncWindowRequested();

    /**
     * @brief 打开 K线图表窗口请求信号
     */
    void openChartWindowRequested();

private:
    /**
     * @brief 设置 UI 布局 (简化版 - 只包含导航按钮)
     */
    void setupUI();

    /**
     * @brief 设置菜单栏
     */
    void setupMenuBar();

    /**
     * @brief 设置导航按钮区域
     */
    void setupNavigationButtons();

    // ========== 导航按钮 ==========

    QPushButton* m_taskButton = nullptr;
    QPushButton* m_configButton = nullptr;
    QPushButton* m_queryButton = nullptr;
    QPushButton* m_syncButton = nullptr;
    QPushButton* m_chartButton = nullptr;
};