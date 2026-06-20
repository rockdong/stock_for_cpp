#include "ConfigPanel.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QSettings>
#include <QCoreApplication>
#include <QDebug>

ConfigPanel::ConfigPanel(QWidget* parent)
    : QWidget(parent)
{
    setupUI();
    loadEnvConfig();
    loadThemeConfig();
}

ConfigPanel::~ConfigPanel() {
    // 析构时无需特殊处理
}

void ConfigPanel::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    // 使用 TabWidget 组织不同的配置区域
    QTabWidget* configTabs = new QTabWidget(this);

    // ========== Tab 1: .env 配置 ==========

    QWidget* envTab = new QWidget();
    QVBoxLayout* envLayout = new QVBoxLayout(envTab);

    QLabel* envLabel = new QLabel("环境配置 (.env 文件)", this);
    envLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    envLayout->addWidget(envLabel);

    m_envEditor = new QTextEdit(this);
    m_envEditor->setStyleSheet(
        "background-color: #f5f5f5; color: #333333; font-family: 'Courier New', monospace; font-size: 12px;"
    );
    m_envEditor->setPlaceholderText("加载 .env 配置文件...");
    envLayout->addWidget(m_envEditor);

    QHBoxLayout* envButtonsLayout = new QHBoxLayout();

    m_reloadEnvBtn = new QPushButton("重新加载", this);
    m_reloadEnvBtn->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));
    m_reloadEnvBtn->setToolTip("重新加载 .env 文件内容");
    connect(m_reloadEnvBtn, &QPushButton::clicked, this, [this]() {
        loadEnvConfig();
    });

    m_saveEnvBtn = new QPushButton("保存配置", this);
    m_saveEnvBtn->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));
    m_saveEnvBtn->setToolTip("保存 .env 配置文件");
    m_saveEnvBtn->setStyleSheet("background-color: #4caf50; color: white; font-weight: bold;");
    connect(m_saveEnvBtn, &QPushButton::clicked, this, [this]() {
        if (validateEnvConfig()) {
            if (saveConfigFile(m_envFilePath)) {
                QMessageBox::information(this, "保存成功", "环境配置已保存，部分配置需要重启应用生效");
            }
        }
    });

    envButtonsLayout->addWidget(m_reloadEnvBtn);
    envButtonsLayout->addWidget(m_saveEnvBtn);
    envButtonsLayout->addStretch();
    envLayout->addLayout(envButtonsLayout);

    configTabs->addTab(envTab, "环境配置");

    // ========== Tab 2: 策略配置 ==========

    QWidget* strategyTab = new QWidget();
    QVBoxLayout* strategyLayout = new QVBoxLayout(strategyTab);

    QLabel* strategyLabel = new QLabel("策略配置", this);
    strategyLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    strategyLayout->addWidget(strategyLabel);

    m_strategyEditor = new QTextEdit(this);
    m_strategyEditor->setStyleSheet(
        "background-color: #f5f5f5; color: #333333; font-family: 'Courier New', monospace; font-size: 12px;"
    );
    m_strategyEditor->setPlaceholderText("策略配置文件（预留功能）...");
    m_strategyEditor->append("# 策略配置文件\n");
    m_strategyEditor->append("# EMA17Breakout:\n");
    m_strategyEditor->append("#   ema_period: 17\n");
    m_strategyEditor->append("#   threshold: 0.02\n");
    m_strategyEditor->append("\n");
    m_strategyEditor->append("# MACD:\n");
    m_strategyEditor->append("#   fast_period: 12\n");
    m_strategyEditor->append("#   slow_period: 26\n");
    m_strategyEditor->append("#   signal_period: 9\n");
    strategyLayout->addWidget(m_strategyEditor);

    QHBoxLayout* strategyButtonsLayout = new QHBoxLayout();

    m_saveStrategyBtn = new QPushButton("保存策略配置", this);
    m_saveStrategyBtn->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));
    m_saveStrategyBtn->setToolTip("保存策略配置文件");
    m_saveStrategyBtn->setStyleSheet("background-color: #4caf50; color: white; font-weight: bold;");
    connect(m_saveStrategyBtn, &QPushButton::clicked, this, &ConfigPanel::onSaveStrategyBtn);

    strategyButtonsLayout->addWidget(m_saveStrategyBtn);
    strategyButtonsLayout->addStretch();
    strategyLayout->addLayout(strategyButtonsLayout);

    configTabs->addTab(strategyTab, "策略配置");

    // ========== Tab 3: 主题和日志设置 ==========

    QWidget* settingsTab = new QWidget();
    QVBoxLayout* settingsLayout = new QVBoxLayout(settingsTab);

    // 主题设置
    QGroupBox* themeGroup = new QGroupBox("主题设置", this);
    QHBoxLayout* themeLayout = new QHBoxLayout(themeGroup);

    QLabel* themeLabel = new QLabel("界面主题:", this);
    m_themeCombo = new QComboBox(this);
    m_themeCombo->addItem("浅色主题", "light");
    m_themeCombo->addItem("深色主题", "dark");
    m_themeCombo->addItem("跟随系统", "system");

    m_applyThemeBtn = new QPushButton("应用主题", this);
    m_applyThemeBtn->setStyleSheet("background-color: #2196f3; color: white;");
    connect(m_applyThemeBtn, &QPushButton::clicked, this, [this]() {
        QString theme = m_themeCombo->currentData().toString();
        emit themeChanged(theme);
        QSettings settings("StockLens", "StockUI");
        settings.setValue("theme", theme);
        QMessageBox::information(this, "主题切换", QString("已切换到 %1 主题").arg(m_themeCombo->currentText()));
    });

    themeLayout->addWidget(themeLabel);
    themeLayout->addWidget(m_themeCombo);
    themeLayout->addWidget(m_applyThemeBtn);
    themeLayout->addStretch();
    settingsLayout->addWidget(themeGroup);

    // 日志设置
    QGroupBox* logGroup = new QGroupBox("日志设置", this);
    QVBoxLayout* logLayout = new QVBoxLayout(logGroup);

    QHBoxLayout* logLevelLayout = new QHBoxLayout();
    QLabel* logLevelLabel = new QLabel("日志级别:", this);
    m_logLevelCombo = new QComboBox(this);
    m_logLevelCombo->addItem("DEBUG (调试)", "DEBUG");
    m_logLevelCombo->addItem("INFO (信息)", "INFO");
    m_logLevelCombo->addItem("WARNING (警告)", "WARNING");
    m_logLevelCombo->addItem("ERROR (错误)", "ERROR");
    m_logLevelCombo->setCurrentIndex(1);  // 默认 INFO

    logLevelLayout->addWidget(logLevelLabel);
    logLevelLayout->addWidget(m_logLevelCombo);
    logLevelLayout->addStretch();
    logLayout->addLayout(logLevelLayout);

    QHBoxLayout* logSwitchesLayout = new QHBoxLayout();
    m_logConsoleCheck = new QCheckBox("启用控制台日志", this);
    m_logConsoleCheck->setChecked(true);
    m_logFileCheck = new QCheckBox("启用文件日志", this);
    m_logFileCheck->setChecked(false);  // Qt UI 默认禁用文件日志（沙箱问题）

    logSwitchesLayout->addWidget(m_logConsoleCheck);
    logSwitchesLayout->addWidget(m_logFileCheck);
    logSwitchesLayout->addStretch();
    logLayout->addLayout(logSwitchesLayout);

    settingsLayout->addWidget(logGroup);

    // 说明文本
    QLabel* noteLabel = new QLabel(
        "说明：日志设置仅在应用启动时生效，修改后需要重启应用。\n"
        "Qt UI 应用建议禁用文件日志以避免 macOS 沙箱权限问题。",
        this
    );
    noteLabel->setStyleSheet("color: #666666; font-size: 11px; margin-top: 10px;");
    settingsLayout->addWidget(noteLabel);

    settingsLayout->addStretch();
    configTabs->addTab(settingsTab, "主题和日志");

    mainLayout->addWidget(configTabs);

    // 设置配置文件路径
    m_envFilePath = QCoreApplication::applicationDirPath() + "/../../../cpp/.env";
    m_strategyFilePath = QCoreApplication::applicationDirPath() + "/../../../cpp/config/strategies.yaml";
}

