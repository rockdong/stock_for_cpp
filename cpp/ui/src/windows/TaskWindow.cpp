/**
 * @file TaskWindow.cpp
 * @brief 任务管理独立窗口实现
 * @author StockLens Team
 * @version 2.0.0
 */

#include "TaskWindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFileInfo>
#include <QStatusBar>      // QStatusBar 完整定义
#include <QMessageBox>
#include <QCoreApplication>
#include <QDebug>

TaskWindow::TaskWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("任务管理 - Stock for C++");
    resize(800, 600);

    setupUI();
    setupConnections();

    statusBar()->showMessage("就绪");
}

TaskWindow::~TaskWindow() {
    // 停止正在运行的任务
    if (m_taskRunning && m_taskProcess) {
        m_taskProcess->terminate();
        m_taskProcess->waitForFinished(2000);
    }
}

void TaskWindow::setupUI() {
    // ========== 主布局 ==========

    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(10);

    // ========== 1. 任务状态面板 ==========

    QGroupBox* statusGroup = new QGroupBox("任务状态", this);
    QHBoxLayout* statusLayout = new QHBoxLayout(statusGroup);

    m_statusLabel = new QLabel("未运行", this);
    m_statusLabel->setStyleSheet("font-weight: bold; color: #555555;");
    statusLayout->addWidget(new QLabel("当前状态:", this));
    statusLayout->addWidget(m_statusLabel);

    m_durationLabel = new QLabel("--", this);
    statusLayout->addWidget(new QLabel("运行时间:", this));
    statusLayout->addWidget(m_durationLabel);

    statusLayout->addStretch();
    mainLayout->addWidget(statusGroup);

    // ========== 2. 任务控制按钮 ==========

    QGroupBox* controlGroup = new QGroupBox("任务控制", this);
    QHBoxLayout* controlLayout = new QHBoxLayout(controlGroup);

    m_startButton = new QPushButton("启动分析任务", this);
    m_startButton->setStyleSheet("background-color: #0078d4; color: white; padding: 10px;");
    controlLayout->addWidget(m_startButton);

    m_stopButton = new QPushButton("停止任务", this);
    m_stopButton->setStyleSheet("background-color: #d83b01; color: white; padding: 10px;");
    m_stopButton->setEnabled(false);  // 初始禁用
    controlLayout->addWidget(m_stopButton);

    controlLayout->addStretch();
    mainLayout->addWidget(controlGroup);

    // ========== 3. 任务实时日志 ==========

    QGroupBox* logGroup = new QGroupBox("任务日志", this);
    QVBoxLayout* logLayout = new QVBoxLayout(logGroup);

    m_logOutput = new QTextEdit(this);
    m_logOutput->setReadOnly(true);
    m_logOutput->setStyleSheet("background-color: #1e1e1e; color: #e0e0e0; font-family: monospace;");
    logLayout->addWidget(m_logOutput);

    mainLayout->addWidget(logGroup);

    // ========== 4. 任务历史记录 ==========

    QGroupBox* historyGroup = new QGroupBox("任务历史", this);
    QVBoxLayout* historyLayout = new QVBoxLayout(historyGroup);

    m_historyList = new QListWidget(this);
    m_historyList->setMaximumHeight(150);
    historyLayout->addWidget(m_historyList);

    mainLayout->addWidget(historyGroup);

    // 设置主窗口中心部件
    setCentralWidget(centralWidget);
}

void TaskWindow::setupConnections() {
    // 连接任务控制按钮
    connect(m_startButton, &QPushButton::clicked, this, &TaskWindow::onStartTask);
    connect(m_stopButton, &QPushButton::clicked, this, &TaskWindow::onStopTask);
}

void TaskWindow::onStartTask() {
    // 检查是否已有任务在运行
    if (m_taskRunning && m_taskProcess) {
        QMessageBox::warning(this, "任务已运行", "已有分析任务在后台运行,请先停止当前任务");
        return;
    }

    // 创建后台进程
    if (!m_taskProcess) {
        m_taskProcess = new QProcess(this);
        connect(m_taskProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                this, &TaskWindow::onTaskFinished);
        connect(m_taskProcess, &QProcess::errorOccurred, this, &TaskWindow::onTaskError);
        connect(m_taskProcess, &QProcess::readyReadStandardOutput, this, &TaskWindow::onTaskOutput);
        connect(m_taskProcess, &QProcess::readyReadStandardError, this, &TaskWindow::onTaskOutput);
    }

    // 获取主程序路径
    QString appDir = QCoreApplication::applicationDirPath();
    QString programPath = appDir + "/../../../cpp/build/stock_for_cpp";

    // 验证可执行文件是否存在
    QFileInfo programInfo(programPath);
    if (!programInfo.exists() || !programInfo.isExecutable()) {
        QString errorMsg = QString("可执行文件不存在或无法执行:\n%1\n\n请先构建项目: cd cpp/build && make")
                          .arg(programPath);
        QMessageBox::critical(this, "启动失败", errorMsg);
        m_logOutput->append(errorMsg);
        return;
    }

    // 构建启动参数
    QStringList arguments;
    arguments << "--once";

    m_logOutput->append("=== 启动后台分析任务 ===");
    m_logOutput->append(QString("执行路径: %1").arg(programInfo.absoluteFilePath()));
    m_logOutput->append(QString("执行参数: --once"));
    m_logOutput->append("---");

    // 记录任务开始时间
    m_taskStartTime = QDateTime::currentDateTime();

    // 更新 UI 状态
    m_statusLabel->setText("运行中");
    m_statusLabel->setStyleSheet("font-weight: bold; color: #0078d4;");
    m_startButton->setEnabled(false);
    m_stopButton->setEnabled(true);
    statusBar()->showMessage("分析任务已启动");

    // 启动进程
    m_taskProcess->start(programInfo.absoluteFilePath(), arguments);

    if (m_taskProcess->waitForStarted(3000)) {
        m_taskRunning = true;
    } else {
        QMessageBox::critical(this, "启动失败",
            QString("无法启动分析任务：%1\n\n错误：%2")
            .arg(programPath)
            .arg(m_taskProcess->errorString()));

        m_statusLabel->setText("启动失败");
        m_statusLabel->setStyleSheet("font-weight: bold; color: #d83b01;");
        m_startButton->setEnabled(true);
        m_stopButton->setEnabled(false);
    }
}

