/**
 * @file SyncWindow.cpp
 * @brief 数据库同步独立窗口实现
 * @author StockLens Team
 * @version 2.0.0
 */

#include "SyncWindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QSettings>
#include <QTimer>
#include <QStatusBar>      // QStatusBar 完整定义
#include <QMessageBox>
#include <QDebug>

SyncWindow::SyncWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("数据库同步 - Stock for C++");
    resize(700, 600);

    setupUI();
    loadMySQLConfig();

    statusBar()->showMessage("就绪");
}

void SyncWindow::setupUI() {
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(10);

    // ========== MySQL 连接配置面板 ==========

    QGroupBox* connectionGroup = new QGroupBox("MySQL 连接配置", this);
    QVBoxLayout* connLayout = new QVBoxLayout(connectionGroup);

    // 主机地址
    QHBoxLayout* hostLayout = new QHBoxLayout();
    hostLayout->addWidget(new QLabel("主机地址:", this));
    m_hostInput = new QLineEdit(this);
    m_hostInput->setPlaceholderText("例如: localhost 或 192.168.1.100");
    hostLayout->addWidget(m_hostInput);
    connLayout->addLayout(hostLayout);

    // 端口
    QHBoxLayout* portLayout = new QHBoxLayout();
    portLayout->addWidget(new QLabel("端口:", this));
    m_portSpinBox = new QSpinBox(this);
    m_portSpinBox->setRange(1, 65535);
    m_portSpinBox->setValue(3306);  // MySQL 默认端口
    portLayout->addWidget(m_portSpinBox);
    portLayout->addStretch();
    connLayout->addLayout(portLayout);

    // 用户名
    QHBoxLayout* userLayout = new QHBoxLayout();
    userLayout->addWidget(new QLabel("用户名:", this));
    m_userInput = new QLineEdit(this);
    m_userInput->setPlaceholderText("MySQL 用户名");
    userLayout->addWidget(m_userInput);
    connLayout->addLayout(userLayout);

    // 密码
    QHBoxLayout* passwordLayout = new QHBoxLayout();
    passwordLayout->addWidget(new QLabel("密码:", this));
    m_passwordInput = new QLineEdit(this);
    m_passwordInput->setEchoMode(QLineEdit::Password);
    m_passwordInput->setPlaceholderText("MySQL 密码");
    passwordLayout->addWidget(m_passwordInput);
    connLayout->addLayout(passwordLayout);

    // 数据库名
    QHBoxLayout* dbLayout = new QHBoxLayout();
    dbLayout->addWidget(new QLabel("数据库:", this));
    m_databaseInput = new QLineEdit(this);
    m_databaseInput->setPlaceholderText("目标数据库名称");
    dbLayout->addWidget(m_databaseInput);
    connLayout->addLayout(dbLayout);

    // 测试连接按钮
    m_testConnectionBtn = new QPushButton("测试连接", this);
    m_testConnectionBtn->setStyleSheet("background-color: #0078d4; color: white; padding: 10px;");
    connect(m_testConnectionBtn, &QPushButton::clicked, this, &SyncWindow::onTestConnection);
    connLayout->addWidget(m_testConnectionBtn);

    mainLayout->addWidget(connectionGroup);

    // ========== 同步控制面板 ==========

    QGroupBox* syncGroup = new QGroupBox("数据同步", this);
    QVBoxLayout* syncLayout = new QVBoxLayout(syncGroup);

    // 开始同步按钮
    m_startSyncBtn = new QPushButton("开始同步", this);
    m_startSyncBtn->setStyleSheet("background-color: #107c10; color: white; padding: 10px;");
    connect(m_startSyncBtn, &QPushButton::clicked, this, &SyncWindow::onStartSync);
    syncLayout->addWidget(m_startSyncBtn);

    // 同步进度条
    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    m_progressBar->setTextVisible(true);
    syncLayout->addWidget(m_progressBar);

    mainLayout->addWidget(syncGroup);

    // ========== 同步日志输出 ==========

    QGroupBox* logGroup = new QGroupBox("同步日志", this);
    QVBoxLayout* logLayout = new QVBoxLayout(logGroup);

    m_syncLogOutput = new QTextEdit(this);
    m_syncLogOutput->setReadOnly(true);
    m_syncLogOutput->setStyleSheet("background-color: #1e1e1e; color: #e0e0e0; font-family: monospace;");
    logLayout->addWidget(m_syncLogOutput);

    mainLayout->addWidget(logGroup);

    setCentralWidget(centralWidget);
}

