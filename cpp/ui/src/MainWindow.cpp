/**
 * @file MainWindow.cpp
 * @brief Qt UI 主窗口实现
 * @author StockLens Team
 * @version 1.0.0
 */

#include "MainWindow.h"
#include "adapters/StockListModel.h"
#include "widgets/CandlestickWidget.h"
#include "widgets/StrategyPanel.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QSettings>
#include <QHeaderView>
#include <QSplitter>
#include <QVBoxLayout>

// 使用 ui namespace
using namespace ui;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    // 恢复上次窗口状态
    QSettings settings("StockLens", "StockUI");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());

    // 构建界面
    setupMenuBar();
    setupToolBar();
    setupStatusBar();
    setupDockWidgets();
    setupConnections();

    // 默认布局
    if (!settings.value("windowState").isValid()) {
        resize(1200, 800);
        // 默认停靠布局：左侧股票列表，右侧图表，底部策略面板
    }

    statusBar()->showMessage("就绪");
}

MainWindow::~MainWindow() {
    // 保存窗口状态
    QSettings settings("StockLens", "StockUI");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
}

void MainWindow::setupMenuBar() {
    // 文件菜单
    QMenu* fileMenu = menuBar()->addMenu("文件");
    
    QAction* openAction = fileMenu->addAction("打开数据文件...");
    openAction->setShortcut(QKeySequence::Open);
    connect(openAction, &QAction::triggered, this, &MainWindow::onMenuFileOpen);
    
    fileMenu->addSeparator();
    
    QAction* exitAction = fileMenu->addAction("退出");
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);

    // 设置菜单
    QMenu* settingsMenu = menuBar()->addMenu("设置");
    
    QAction* settingsAction = settingsMenu->addAction("配置...");
    connect(settingsAction, &QAction::triggered, this, &MainWindow::onMenuSettings);

    // 帮助菜单
    QMenu* helpMenu = menuBar()->addMenu("帮助");
    
    QAction* aboutAction = helpMenu->addAction("关于 Stock for C++");
    connect(aboutAction, &QAction::triggered, this, &MainWindow::onMenuAbout);
}

void MainWindow::setupToolBar() {
    QToolBar* mainToolBar = addToolBar("主工具栏");
    
    QAction* refreshAction = mainToolBar->addAction("刷新");
    refreshAction->setIcon(QIcon(":/icons/refresh.png"));
    
    QAction* exportAction = mainToolBar->addAction("导出");
    exportAction->setIcon(QIcon(":/icons/export.png"));
    
    mainToolBar->addSeparator();
    
    QAction* themeAction = mainToolBar->addAction("切换主题");
    themeAction->setIcon(QIcon(":/icons/theme.png"));
}

void MainWindow::setupStatusBar() {
    statusBar()->showMessage("就绪");
}

void MainWindow::setupDockWidgets() {
    // ========== 左侧：股票列表 ==========
    QDockWidget* stockListDock = new QDockWidget("股票列表", this);
    stockListDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    
    m_stockListView = new QTreeView(stockListDock);
    m_stockModel = new StockListModel(this);
    m_stockListView->setModel(m_stockModel);
    m_stockListView->setRootIsDecorated(false);
    m_stockListView->setAlternatingRowColors(true);
    m_stockListView->header()->setStretchLastSection(true);
    
    stockListDock->setWidget(m_stockListView);
    addDockWidget(Qt::LeftDockWidgetArea, stockListDock);

    // ========== 中央区域：K线图表 ==========
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* centralLayout = new QVBoxLayout(centralWidget);
    centralLayout->setContentsMargins(0, 0, 0, 0);
    
    // 实际的 K线图表控件
    m_candlestickWidget = new CandlestickWidget(this);
    centralLayout->addWidget(m_candlestickWidget);
    
    setCentralWidget(centralWidget);

    // ========== 右侧/底部：策略面板 ==========
    m_strategyDock = new QDockWidget("策略配置", this);
    m_strategyDock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::RightDockWidgetArea);
    
    // 实际的策略面板控件
    m_strategyPanel = new StrategyPanel(m_strategyDock);
    m_strategyDock->setWidget(m_strategyPanel);
    
    addDockWidget(Qt::BottomDockWidgetArea, m_strategyDock);
    
    // 设置默认策略列表（示例）
    std::vector<std::string> strategies = {
        "EMA17Breakout",
        "MACD",
        "RSI",
        "BOLL",
        "KDJ"
    };
    m_strategyPanel->setAvailableStrategies(strategies);
}

void MainWindow::setupConnections() {
    // 股票列表点击 → 选股信号
    connect(m_stockListView, &QTreeView::clicked, 
            this, &MainWindow::onStockClicked);
    
    // 策略面板信号
    if (m_strategyPanel) {
        connect(m_strategyPanel, &StrategyPanel::strategyChanged,
                this, [this](const QString& strategy) {
                    statusBar()->showMessage(QString("切换策略: %1").arg(strategy));
                });
        
        connect(m_strategyPanel, &StrategyPanel::paramChanged,
                this, [this](const QString& param, double value) {
                    statusBar()->showMessage(
                        QString("参数变更: %1 = %2").arg(param).arg(value, 0, 'f', 2));
                });
        
        connect(m_strategyPanel, &StrategyPanel::calculateRequested,
                this, [this]() {
                    statusBar()->showMessage("计算策略信号...");
                    // TODO: 调用核心库计算
                    emit strategyResultUpdated();
                });
    }
}

void MainWindow::onStockClicked(const QModelIndex& index) {
    QString tsCode = m_stockModel->getTsCode(index);
    m_currentTsCode = tsCode;
    
    statusBar()->showMessage(QString("选中股票: %1").arg(tsCode));
    emit stockSelected(tsCode);
    
    // TODO: Phase 2 - 触发图表更新
    // m_candlestickWidget->loadStockData(tsCode);
}

void MainWindow::onMenuFileOpen() {
    QString fileName = QFileDialog::getOpenFileName(
        this, "打开数据文件", "", "SQLite 数据库 (*.db);;所有文件 (*)"
    );
    
    if (!fileName.isEmpty()) {
        statusBar()->showMessage(QString("加载: %1").arg(fileName));
        // TODO: 加载自定义数据文件
    }
}

void MainWindow::onMenuSettings() {
    QMessageBox::information(this, "配置", 
        "配置对话框将在 Phase 3 实现");
}

void MainWindow::onMenuAbout() {
    QMessageBox::about(this, "关于 Stock for C++",
        "<h3>Stock for C++ Qt UI</h3>"
        "<p>版本: 1.0.0</p>"
        "<p>基于 Qt 6 的股票分析终端</p>"
        "<p>© 2026 StockLens Team</p>"
    );
}