/**
 * @file MainWindow.h
 * @brief Qt UI 主窗口定义
 * @author StockLens Team
 * @version 1.0.0
 */

#pragma once

#include <QMainWindow>
#include <QDockWidget>
#include <QTreeView>
#include <QTableView>
#include <QChartView>
#include <QStatusBar>
#include <QMenuBar>
#include <QToolBar>

// Qt Charts
#include <QChart>
#include <QCandlestickSeries>
#include <QLineSeries>

// 前向声明
class StockListModel;  // 全局 namespace

namespace ui {
class CandlestickWidget;
class StrategyPanel;
}

/**
 * @class MainWindow
 * @brief 主窗口 - 股票分析终端
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

signals:
    void stockSelected(const QString& tsCode);
    void strategyResultUpdated();

private slots:
    void onStockClicked(const QModelIndex& index);
    void onMenuFileOpen();
    void onMenuSettings();
    void onMenuAbout();

private:
    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
    void setupDockWidgets();
    void setupConnections();

    // 核心组件
    QTreeView* m_stockListView = nullptr;
    StockListModel* m_stockModel = nullptr;  // 全局 namespace
    
    // 图表区域
    QWidget* m_chartContainer = nullptr;
    ui::CandlestickWidget* m_candlestickWidget = nullptr;
    
    // 策略面板
    ui::StrategyPanel* m_strategyPanel = nullptr;
    QDockWidget* m_strategyDock = nullptr;
    
    // 状态信息
    QString m_currentTsCode;
};