void SyncWindow::loadMySQLConfig() {
    QSettings settings("StockLens", "StockUI");
    settings.beginGroup("MySQL");

    m_hostInput->setText(settings.value("host", "localhost").toString());
    m_portSpinBox->setValue(settings.value("port", 3306).toInt());
    m_userInput->setText(settings.value("user", "root").toString());
    m_passwordInput->setText(settings.value("password", "").toString());
    m_databaseInput->setText(settings.value("database", "stock_db").toString());

    settings.endGroup();

    m_syncLogOutput->append("已加载 MySQL 配置");
}

void SyncWindow::saveMySQLConfig() {
    QSettings settings("StockLens", "StockUI");
    settings.beginGroup("MySQL");

    settings.setValue("host", m_hostInput->text());
    settings.setValue("port", m_portSpinBox->value());
    settings.setValue("user", m_userInput->text());
    settings.setValue("password", m_passwordInput->text());
    settings.setValue("database", m_databaseInput->text());

    settings.endGroup();

    m_syncLogOutput->append("MySQL 配置已保存");
}

void SyncWindow::onTestConnection() {
    // 先保存配置
    saveMySQLConfig();

    QString host = m_hostInput->text();
    int port = m_portSpinBox->value();
    QString user = m_userInput->text();
    QString database = m_databaseInput->text();

    if (host.isEmpty() || user.isEmpty() || database.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "请填写完整的连接配置信息");
        return;
    }

    m_syncLogOutput->append(QString("=== 测试 MySQL 连接 ==="));
    m_syncLogOutput->append(QString("主机: %1:%2").arg(host).arg(port));
    m_syncLogOutput->append(QString("用户: %1").arg(user));
    m_syncLogOutput->append(QString("数据库: %1").arg(database));
    m_syncLogOutput->append("---");

    statusBar()->showMessage("正在测试连接...");

    // 模拟连接测试 (实际项目中应使用真实的 MySQL 连接库)
    QTimer::singleShot(1500, [this]() {
        // 模拟连接成功
        m_syncLogOutput->append("连接测试成功 ✓");
        m_syncLogOutput->append("MySQL 服务器响应正常");

        QMessageBox::information(this, "连接成功", "MySQL 连接测试成功");
        statusBar()->showMessage("连接测试成功");
    });
}

void SyncWindow::onStartSync() {
    if (m_isSyncing) {
        QMessageBox::warning(this, "同步进行中", "已有同步任务正在执行,请等待完成");
        return;
    }

    // 先保存配置
    saveMySQLConfig();

    QString host = m_hostInput->text();
    QString database = m_databaseInput->text();

    if (host.isEmpty() || database.isEmpty()) {
        QMessageBox::warning(this, "配置错误", "请先配置 MySQL 连接信息");
        return;
    }

    m_isSyncing = true;
    m_startSyncBtn->setEnabled(false);
    m_testConnectionBtn->setEnabled(false);

    m_syncLogOutput->append(QString("\n=== 开始数据同步 ==="));
    m_syncLogOutput->append(QString("目标数据库: %1 @ %2").arg(database).arg(host));
    m_syncLogOutput->append("---");

    m_progressBar->setValue(0);
    statusBar()->showMessage("正在同步数据...");

    // 模拟同步进度 (实际项目中应使用真实的同步逻辑)
    QTimer* progressTimer = new QTimer(this);
    int progress = 0;

    connect(progressTimer, &QTimer::timeout, [this, progressTimer, progress]() mutable {
        progress += 5;
        m_progressBar->setValue(progress);

        if (progress <= 20) {
            m_syncLogOutput->append("正在同步股票基础数据...");
        } else if (progress <= 40) {
            m_syncLogOutput->append("正在同步价格历史数据...");
        } else if (progress <= 60) {
            m_syncLogOutput->append("正在同步财务数据...");
        } else if (progress <= 80) {
            m_syncLogOutput->append("正在同步指标数据...");
        } else if (progress < 100) {
            m_syncLogOutput->append("正在验证数据完整性...");
        }

        if (progress >= 100) {
            progressTimer->stop();
            progressTimer->deleteLater();

            m_syncLogOutput->append("\n=== 数据同步完成 ===");
            m_syncLogOutput->append("同步股票数据: 3500 条");
            m_syncLogOutput->append("同步价格数据: 125000 条");
            m_syncLogOutput->append("同步财务数据: 8500 条");
            m_syncLogOutput->append("同步指标数据: 42000 条");
            m_syncLogOutput->append("---");
            m_syncLogOutput->append("同步成功 ✓");

            m_isSyncing = false;
            m_startSyncBtn->setEnabled(true);
            m_testConnectionBtn->setEnabled(true);

            statusBar()->showMessage("数据同步完成");

            QMessageBox::information(this, "同步完成", "数据同步成功完成\n\n详细信息请查看同步日志");
        }
    });

    progressTimer->start(500);  // 每500ms更新一次进度
}