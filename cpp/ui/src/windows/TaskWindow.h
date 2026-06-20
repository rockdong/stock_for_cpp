/**
 * @file TaskWindow.h
 * @brief 任务管理独立窗口头文件
 * @author StockLens Team
 * @version 2.0.0
 */

#pragma once

#include <QMainWindow>
#include <QWidget>
#include <QProcess>
#include <QTextEdit>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QDateTime>

namespace data {
    class StockDAO;
    class PriceDAO;
}

/**
 * @class TaskWindow
 * @brief 任务管理独立窗口 - 单一职责: 任务管理
 * 
 * 负责功能:
 * - 启动后台分析任务
 * - 监控任务运行状态
 * - 显示任务实时日志
 * - 记录任务执行历史
 * - 停止正在运行的任务
 */
class TaskWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit TaskWindow(QWidget* parent = nullptr);
    ~TaskWindow();

private slots:
    /**
     * @brief 启动任务按钮点击
     */
    void onStartTask();

    /**
     * @brief 停止任务按钮点击
     */
    void onStopTask();

    /**
     * @brief 任务进程完成回调
     * @param exitCode 退出码
     * @param exitStatus 退出状态
     */
    void onTaskFinished(int exitCode, QProcess::ExitStatus exitStatus);

    /**
     * @brief 任务进程错误回调
     * @param error 进程错误类型
     */
    void onTaskError(QProcess::ProcessError error);

    /**
     * @brief 任务进程输出回调 (标准输出和标准错误)
     */
    void onTaskOutput();

private:
    /**
     * @brief 设置 UI 布局
     */
    void setupUI();

    /**
     * @brief 设置信号连接
     */
    void setupConnections();

    /**
     * @brief 添加任务历史记录
     * @param startTime 开始时间
     * @param duration 运行时长(秒)
     * @param success 是否成功
     * @param message 结果消息
     */
    void addTaskHistory(const QDateTime& startTime, int duration, bool success, const QString& message);

    // ========== UI 组件 ==========

    // 任务状态显示
    QLabel* m_statusLabel = nullptr;
    QLabel* m_durationLabel = nullptr;

    // 任务控制按钮
    QPushButton* m_startButton = nullptr;
    QPushButton* m_stopButton = nullptr;

    // 任务日志输出
    QTextEdit* m_logOutput = nullptr;

    // 任务历史记录列表
    QListWidget* m_historyList = nullptr;

    // ========== 后台进程管理 ==========

    QProcess* m_taskProcess = nullptr;  // 后台分析任务进程
    bool m_taskRunning = false;         // 任务运行状态标志
    QDateTime m_taskStartTime;          // 任务开始时间
};