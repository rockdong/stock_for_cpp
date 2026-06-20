/**
 * @file ConfigWindow.cpp
 * @brief 配置管理独立窗口实现
 * @author StockLens Team
 * @version 2.0.0
 */

#include "ConfigWindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QSettings>
#include <QFile>
#include <QTextStream>
#include <QStatusBar>      // QStatusBar 完整定义
#include <QMessageBox>
#include <QCoreApplication>
#include <QDir>
#include <QDebug>

ConfigWindow::ConfigWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("配置管理 - Stock for C++");
    resize(600, 500);

    // 初始化配置文件路径
    QString appDir = QCoreApplication::applicationDirPath();
    m_envFilePath = appDir + "/../../../.env";

    setupUI();
    loadEnvConfig();
    loadThemeConfig();

    statusBar()->showMessage("就绪");
}

void ConfigWindow::setupUI() {
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(10);

    // ========== 环境配置面板 ==========

    QGroupBox* envGroup = new QGroupBox("环境配置 (.env)", this);
    QVBoxLayout* envLayout = new QVBoxLayout(envGroup);

    m_envEditor = new QTextEdit(this);
    m_envEditor->setPlaceholderText("在此编辑 .env 配置文件内容...\n例如:\nDB_NAME=stock_db.db\nLOG_LEVEL=INFO");
    envLayout->addWidget(m_envEditor);

    m_saveEnvBtn = new QPushButton("保存环境配置", this);
    m_saveEnvBtn->setStyleSheet("background-color: #0078d4; color: white; padding: 8px;");
    connect(m_saveEnvBtn, &QPushButton::clicked, this, &ConfigWindow::onSaveEnvBtn);
    envLayout->addWidget(m_saveEnvBtn);

    mainLayout->addWidget(envGroup);

    // ========== 策略参数面板 ==========

    QGroupBox* strategyGroup = new QGroupBox("策略参数配置", this);
    QVBoxLayout* strategyLayout = new QVBoxLayout(strategyGroup);

    QHBoxLayout* nameLayout = new QHBoxLayout();
    nameLayout->addWidget(new QLabel("策略名称:", this));
    m_strategyNameInput = new QLineEdit(this);
    m_strategyNameInput->setPlaceholderText("输入策略名称");
    nameLayout->addWidget(m_strategyNameInput);
    strategyLayout->addLayout(nameLayout);

    QHBoxLayout* paramsLayout = new QHBoxLayout();
    paramsLayout->addWidget(new QLabel("策略参数:", this));
    m_strategyParamsInput = new QLineEdit(this);
    m_strategyParamsInput->setPlaceholderText("输入策略参数(JSON格式)");
    paramsLayout->addWidget(m_strategyParamsInput);
    strategyLayout->addLayout(paramsLayout);

    m_saveStrategyBtn = new QPushButton("保存策略配置", this);
    m_saveStrategyBtn->setStyleSheet("background-color: #107c10; color: white; padding: 8px;");
    connect(m_saveStrategyBtn, &QPushButton::clicked, this, &ConfigWindow::onSaveStrategyBtn);
    strategyLayout->addWidget(m_saveStrategyBtn);

    mainLayout->addWidget(strategyGroup);

    // ========== 主题切换面板 ==========

    QGroupBox* themeGroup = new QGroupBox("主题设置", this);
    QHBoxLayout* themeLayout = new QHBoxLayout(themeGroup);

    themeLayout->addWidget(new QLabel("当前主题:", this));
    m_themeComboBox = new QComboBox(this);
    m_themeComboBox->addItem("浅色主题", "light");
    m_themeComboBox->addItem("深色主题", "dark");
    themeLayout->addWidget(m_themeComboBox);

    m_toggleThemeBtn = new QPushButton("应用主题", this);
    m_toggleThemeBtn->setStyleSheet("background-color: #881798; color: white; padding: 8px;");
    connect(m_toggleThemeBtn, &QPushButton::clicked, this, &ConfigWindow::onThemeToggle);
    themeLayout->addWidget(m_toggleThemeBtn);

    themeLayout->addStretch();
    mainLayout->addWidget(themeGroup);

    setCentralWidget(centralWidget);
}

