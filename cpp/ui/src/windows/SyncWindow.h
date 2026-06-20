/**
 * @file SyncWindow.h
 * @brief 数据库同步独立窗口头文件
 * @author StockLens Team
 * @version 2.0.0
 */

#pragma once

#include <QMainWindow>
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QProgressBar>
#include <QSpinBox>
#include <QString>

/**
 * @class SyncWindow
 * @brief 数据库同步独立窗口 - 单一职责: MySQL 数据库同步
 * 
 * 负责功能:
 * - MySQL 连接配置
 * - 连接测试
 * - 执行数据同步
 * - 显示同步进度和日志
 */
class SyncWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit SyncWindow(QWidget* parent = nullptr);
    ~SyncWindow() = default;

private slots:
    /**
     * @brief 测试 MySQL 连接按钮点击
     */
    void onTestConnection();

    /**
     * @brief 开始同步按钮点击
     */
    void onStartSync();

private:
    /**
     * @brief 设置 UI 布局
     */
    void setupUI();

    /**
     * @brief 加载 MySQL 配置
     */
    void loadMySQLConfig();

    /**
     * @brief 保存 MySQL 配置
     */
    void saveMySQLConfig();

    /**
     * @brief 模拟同步进度更新 (演示用途)
     */
    void onSyncProgressUpdated();

    // ========== UI 组件 ==========

    // MySQL 连接配置输入框
    QLineEdit* m_hostInput = nullptr;
    QSpinBox* m_portSpinBox = nullptr;
    QLineEdit* m_userInput = nullptr;
    QLineEdit* m_passwordInput = nullptr;
    QLineEdit* m_databaseInput = nullptr;

    // 测试和同步按钮
    QPushButton* m_testConnectionBtn = nullptr;
    QPushButton* m_startSyncBtn = nullptr;

    // 同步进度显示
    QProgressBar* m_progressBar = nullptr;
    QTextEdit* m_syncLogOutput = nullptr;

    // 同步状态
    bool m_isSyncing = false;
};