#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QTextEdit>
#include <QProgressBar>
#include <QLabel>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDateTime>

/**
 * @brief MySQL 数据同步面板 - 同步本地 SQLite 数据到线上 MySQL 数据库
 *
 * 功能：
 * - MySQL 连接配置（主机、端口、数据库、用户名、密码）
 * - 选择要同步的数据表（stocks, prices, trades, analysis_results）
 * - 测试 MySQL 连接
 * - 执行数据同步（从 SQLite 到 MySQL）
 * - 显示同步进度和日志
 * - 支持断点续传（增量同步）
 */
class MySQLSyncPanel : public QWidget {
    Q_OBJECT

public:
    explicit MySQLSyncPanel(QWidget* parent = nullptr);
    ~MySQLSyncPanel();

    /**
     * @brief 设置 MySQL 连接配置
     */
    void setMySQLConfig(const QString& host, int port, const QString& database,
                       const QString& user, const QString& password);

    /**
     * @brief 获取 MySQL 连接配置
     */
    QString getMySQLHost() const;
    int getMySQLPort() const;
    QString getMySQLDatabase() const;
    QString getMySQLUser() const;
    QString getMySQLPassword() const;

    /**
     * @brief 添加同步日志
     */
    void appendSyncLog(const QString& log, bool isError = false);

    /**
     * @brief 设置同步进度
     */
    void setSyncProgress(int value, const QString& statusText = "");

    /**
     * @brief 启用/禁用开始同步按钮
     */
    void enableSyncButton(bool enable);

signals:
    /**
     * @brief 测试 MySQL 连接信号
     */
    void testConnectionRequested(const QString& host, int port, const QString& database,
                                const QString& user, const QString& password);

    /**
     * @brief 开始同步信号
     */
    void syncRequested(const QString& host, int port, const QString& database,
                      const QString& user, const QString& password,
                      const QStringList& tables);

    /**
     * @brief 同步完成信号
     */
    void syncCompleted(bool success, const QString& message);

private:
    void setupUI();
    void loadSavedConfig();
    void saveConfig();
    bool validateInput();

    // ========== 连接配置区域 ==========

    QLineEdit* m_hostEdit = nullptr;       // MySQL 主机地址
    QSpinBox* m_portSpinBox = nullptr;     // MySQL 端口
    QLineEdit* m_databaseEdit = nullptr;   // 数据库名称
    QLineEdit* m_userEdit = nullptr;       // 用户名
    QLineEdit* m_passwordEdit = nullptr;   // 密码
    QPushButton* m_testConnectionBtn = nullptr;  // 测试连接按钮
    QLabel* m_connectionStatus = nullptr;  // 连接状态显示

    // ========== 同步选项区域 ==========

    QCheckBox* m_syncStocks = nullptr;     // 同步股票列表
    QCheckBox* m_syncPrices = nullptr;     // 同步价格数据
    QCheckBox* m_syncTrades = nullptr;     // 同步交易数据
    QCheckBox* m_syncAnalysis = nullptr;   // 同步分析结果
    QCheckBox* m_incrementalSync = nullptr;  // 增量同步（仅同步新增数据）

    // ========== 同步控制区域 ==========

    QPushButton* m_startSyncBtn = nullptr;  // 开始同步按钮
    QPushButton* m_stopSyncBtn = nullptr;   // 停止同步按钮（未实现）
    QProgressBar* m_syncProgress = nullptr;  // 同步进度条
    QLabel* m_syncStatus = nullptr;          // 同步状态文本

    // ========== 日志区域 ==========

    QTextEdit* m_syncLog = nullptr;         // 同步日志
    QPushButton* m_clearLogBtn = nullptr;   // 清空日志按钮

    // ========== 状态数据 ==========

    bool m_connectionTested = false;        // 连接是否已测试
    bool m_syncInProgress = false;          // 是否正在同步
};