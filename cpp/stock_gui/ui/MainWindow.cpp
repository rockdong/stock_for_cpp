#include "MainWindow.h"
#include "AnalysisWidget.h"
#include "ConfigWidget.h"
#include "LogWidget.h"
#include "StatusWidget.h"

#include <QAction>
#include <QMenu>
#include <QIcon>
#include <QLabel>

namespace stock_gui {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , tabWidget_(nullptr)
    , statusBar_(nullptr)
    , menuBar_(nullptr)
    , toolBar_(nullptr)
    , analysisPage_(nullptr)
    , configPage_(nullptr)
    , logPage_(nullptr)
    , statusPage_(nullptr)
{
    SetupUI();
}

MainWindow::~MainWindow() = default;

void MainWindow::SetupUI() {
    // 设置菜单栏
    SetupMenuBar();

    // 设置工具栏
    SetupToolBar();

    // 设置标签页
    SetupTabWidget();

    // 设置状态栏
    SetupStatusBar();
}

void MainWindow::SetupMenuBar() {
    menuBar_ = new QMenuBar(this);
    setMenuBar(menuBar_);

    // 文件菜单
    QMenu* fileMenu = menuBar_->addMenu("文件");
    QAction* exitAction = fileMenu->addAction("退出");
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &QMainWindow::close);

    // 编辑菜单
    QMenu* editMenu = menuBar_->addMenu("编辑");
    QAction* configAction = editMenu->addAction("配置");
    configAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_C));
    connect(configAction, &QAction::triggered, [this]() {
        tabWidget_->setCurrentIndex(1);  // 跳转到配置页面
    });

    // 视图菜单
    QMenu* viewMenu = menuBar_->addMenu("视图");
    QAction* logAction = viewMenu->addAction("日志");
    logAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_L));
    connect(logAction, &QAction::triggered, [this]() {
        tabWidget_->setCurrentIndex(2);  // 跳转到日志页面
    });

    // 帮助菜单
    QMenu* helpMenu = menuBar_->addMenu("帮助");
    QAction* aboutAction = helpMenu->addAction("关于");
    connect(aboutAction, &QAction::triggered, [this]() {
        // TODO: 显示关于对话框
    });
}

void MainWindow::SetupToolBar() {
    toolBar_ = new QToolBar("工具栏", this);
    addToolBar(toolBar_);

    QAction* analyzeAction = toolBar_->addAction("开始分析");
    analyzeAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_A));
    connect(analyzeAction, &QAction::triggered, [this]() {
        // TODO: 开始分析
    });

    QAction* refreshAction = toolBar_->addAction("刷新数据");
    refreshAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_R));
    connect(refreshAction, &QAction::triggered, [this]() {
        // TODO: 刷新数据
    });

    QAction* exportAction = toolBar_->addAction("导出报告");
    exportAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_E));
    connect(exportAction, &QAction::triggered, [this]() {
        // TODO: 导出报告
    });
}

void MainWindow::SetupTabWidget() {
    tabWidget_ = new QTabWidget(this);
    setCentralWidget(tabWidget_);

    // 创建各个页面
    analysisPage_ = new AnalysisWidget(this);
    configPage_ = new ConfigWidget(this);
    logPage_ = new LogWidget(this);
    statusPage_ = new StatusWidget(this);

    // 添加标签页
    tabWidget_->addTab(analysisPage_, "分析");
    tabWidget_->addTab(configPage_, "配置");
    tabWidget_->addTab(logPage_, "日志");
    tabWidget_->addTab(statusPage_, "状态");

    // 设置标签页样式
    tabWidget_->setTabPosition(QTabWidget::North);
    tabWidget_->setDocumentMode(true);
}

void MainWindow::SetupStatusBar() {
    statusBar_ = new QStatusBar(this);
    setStatusBar(statusBar_);

    // 添加状态标签
    QLabel* statusLabel = new QLabel("就绪", this);
    statusBar_->addWidget(statusLabel);

    // 添加进度标签（右侧）
    QLabel* progressLabel = new QLabel("进度: 0%", this);
    statusBar_->addPermanentWidget(progressLabel);
}

}  // namespace stock_gui