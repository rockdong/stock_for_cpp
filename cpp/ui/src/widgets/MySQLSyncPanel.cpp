#include "MySQLSyncPanel.h"
#include <QSettings>
#include <QMessageBox>
#include <QScrollBar>
#include <QStyle>
#include <QApplication>
#include <QDebug>

MySQLSyncPanel::MySQLSyncPanel(QWidget* parent)
    : QWidget(parent)
{
    setupUI();
    loadSavedConfig();
}

MySQLSyncPanel::~MySQLSyncPanel() {
    // 析构时保存配置
    saveConfig();
}

void MySQLSyncPanel::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    // ========== MySQL 连接配置区域 ==========

    QGroupBox* connectionGroup = new QGroupBox("MySQL 连接配置", this);
    QVBoxLayout* connectionLayout = new QVBoxLayout(connectionGroup);

    // 主机和端口
    QHBoxLayout* hostLayout = new QHBoxLayout();
    QLabel* hostLabel = new QLabel("主机地址:", this);
    m_hostEdit = new QLineEdit(this);
    m_hostEdit->setPlaceholderText("例如: mysql.example.com 或 192.168.1.100");
    m_hostEdit->setText("localhost");  // 默认值

    QLabel* portLabel = new QLabel("端口:", this);
    m_portSpinBox = new QSpinBox(this);
    m_portSpinBox->setRange(1, 65535);
    m_portSpinBox->setValue(3306);  // MySQL 默认端口
    m_portSpinBox->setFixedWidth(80);

    hostLayout->addWidget(hostLabel);
    hostLayout->addWidget(m_hostEdit, 3);
    hostLayout->addWidget(portLabel);
    hostLayout->addWidget(m_portSpinBox);
    connectionLayout->addLayout(hostLayout);

    // 数据库名称
    QHBoxLayout* dbLayout = new QHBoxLayout();
    QLabel* dbLabel = new QLabel("数据库:", this);
    m_databaseEdit = new QLineEdit(this);
    m_databaseEdit->setPlaceholderText("例如: stock_analysis");
    m_databaseEdit->setText("stock_db");  // 默认值

    dbLayout->addWidget(dbLabel);
    dbLayout->addWidget(m_databaseEdit);
    connectionLayout->addLayout(dbLayout);

    // 用户名和密码
    QHBoxLayout* userLayout = new QHBoxLayout();
    QLabel* userLabel = new QLabel("用户名:", this);
    m_userEdit = new QLineEdit(this);
    m_userEdit->setPlaceholderText("MySQL 用户名");
    m_userEdit->setText("root");  // 默认值

    QLabel* passwordLabel = new QLabel("密码:", this);
    m_passwordEdit = new QLineEdit(this);
    m_passwordEdit->setPlaceholderText("MySQL 密码");
    m_passwordEdit->setEchoMode(QLineEdit::Password);  // 密码模式

    userLayout->addWidget(userLabel);
    userLayout->addWidget(m_userEdit, 1);
    userLayout->addWidget(passwordLabel);
    userLayout->addWidget(m_passwordEdit, 1);
    connectionLayout->addLayout(userLayout);

    // 测试连接按钮和状态显示
    QHBoxLayout* testLayout = new QHBoxLayout();
    m_testConnectionBtn = new QPushButton("测试连接", this);
    m_testConnectionBtn->setIcon(style()->standardIcon(QStyle::SP_DialogApplyButton));
    m_testConnectionBtn->setToolTip("测试 MySQL 连接是否成功");
    connect(m_testConnectionBtn, &QPushButton::clicked, this, [this]() {
        if (validateInput()) {
            saveConfig();  // 保存配置
            emit testConnectionRequested(
                m_hostEdit->text(),
                m_portSpinBox->value(),
                m_databaseEdit->text(),
                m_userEdit->text(),
                m_passwordEdit->text()
            );
        }
    });

    m_connectionStatus = new QLabel("未测试", this);
    m_connectionStatus->setStyleSheet("color: #999999; font-weight: bold;");

    testLayout->addWidget(m_testConnectionBtn);
    testLayout->addStretch();
    testLayout->addWidget(m_connectionStatus);
    connectionLayout->addLayout(testLayout);

    mainLayout->addWidget(connectionGroup);

    // ========== 同步选项区域 ==========

    QGroupBox* syncOptionsGroup = new QGroupBox("同步选项", this);
    QVBoxLayout* optionsLayout = new QVBoxLayout(syncOptionsGroup);

    // 数据表选择
    QLabel* tablesLabel = new QLabel("选择要同步的数据表:", this);
    optionsLayout->addWidget(tablesLabel);

    QHBoxLayout* checkboxesLayout = new QHBoxLayout();

    m_syncStocks = new QCheckBox("股票列表 (stocks)", this);
    m_syncStocks->setChecked(true);  // 默认选中
    m_syncStocks->setToolTip("同步股票基本信息表");

    m_syncPrices = new QCheckBox("价格数据 (prices)", this);
    m_syncPrices->setChecked(true);
    m_syncPrices->setToolTip("同步股票历史价格数据（可能数据量较大）");

    m_syncTrades = new QCheckBox("交易数据 (trades)", this);
    m_syncTrades->setChecked(false);
    m_syncTrades->setToolTip("同步交易记录数据");

    m_syncAnalysis = new QCheckBox("分析结果 (analysis_results)", this);
    m_syncAnalysis->setChecked(true);
    m_syncAnalysis->setToolTip("同步策略分析结果");

    checkboxesLayout->addWidget(m_syncStocks);
    checkboxesLayout->addWidget(m_syncPrices);
    checkboxesLayout->addWidget(m_syncTrades);
    checkboxesLayout->addWidget(m_syncAnalysis);
    optionsLayout->addLayout(checkboxesLayout);

    // 增量同步选项
    m_incrementalSync = new QCheckBox("增量同步（仅同步新增数据）", this);
    m_incrementalSync->setChecked(true);
    m_incrementalSync->setToolTip("仅同步本地 SQLite 中新增的数据，避免重复同步已有数据");
    optionsLayout->addWidget(m_incrementalSync);

    mainLayout->addWidget(syncOptionsGroup);

    // ========== 同步控制区域 ==========

    QGroupBox* controlGroup = new QGroupBox("同步控制", this);
    QVBoxLayout* controlLayout = new QVBoxLayout(controlGroup);

    // 进度条和状态
    QHBoxLayout* progressLayout = new QHBoxLayout();
    m_syncProgress = new QProgressBar(this);
    m_syncProgress->setRange(0, 100);
    m_syncProgress->setValue(0);
    m_syncProgress->setTextVisible(true);
    m_syncProgress->setFormat("等待同步...");

    m_syncStatus = new QLabel("", this);
    m_syncStatus->setStyleSheet("color: #666666; font-size: 11px;");

    progressLayout->addWidget(m_syncProgress, 3);
    progressLayout->addWidget(m_syncStatus, 1);
    controlLayout->addLayout(progressLayout);

    // 控制按钮
    QHBoxLayout* buttonsLayout = new QHBoxLayout();

    m_startSyncBtn = new QPushButton("开始同步", this);
    m_startSyncBtn->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    m_startSyncBtn->setToolTip("开始将本地 SQLite 数据同步到 MySQL");
    m_startSyncBtn->setEnabled(false);  // 初始禁用，需要先测试连接
    connect(m_startSyncBtn, &QPushButton::clicked, this, [this]() {
        if (validateInput()) {
            // 收集选中的表
            QStringList tables;
            if (m_syncStocks->isChecked()) tables << "stocks";
            if (m_syncPrices->isChecked()) tables << "prices";
            if (m_syncTrades->isChecked()) tables << "trades";
            if (m_syncAnalysis->isChecked()) tables << "analysis_results";

            if (tables.isEmpty()) {
                QMessageBox::warning(this, "同步选项错误", "请至少选择一个数据表进行同步");
                return;
            }

            saveConfig();  // 保存配置
            emit syncRequested(
                m_hostEdit->text(),
                m_portSpinBox->value(),
                m_databaseEdit->text(),
                m_userEdit->text(),
                m_passwordEdit->text(),
                tables
            );
        }
    });

    m_stopSyncBtn = new QPushButton("停止同步", this);
    m_stopSyncBtn->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    m_stopSyncBtn->setEnabled(false);  // 未实现
    m_stopSyncBtn->setToolTip("功能未实现");

    buttonsLayout->addWidget(m_startSyncBtn);
    buttonsLayout->addWidget(m_stopSyncBtn);
    buttonsLayout->addStretch();
    controlLayout->addLayout(buttonsLayout);

    mainLayout->addWidget(controlGroup);

    // ========== 日志区域 ==========

    QLabel* logLabel = new QLabel("同步日志:", this);
    logLabel->setStyleSheet("font-weight: bold; margin-top: 10px;");
    mainLayout->addWidget(logLabel);

    m_syncLog = new QTextEdit(this);
    m_syncLog->setReadOnly(true);
    m_syncLog->setStyleSheet(
        "background-color: #1e1e1e; color: #d4d4d4; font-family: 'Courier New', monospace; font-size: 12px;"
    );
    m_syncLog->setMinimumHeight(150);
    m_syncLog->append("=== MySQL 同步面板已初始化 ===");
    m_syncLog->append("请配置 MySQL 连接信息，然后点击\"测试连接\"验证\n");
    mainLayout->addWidget(m_syncLog);

    // 清空日志按钮
    QHBoxLayout* logControlLayout = new QHBoxLayout();
    m_clearLogBtn = new QPushButton("清空日志", this);
    m_clearLogBtn->setIcon(style()->standardIcon(QStyle::SP_DialogResetButton));
    connect(m_clearLogBtn, &QPushButton::clicked, this, [this]() {
        m_syncLog->clear();
        m_syncLog->append("=== 日志已清空 ===");
        m_syncLog->append(QString("清空时间: %1\n")
                          .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")));
    });
    logControlLayout->addWidget(m_clearLogBtn);
    logControlLayout->addStretch();
    mainLayout->addLayout(logControlLayout);
}