void ConfigWindow::loadEnvConfig() {
    QFile envFile(m_envFilePath);
    if (envFile.exists() && envFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&envFile);
        QString envContent = in.readAll();
        envFile.close();
        m_envEditor->setText(envContent);
        statusBar()->showMessage(QString("已加载配置文件: %1").arg(m_envFilePath));
    } else {
        m_envEditor->setPlaceholderText(QString("未找到 .env 文件 (%1)\n\n请创建配置文件").arg(m_envFilePath));
        statusBar()->showMessage("未找到配置文件");
    }
}

void ConfigWindow::loadThemeConfig() {
    QSettings settings("StockLens", "StockUI");
    QString currentTheme = settings.value("theme", "light").toString();

    int index = m_themeComboBox->findData(currentTheme);
    if (index >= 0) {
        m_themeComboBox->setCurrentIndex(index);
    }
}

void ConfigWindow::onSaveEnvBtn() {
    if (!validateEnvConfig()) {
        QMessageBox::warning(this, "配置验证失败", "请检查配置文件格式是否正确");
        return;
    }

    QFile envFile(m_envFilePath);
    if (envFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&envFile);
        out << m_envEditor->toPlainText();
        envFile.close();
        QMessageBox::information(this, "保存成功", QString("环境配置已保存到: %1").arg(m_envFilePath));
        statusBar()->showMessage("环境配置保存成功");
    } else {
        QMessageBox::critical(this, "保存失败", QString("无法保存到文件: %1\n\n错误: %2")
                              .arg(m_envFilePath)
                              .arg(envFile.errorString()));
    }
}

void ConfigWindow::onSaveStrategyBtn() {
    QString strategyName = m_strategyNameInput->text();
    QString strategyParams = m_strategyParamsInput->text();

    if (strategyName.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "请输入策略名称");
        return;
    }

    // 保存策略配置到 QSettings
    QSettings settings("StockLens", "StockUI");
    settings.beginGroup("Strategies");
    settings.setValue(strategyName + "/params", strategyParams);
    settings.endGroup();

    QMessageBox::information(this, "保存成功", QString("策略 '%1' 配置已保存").arg(strategyName));
    statusBar()->showMessage(QString("策略配置保存成功: %1").arg(strategyName));
}

void ConfigWindow::onThemeToggle() {
    QString selectedTheme = m_themeComboBox->currentData().toString();

    // 发送主题切换信号
    emit themeChanged(selectedTheme);

    QMessageBox::information(this, "主题切换", QString("已切换到 %1 主题").arg(
        selectedTheme == "light" ? "浅色" : "深色"));
    statusBar()->showMessage(QString("主题已切换: %1").arg(selectedTheme));
}

bool ConfigWindow::validateEnvConfig() {
    QString envContent = m_envEditor->toPlainText();

    if (envContent.isEmpty()) {
        return true;  // 空配置也是合法的
    }

    // 简单验证: 检查是否包含非法字符或格式错误
    QStringList lines = envContent.split('\n');
    for (const QString& line : lines) {
        QString trimmedLine = line.trimmed();

        // 跳过空行和注释
        if (trimmedLine.isEmpty() || trimmedLine.startsWith('#')) {
            continue;
        }

        // 检查是否包含 '=' 分隔符
        if (!trimmedLine.contains('=')) {
            return false;
        }

        // 检查格式: KEY=VALUE
        int equalPos = trimmedLine.indexOf('=');
        QString key = trimmedLine.left(equalPos).trimmed();
        QString value = trimmedLine.mid(equalPos + 1).trimmed();

        if (key.isEmpty()) {
            return false;
        }
    }

    return true;
}