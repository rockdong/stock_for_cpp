#pragma once

#include <QWidget>
#include <QTextEdit>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDateTime>
#include <QTimer>

/**
 * @brief 任务管理面板 - 显示后台任务状态、日志和历史记录
 *
 * 功能：
 * - 显示任务运行状态（运行中、已停止、已完成）
 * - 实时显示任务输出日志
 * - 显示任务执行历史（时间戳、持续时间、结果）
 * - 提供启动/停止任务的快捷按钮
 */
class TaskManagementPanel : public QWidget {
    Q_OBJECT

public:
    explicit TaskManagementPanel(QWidget* parent = nullptr);
    ~TaskManagementPanel();

    /**
     * @brief 设置任务运行状态
     * @param running 是否正在运行
     */
    void setTaskRunning(bool running);

    /**
     * @brief 添加任务输出日志
     * @param output 输出内容
     * @param isError 是否为错误输出
     */
    void appendTaskOutput(const QString& output, bool isError = false);

    /**
     * @brief 添加任务历史记录
     * @param startTime 开始时间
     * @param duration 持续时间（秒）
     * @param success 是否成功
     * @param message 结果消息
     */
    void addTaskHistory(const QDateTime& startTime, int duration, bool success, const QString& message);

    /**
     * @brief 清空输出日志
     */
    void clearOutputLog();

signals:
    /**
     * @brief 启动任务信号
     */
    void startTaskRequested();

    /**
     * @brief 停止任务信号
     */
    void stopTaskRequested();

private:
    void setupUI();
    void updateStatusDisplay();

    // ========== UI 组件 ==========

    // 状态区域
    QLabel* m_statusLabel = nullptr;        // 状态文本：运行中/已停止
    QLabel* m_statusIcon = nullptr;         // 状态图标
    QProgressBar* m_progressBar = nullptr; // 进度条（任务运行时显示）

    // 输出日志区域
    QTextEdit* m_outputLog = nullptr;       // 实时输出日志
    QPushButton* m_clearLogButton = nullptr; // 清空日志按钮

    // 历史记录区域
    QTextEdit* m_historyLog = nullptr;      // 任务执行历史

    // 控制按钮
    QPushButton* m_startButton = nullptr;   // 启动任务按钮
    QPushButton* m_stopButton = nullptr;    // 停止任务按钮

    // ========== 状态数据 ==========

    bool m_taskRunning = false;             // 任务运行状态
    QDateTime m_taskStartTime;              // 任务开始时间
    QTimer* m_durationTimer = nullptr;      // 持续时间计时器
};