void MySQLSyncPanel::loadSavedConfig() {
    QSettings settings("StockLens", "MySQLSync");

    QString host = settings.value("host", "localhost").toString();
    int port = settings.value("port", 3306).toInt();
    QString database = settings.value("database", "stock_db").toString();
    QString user = settings.value("user", "root").toString();
    QString password = settings.value("password", "").toString();

    m_hostEdit->setText(host);
    m_portSpinBox->setValue(port);
    m_databaseEdit->setText(database);
    m_userEdit->setText(user);
    m_passwordEdit->setText(password);

    appendSyncLog(QString("已加载保存的配置: %1:%2/%3 (用户: %4)")
                 .arg(host, QString::number(port), database, user));
}

void MySQLSyncPanel::saveConfig() {
    QSettings settings("StockLens", "MySQLSync");

    settings.setValue("host", m_hostEdit->text());
    settings.setValue("port", m_portSpinBox->value());
    settings.setValue("database", m_databaseEdit->text());
    settings.setValue("user", m_userEdit->text());
    settings.setValue("password", m_passwordEdit->text());

    qDebug() << "MySQL 配置已保存";
}

bool MySQLSyncPanel::validateInput() {
    if (m_hostEdit->text().isEmpty()) {
        QMessageBox::warning(this, "输入错误", "请输入 MySQL 主机地址");
        m_hostEdit->setFocus();
        return false;
    }

    if (m_databaseEdit->text().isEmpty()) {
        QMessageBox::warning(this, "输入错误", "请输入数据库名称");
        m_databaseEdit->setFocus();
        return false;
    }

    if (m_userEdit->text().isEmpty()) {
        QMessageBox::warning(this, "输入错误", "请输入 MySQL 用户名");
        m_userEdit->setFocus();
        return false;
    }

    return true;
}

