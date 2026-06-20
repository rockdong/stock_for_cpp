#pragma once

#include <QWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QGroupBox>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>

/**
 * @brief 配置面板 - 编辑系统配置文件和环境变量
 *
 * 功能：
 * - 编辑 .env 配置文件（数据库、API、日志等配置）
 * - 编辑策略配置文件（分析策略参数）
 * - 主题切换（浅色/深色）
 * - 日志级别设置
 * - 数据源配置
 */
class ConfigPanel : public QWidget {
    Q_OBJECT

public:
    explicit ConfigPanel(QWidget* parent = nullptr);
    ~ConfigPanel();

    /**
     * @brief 加载配置文件内容
     */
    void loadConfigFile(const QString& filePath);

    /**
     * @brief 保存配置文件内容
     */
    bool saveConfigFile(const QString& filePath);

signals:
    /**
     * @brief 配置已保存信号
     */
    void configSaved(const QString& filePath);

    /**
     * @brief 主题切换信号
     */
    void themeChanged(const QString& theme);

private slots:
    /**
     * @brief 保存策略配置按钮点击槽函数
     */
    void onSaveStrategyBtn();

private:
    void setupUI();
    void loadEnvConfig();
    void loadThemeConfig();
    bool validateEnvConfig();

    // ========== .env 配置区域 ==========

    QTextEdit* m_envEditor = nullptr;      // .env 文件编辑器
    QPushButton* m_saveEnvBtn = nullptr;   // 保存 .env 按钮
    QPushButton* m_reloadEnvBtn = nullptr; // 重新加载 .env 按钮

    // ========== 策略配置区域 ==========

    QTextEdit* m_strategyEditor = nullptr;  // 策略配置编辑器
    QPushButton* m_saveStrategyBtn = nullptr; // 保存策略配置按钮

    // ========== 主题设置区域 ==========

    QComboBox* m_themeCombo = nullptr;      // 主题选择下拉框
    QPushButton* m_applyThemeBtn = nullptr; // 应用主题按钮

    // ========== 日志设置区域 ==========

    QComboBox* m_logLevelCombo = nullptr;   // 日志级别选择
    QCheckBox* m_logConsoleCheck = nullptr; // 控制台日志开关
    QCheckBox* m_logFileCheck = nullptr;    // 文件日志开关

    // ========== 配置文件路径 ==========

    QString m_envFilePath;
    QString m_strategyFilePath;
};