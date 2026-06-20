#include "TaskManagementPanel.h"
#include <QStyle>
#include <QApplication>
#include <QScrollBar>

TaskManagementPanel::TaskManagementPanel(QWidget* parent)
    : QWidget(parent)
{
    setupUI();

    // 初始化持续时间计时器
    m_durationTimer = new QTimer(this);
    m_durationTimer->setInterval(1000); // 每秒更新
    connect(m_durationTimer, &QTimer::timeout, this, [this]() {
        if (m_taskRunning && m_taskStartTime.isValid()) {
            int elapsed = m_taskStartTime.secsTo(QDateTime::currentDateTime());
            m_progressBar->setFormat(QString("运行时间: %1 秒").arg(elapsed));
        }
    });
}

TaskManagementPanel::~TaskManagementPanel() {
    if (m_durationTimer) {
        m_durationTimer->stop();
    }
}

void TaskManagementPanel::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    // ========== 状态区域 ==========

    QHBoxLayout* statusLayout = new QHBoxLayout();

    m_statusIcon = new QLabel(this);
    m_statusIcon->setFixedSize(32, 32);
    m_statusIcon->setAlignment(Qt::AlignCenter);

    m_statusLabel = new QLabel("任务状态: 已停止", this);
    m_statusLabel->setStyleSheet("font-weight: bold; font-size: 14px;");

    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 0); // 无限进度条（表示运行中）
    m_progressBar->setTextVisible(true);
    m_progressBar->setFormat("等待启动...");
    m_progressBar->setVisible(false); // 初始隐藏

    statusLayout->addWidget(m_statusIcon);
    statusLayout->addWidget(m_statusLabel);
    statusLayout->addStretch();
    statusLayout->addWidget(m_progressBar);

    mainLayout->addLayout(statusLayout);

    // ========== 控制按钮 ==========

    QHBoxLayout* controlLayout = new QHBoxLayout();

    m_startButton = new QPushButton("启动分析任务", this);
    m_startButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    m_startButton->setToolTip("启动后台股票分析任务");
    connect(m_startButton, &QPushButton::clicked, this, &TaskManagementPanel::startTaskRequested);

    m_stopButton = new QPushButton("停止任务", this);
    m_stopButton->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    m_stopButton->setToolTip("停止正在运行的分析任务");
    m_stopButton->setEnabled(false); // 初始禁用
    connect(m_stopButton, &QPushButton::clicked, this, &TaskManagementPanel::stopTaskRequested);

    controlLayout->addWidget(m_startButton);
    controlLayout->addWidget(m_stopButton);
    controlLayout->addStretch();

    mainLayout->addLayout(controlLayout);

    // ========== 输出日志区域 ==========

    QLabel* outputLabel = new QLabel("实时输出日志:", this);
    outputLabel->setStyleSheet("font-weight: bold; margin-top: 10px;");
    mainLayout->addWidget(outputLabel);

    m_outputLog = new QTextEdit(this);
    m_outputLog->setReadOnly(true);
    m_outputLog->setStyleSheet(
        "background-color: #1e1e1e; color: #d4d4d4; font-family: 'Courier New', monospace; font-size: 12px;"
    );
    m_outputLog->setMinimumHeight(200);
    m_outputLog->append("=== 任务管理面板已初始化 ===");
    m_outputLog->append("等待启动分析任务...\n");

    mainLayout->addWidget(m_outputLog);

    // 清空日志按钮
    QHBoxLayout* logControlLayout = new QHBoxLayout();
    m_clearLogButton = new QPushButton("清空日志", this);
    m_clearLogButton->setIcon(style()->standardIcon(QStyle::SP_DialogResetButton));
    connect(m_clearLogButton, &QPushButton::clicked, this, &TaskManagementPanel::clearOutputLog);
    logControlLayout->addWidget(m_clearLogButton);
    logControlLayout->addStretch();
    mainLayout->addLayout(logControlLayout);

    // ========== 历史记录区域 ==========

    QLabel* historyLabel = new QLabel("任务执行历史:", this);
    historyLabel->setStyleSheet("font-weight: bold; margin-top: 10px;");
    mainLayout->addWidget(historyLabel);

    m_historyLog = new QTextEdit(this);
    m_historyLog->setReadOnly(true);
    m_historyLog->setStyleSheet(
        "background-color: #f5f5f5; color: #333333; font-family: 'Arial', sans-serif; font-size: 11px;"
    );
    m_historyLog->setMaximumHeight(150);
    m_historyLog->append("=== 任务执行历史 ===");
    m_historyLog->append("暂无历史记录\n");

    mainLayout->addWidget(m_historyLog);

    // 初始化状态显示
    updateStatusDisplay();
}

void TaskManagementPanel::setTaskRunning(bool running) {
    m_taskRunning = running;

    if (running) {
        m_taskStartTime = QDateTime::currentDateTime();
        m_durationTimer->start();
        m_progressBar->setVisible(true);
        m_progressBar->setFormat("运行时间: 0 秒");
    } else {
        m_durationTimer->stop();
        m_progressBar->setVisible(false);
    }

    // 更新按钮状态
    m_startButton->setEnabled(!running);
    m_stopButton->setEnabled(running);

    updateStatusDisplay();
}

void TaskManagementPanel::appendTaskOutput(const QString& output, bool isError) {
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

    if (isError) {
        m_outputLog->append(QString("<span style='color: #f44336;'>[%1] [ERROR] %2</span>")
                           .arg(timestamp, output));
    } else {
        m_outputLog->append(QString("<span style='color: #4caf50;'>[%1] [INFO] %2</span>")
                           .arg(timestamp, output));
    }

    // 自动滚动到底部
    QScrollBar* scrollbar = m_outputLog->verticalScrollBar();
    scrollbar->setValue(scrollbar->maximum());
}

void TaskManagementPanel::addTaskHistory(const QDateTime& startTime, int duration,
                                        bool success, const QString& message) {
    QString timestamp = startTime.toString("yyyy-MM-dd HH:mm:ss");
    QString status = success ? "✓ 成功" : "✗ 失败";
    QString color = success ? "#4caf50" : "#f44336";

    QString entry = QString("<span style='color: %1;'>[%2] %3 | 持续时间: %4秒 | %5</span>")
                   .arg(color, timestamp, status, QString::number(duration), message);

    m_historyLog->append(entry);

    // 自动滚动到底部
    QScrollBar* scrollbar = m_historyLog->verticalScrollBar();
    scrollbar->setValue(scrollbar->maximum());
}

void TaskManagementPanel::clearOutputLog() {
    m_outputLog->clear();
    m_outputLog->append("=== 日志已清空 ===");
    m_outputLog->append(QString("清空时间: %1\n")
                       .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")));
}

void TaskManagementPanel::updateStatusDisplay() {
    if (m_taskRunning) {
        m_statusIcon->setStyleSheet(
            "background-color: #4caf50; border-radius: 16px; color: white; font-weight: bold;"
        );
        m_statusIcon->setText("●");
        m_statusLabel->setText("任务状态: 运行中");
        m_statusLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: #4caf50;");
    } else {
        m_statusIcon->setStyleSheet(
            "background-color: #9e9e9e; border-radius: 16px; color: white; font-weight: bold;"
        );
        m_statusIcon->setText("○");
        m_statusLabel->setText("任务状态: 已停止");
        m_statusLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: #9e9e9e;");
    }
}