void TaskWindow::onStopTask() {
    if (!m_taskRunning || !m_taskProcess) {
        return;
    }

    m_logOutput->append("正在停止分析任务...");

    // 优雅终止进程
    m_taskProcess->terminate();

    // 等待进程结束
    if (!m_taskProcess->waitForFinished(5000)) {
        m_logOutput->append("进程未响应终止信号,强制终止");
        m_taskProcess->kill();
        m_taskProcess->waitForFinished(2000);
    }

    m_taskRunning = false;
    m_statusLabel->setText("已停止");
    m_statusLabel->setStyleSheet("font-weight: bold; color: #d83b01;");
    m_startButton->setEnabled(true);
    m_stopButton->setEnabled(false);

    statusBar()->showMessage("分析任务已停止");
}

void TaskWindow::onTaskFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    m_taskRunning = false;

    QString message;
    bool success = (exitCode == 0 && exitStatus == QProcess::NormalExit);

    if (exitStatus == QProcess::NormalExit) {
        message = QString("分析任务已完成(退出码: %1)").arg(exitCode);
    } else {
        message = "分析任务异常终止";
    }

    m_logOutput->append(QString("\n=== %1 ===").arg(message));

    // 更新 UI 状态
    m_statusLabel->setText(success ? "已完成" : "失败");
    m_statusLabel->setStyleSheet(success ? "font-weight: bold; color: #107c10;" : "font-weight: bold; color: #d83b01;");
    m_startButton->setEnabled(true);
    m_stopButton->setEnabled(false);

    // 记录任务历史
    if (m_taskStartTime.isValid()) {
        int duration = m_taskStartTime.secsTo(QDateTime::currentDateTime());
        m_durationLabel->setText(QString("%1秒").arg(duration));
        addTaskHistory(m_taskStartTime, duration, success, QString("退出码: %1").arg(exitCode));
    }

    statusBar()->showMessage(message);

    // 显示完成通知
    if (success) {
        QMessageBox::information(this, "任务完成", "股票分析任务已完成");
    } else {
        QMessageBox::warning(this, "任务失败",
            QString("分析任务执行失败(退出码: %1)\n\n请查看日志了解详情").arg(exitCode));
    }
}

void TaskWindow::onTaskError(QProcess::ProcessError error) {
    m_taskRunning = false;

    QString errorMessage;
    switch (error) {
        case QProcess::FailedToStart:
            errorMessage = "进程启动失败";
            break;
        case QProcess::Crashed:
            errorMessage = "进程崩溃";
            break;
        case QProcess::Timedout:
            errorMessage = "进程超时";
            break;
        case QProcess::WriteError:
            errorMessage = "写入进程失败";
            break;
        case QProcess::ReadError:
            errorMessage = "读取进程输出失败";
            break;
        default:
            errorMessage = "未知错误";
            break;
    }

    m_logOutput->append(QString("\n!!! 任务错误: %1 !!!").arg(errorMessage));
    m_logOutput->append(QString("详细信息: %1").arg(m_taskProcess->errorString()));

    m_statusLabel->setText("错误");
    m_statusLabel->setStyleSheet("font-weight: bold; color: #d83b01;");
    m_startButton->setEnabled(true);
    m_stopButton->setEnabled(false);

    statusBar()->showMessage(QString("任务错误: %1").arg(errorMessage));

    QMessageBox::critical(this, "任务错误",
        QString("分析任务遇到错误：%1\n\n详细信息：%2")
        .arg(errorMessage)
        .arg(m_taskProcess->errorString()));
}

void TaskWindow::onTaskOutput() {
    if (!m_taskProcess) {
        return;
    }

    // 读取标准输出和标准错误
    QString output = m_taskProcess->readAllStandardOutput();
    QString errorOutput = m_taskProcess->readAllStandardError();

    // 显示到日志输出
    if (!output.isEmpty()) {
        m_logOutput->append(output.trimmed());
    }

    if (!errorOutput.isEmpty()) {
        m_logOutput->append(QString("[ERROR] %1").arg(errorOutput.trimmed()));
    }

    // 更新运行时间
    if (m_taskStartTime.isValid()) {
        int duration = m_taskStartTime.secsTo(QDateTime::currentDateTime());
        m_durationLabel->setText(QString("%1秒").arg(duration));
    }
}

void TaskWindow::addTaskHistory(const QDateTime& startTime, int duration, bool success, const QString& message) {
    QString historyItem = QString("[%1] 运行 %2秒 - %3 - %4")
                          .arg(startTime.toString("yyyy-MM-dd HH:mm:ss"))
                          .arg(duration)
                          .arg(success ? "成功" : "失败")
                          .arg(message);

    m_historyList->addItem(historyItem);
}