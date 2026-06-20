/**
 * @file ApplicationManager.h
 * @brief Qt UI 应用程序管理器头文件 - 界面协调器
 * @author StockLens Team
 * @version 2.0.0
 */

#pragma once

#include <QObject>
#include <QString>

// 前置声明独立窗口类
class MainWindow;
class TaskWindow;
class ConfigWindow;
class QueryWindow;
class SyncWindow;
class ChartWindow;

namespace data {
    class StockDAO;
    class PriceDAO;
}

/**
 * @class ApplicationManager
 * @brief 应用程序管理器 - 界面协调器 (单一职责)
 * 
 * 负责功能:
 * - 初始化所有界面窗口
 * - 数据库连接初始化
 * - 全局主题管理
 * - 窗口导航和协调
 * - DAO 对象共享
 */
class ApplicationManager : public QObject {
    Q_OBJECT

public:
    explicit ApplicationManager(QObject* parent = nullptr);
    ~ApplicationManager();

    /**
     * @brief 初始化应用程序 (数据库、窗口、主题)
     */
    void initialize();

signals:
    /**
     * @brief 窗口切换信号
     * @param windowName 窗口名称
     */
    void windowSwitched(const QString& windowName);

public slots:
    /**
     * @brief 显示入口窗口
     */
    void showMainWindow();

    /**
     * @brief 显示任务管理窗口
     */
    void showTaskWindow();

    /**
     * @brief 显示配置管理窗口
     */
    void showConfigWindow();

    /**
     * @brief 显示数据查询窗口
     */
    void showQueryWindow();

    /**
     * @brief 显示数据库同步窗口
     */
    void showSyncWindow();

    /**
     * @brief 显示 K线图表窗口
     */
    void showChartWindow();

    /**
     * @brief 关闭所有窗口
     */
    void closeAllWindows();

    /**
     * @brief 应用全局主题
     * @param themeName 主题名称 ("light" 或 "dark")
     */
    void applyTheme(const QString& themeName);

private:
    /**
     * @brief 初始化数据库连接
     * @return true 初始化成功
     * @return false 初始化失败
     */
    bool initializeDatabase();

    /**
     * @brief 创建所有独立窗口
     */
    void createWindows();

    /**
     * @brief 创建 QSS 样式表文件 (如果不存在)
     */
    void createQSSFiles();

    // ========== 窗口成员 ==========

    MainWindow* m_mainWindow = nullptr;     // 入口窗口
    TaskWindow* m_taskWindow = nullptr;     // 任务管理窗口
    ConfigWindow* m_configWindow = nullptr; // 配置管理窗口
    QueryWindow* m_queryWindow = nullptr;   // 数据查询窗口
    SyncWindow* m_syncWindow = nullptr;     // 数据库同步窗口
    ChartWindow* m_chartWindow = nullptr;   // K线图表窗口

    // ========== DAO 对象 ==========

    data::StockDAO* m_stockDAO = nullptr;   // 股票数据访问对象
    data::PriceDAO* m_priceDAO = nullptr;   // 价格数据访问对象

    // ========== 全局状态 ==========

    QString m_currentTheme;                 // 当前主题名称
};