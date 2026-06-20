/**
 * @file ConfigWindow.h
 * @brief 配置管理独立窗口头文件
 * @author StockLens Team
 * @version 2.0.0
 */

#pragma once

#include <QMainWindow>
#include <QWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QString>

/**
 * @class ConfigWindow
 * @brief 配置管理独立窗口 - 单一职责: 配置管理
 * 
 * 负责功能:
 * - 编辑 .env 配置文件
 * - 配置策略参数
 * - 主题切换(浅色/深色)
 * - 保存和加载配置
 */
class ConfigWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit ConfigWindow(QWidget* parent = nullptr);
    ~ConfigWindow() = default;

signals:
    /**
     * @brief 主题切换信号 - 通知 ApplicationManager 更新全局主题
     * @param themeName 主题名称 ("light" 或 "dark")
     */
    void themeChanged(const QString& themeName);

private slots:
    /**
     * @brief 保存策略配置按钮点击
     */
    void onSaveStrategyBtn();

    /**
     * @brief 保存环境配置按钮点击
     */
    void onSaveEnvBtn();

    /**
     * @brief 主题切换按钮点击
     */
    void onThemeToggle();

private:
    /**
     * @brief 设置 UI 布局
     */
    void setupUI();

    /**
     * @brief 加载 .env 配置文件内容
     */
    void loadEnvConfig();

    /**
     * @brief 加载主题配置
     */
    void loadThemeConfig();

    /**
     * @brief 验证环境配置是否有效
     * @return true 配置有效
     * @return false 配置无效
     */
    bool validateEnvConfig();

    // ========== UI 组件 ==========

    // 环境配置编辑器
    QTextEdit* m_envEditor = nullptr;

    // 策略参数输入框
    QLineEdit* m_strategyNameInput = nullptr;
    QLineEdit* m_strategyParamsInput = nullptr;

    // 主题切换下拉框
    QComboBox* m_themeComboBox = nullptr;

    // 保存按钮
    QPushButton* m_saveEnvBtn = nullptr;
    QPushButton* m_saveStrategyBtn = nullptr;
    QPushButton* m_toggleThemeBtn = nullptr;

    // 配置文件路径
    QString m_envFilePath;
};