void MySQLSyncPanel::setMySQLConfig(const QString& host, int port, const QString& database,
                                   const QString& user, const QString& password) {
    m_hostEdit->setText(host);
    m_portSpinBox->setValue(port);
    m_databaseEdit->setText(database);
    m_userEdit->setText(user);
    m_passwordEdit->setText(password);
}

QString MySQLSyncPanel::getMySQLHost() const {
    return m_hostEdit->text();
}

int MySQLSyncPanel::getMySQLPort() const {
    return m_portSpinBox->value();
}

QString MySQLSyncPanel::getMySQLDatabase() const {
    return m_databaseEdit->text();
}

QString MySQLSyncPanel::getMySQLUser() const {
    return m_userEdit->text();
}

QString MySQLSyncPanel::getMySQLPassword() const {
    return m_passwordEdit->text();
}

void MySQLSyncPanel::appendSyncLog(const QString& log, bool isError) {
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

    if (isError) {
        m_syncLog->append(QString("<span style='color: #f44336;'>[%1] [ERROR] %2</span>")
                         .arg(timestamp, log));
    } else {
        m_syncLog->append(QString("<span style='color: #4caf50;'>[%1] [INFO] %2</span>")
                         .arg(timestamp, log));
    }

    // 自动滚动到底部
    QScrollBar* scrollbar = m_syncLog->verticalScrollBar();
    scrollbar->setValue(scrollbar->maximum());
}

void MySQLSyncPanel::setSyncProgress(int value, const QString& statusText) {
    m_syncProgress->setValue(value);
    if (!statusText.isEmpty()) {
        m_syncProgress->setFormat(statusText);
    }
}

void MySQLSyncPanel::enableSyncButton(bool enable) {
    m_startSyncBtn->setEnabled(enable);
}