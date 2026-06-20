/**
 * @file MainWindow.cpp (简化版)
 * @brief Qt UI 主窗口实现 - 入口窗口 (单一职责: 导航)
 * @author StockLens Team
 * @version 2.0.0
 */

#include "MainWindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QStatusBar>      // QStatusBar 完整定义
#include <QMessageBox>
#include <QCoreApplication>
#include <QDebug>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("Stock for C++ - 入口");
    resize(600, 400);

    setupUI();
    setupMenuBar();

    statusBar()->showMessage("欢迎使用 Stock for C++");
}

void MainWindow::setupUI() {
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(20);

    // ========== 应用标题 ==========

    QLabel* titleLabel = new QLabel("Stock for C++", this);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #0078d4;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    QLabel* subtitleLabel = new QLabel("股票数据分析系统", this);
    subtitleLabel->setStyleSheet("font-size: 16px; color: #555555;");
    subtitleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(subtitleLabel);

    mainLayout->addSpacing(20);

    // ========== 先设置 centralWidget,然后再创建导航按钮 ==========
    
    setCentralWidget(centralWidget);

    // ========== 导航按钮区域 ==========

    setupNavigationButtons();

    mainLayout->addStretch();
}

void MainWindow::setupMenuBar() {
    // ========== 文件菜单 ==========

    QMenu* fileMenu = menuBar()->addMenu("文件");

    QAction* exitAction = new QAction("退出", this);
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, [this]() {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this, "确认退出", "确定要退出应用程序吗?",
            QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            QCoreApplication::quit();
        }
    });
    fileMenu->addAction(exitAction);

    // ========== 设置菜单 ==========

    QMenu* settingsMenu = menuBar()->addMenu("设置");

    QAction* configAction = new QAction("打开配置窗口", this);
    connect(configAction, &QAction::triggered, this, [this]() {
        emit openConfigWindowRequested();
    });
    settingsMenu->addAction(configAction);

    // ========== 帮助菜单 ==========

    QMenu* helpMenu = menuBar()->addMenu("帮助");

    QAction* aboutAction = new QAction("关于", this);
    connect(aboutAction, &QAction::triggered, this, [this]() {
        QMessageBox::about(this, "关于 Stock for C++",
            "Stock for C++ v2.0.0\n\n"
            "股票数据分析系统\n\n"
            "功能特性:\n"
            "- 股票数据查询和分析\n"
            "- K线图表显示\n"
            "- 后台任务管理\n"
            "- MySQL 数据同步\n"
            "- 配置管理\n\n"
            "技术栈: Qt 6, C++17, SQLite/MySQL");
    });
    helpMenu->addAction(aboutAction);
}

void MainWindow::setupNavigationButtons() {
    QWidget* navWidget = new QWidget(this);
    QVBoxLayout* navLayout = new QVBoxLayout(navWidget);
    navLayout->setSpacing(15);

    // ========== 快捷导航按钮 ==========

    QLabel* navLabel = new QLabel("快捷导航", this);
    navLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #333333;");
    navLayout->addWidget(navLabel);

    // 任务管理按钮
    m_taskButton = new QPushButton("任务管理", this);
    m_taskButton->setStyleSheet(
        "QPushButton { background-color: #0078d4; color: white; border: none; "
        "padding: 15px; font-size: 14px; border-radius: 5px; }"
        "QPushButton:hover { background-color: #106ebe; }");
    connect(m_taskButton, &QPushButton::clicked, this, [this]() {
        emit openTaskWindowRequested();
        statusBar()->showMessage("已打开任务管理窗口");
    });
    navLayout->addWidget(m_taskButton);

    // 配置管理按钮
    m_configButton = new QPushButton("配置管理", this);
    m_configButton->setStyleSheet(
        "QPushButton { background-color: #107c10; color: white; border: none; "
        "padding: 15px; font-size: 14px; border-radius: 5px; }"
        "QPushButton:hover { background-color: #0b6a0b; }");
    connect(m_configButton, &QPushButton::clicked, this, [this]() {
        emit openConfigWindowRequested();
        statusBar()->showMessage("已打开配置管理窗口");
    });
    navLayout->addWidget(m_configButton);

    // 数据查询按钮
    m_queryButton = new QPushButton("数据查询", this);
    m_queryButton->setStyleSheet(
        "QPushButton { background-color: #881798; color: white; border: none; "
        "padding: 15px; font-size: 14px; border-radius: 5px; }"
        "QPushButton:hover { background-color: #6b0d75; }");
    connect(m_queryButton, &QPushButton::clicked, this, [this]() {
        emit openQueryWindowRequested();
        statusBar()->showMessage("已打开数据查询窗口");
    });
    navLayout->addWidget(m_queryButton);

    // 数据库同步按钮
    m_syncButton = new QPushButton("数据库同步", this);
    m_syncButton->setStyleSheet(
        "QPushButton { background-color: #d83b01; color: white; border: none; "
        "padding: 15px; font-size: 14px; border-radius: 5px; }"
        "QPushButton:hover { background-color: #a62c00; }");
    connect(m_syncButton, &QPushButton::clicked, this, [this]() {
        emit openSyncWindowRequested();
        statusBar()->showMessage("已打开数据库同步窗口");
    });
    navLayout->addWidget(m_syncButton);

    // K线图表按钮
    m_chartButton = new QPushButton("K线图表", this);
    m_chartButton->setStyleSheet(
        "QPushButton { background-color: #ff8c00; color: white; border: none; "
        "padding: 15px; font-size: 14px; border-radius: 5px; }"
        "QPushButton:hover { background-color: #d97000; }");
    connect(m_chartButton, &QPushButton::clicked, this, [this]() {
        emit openChartWindowRequested();
        statusBar()->showMessage("已打开 K线图表窗口");
    });
    navLayout->addWidget(m_chartButton);

    // 添加到主布局
    QVBoxLayout* mainLayout = static_cast<QVBoxLayout*>(centralWidget()->layout());
    mainLayout->addWidget(navWidget);
}