void ConfigPanel::loadEnvConfig() {
    QFile envFile(m_envFilePath);
    if (envFile.exists() && envFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&envFile);
        QString content = in.readAll();
        envFile.close();

        m_envEditor->clear();
        m_envEditor->setPlainText(content);

        qDebug() << "已加载 .env 配置文件：" << m_envFilePath;
    } else {
        m_envEditor->clear();
        m_envEditor->append("# .env 配置文件不存在或无法读取\n");
        m_envEditor->append(QString("# 文件路径：%1\n").arg(m_envFilePath));
        m_envEditor->append("\n");
        m_envEditor->append("# 示例配置：\n");
        m_envEditor->append("DB_TYPE=sqlite\n");
        m_envEditor->append("DB_PATH=stock_db.db\n");
        m_envEditor->append("\n");
        m_envEditor->append("LOG_LEVEL=INFO\n");
        m_envEditor->append("LOG_CONSOLE_ENABLED=true\n");
        m_envEditor->append("LOG_FILE_ENABLED=false\n");

        qWarning() << "无法加载 .env 文件：" << m_envFilePath;
    }
}

void ConfigPanel::loadThemeConfig() {
    QSettings settings("StockLens", "StockUI");
    QString currentTheme = settings.value("theme", "light").toString();

    // 设置下拉框选中项
    for (int i = 0; i < m_themeCombo->count(); ++i) {
        if (m_themeCombo->itemData(i).toString() == currentTheme) {
            m_themeCombo->setCurrentIndex(i);
            break;
        }
    }

    qDebug() << "当前主题：" << currentTheme;
}

void ConfigPanel::loadConfigFile(const QString& filePath) {
    QFile file(filePath);
    if (file.exists() && file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString content = in.readAll();
        file.close();

        m_envEditor->setPlainText(content);
        m_envFilePath = filePath;
    }
}

bool ConfigPanel::saveConfigFile(const QString& filePath) {
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << m_envEditor->toPlainText();
        file.close();

        qDebug() << "配置文件已保存：" << filePath;
        emit configSaved(filePath);
        return true;
    } else {
        QMessageBox::warning(this, "保存失败", QString("无法写入文件：%1").arg(filePath));
        return false;
    }
}

bool ConfigPanel::validateEnvConfig() {
    QString content = m_envEditor->toPlainText();

    // 简单验证：检查是否包含必要的配置项
    if (!content.contains("DB_TYPE")) {
        QMessageBox::warning(this, "配置验证失败", ".env 文件缺少 DB_TYPE 配置项");
        return false;
    }

    if (!content.contains("LOG_LEVEL")) {
        QMessageBox::warning(this, "配置验证失败", ".env 文件缺少 LOG_LEVEL 配置项");
        return false;
    }

    return true;
}

void ConfigPanel::onSaveStrategyBtn() {
    QMessageBox::information(this, "预留功能", "策略配置保存功能将在后续版本实现");
}