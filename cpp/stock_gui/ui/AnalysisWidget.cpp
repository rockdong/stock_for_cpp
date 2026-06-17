#include "AnalysisWidget.h"

#include <QValueAxis>
#include <QMessageBox>

namespace stock_gui {

AnalysisWidget::AnalysisWidget(QWidget* parent)
    : QWidget(parent)
    , mainLayout_(nullptr)
    , chartGroup_(nullptr)
    , chartView_(nullptr)
    , chart_(nullptr)
    , priceSeries_(nullptr)
    , controlGroup_(nullptr)
    , startButton_(nullptr)
    , stopButton_(nullptr)
    , progressBar_(nullptr)
    , statsGroup_(nullptr)
    , completedLabel_(nullptr)
    , failedLabel_(nullptr)
    , statusLabel_(nullptr)
{
    SetupUI();
}

AnalysisWidget::~AnalysisWidget() = default;

void AnalysisWidget::SetupUI() {
    mainLayout_ = new QVBoxLayout(this);

    // 1. 图表区域
    chartGroup_ = new QGroupBox("股价走势图", this);
    QVBoxLayout* chartLayout = new QVBoxLayout(chartGroup_);

    // 创建图表
    chart_ = new QChart();
    chart_->setTitle("股票价格走势");
    chart_->setAnimationOptions(QChart::SeriesAnimations);

    // 创建价格序列
    priceSeries_ = new QLineSeries();
    priceSeries_->setName("股价");
    chart_->addSeries(priceSeries_);

    // 设置坐标轴
    QValueAxis* axisX = new QValueAxis();
    axisX->setTitleText("时间");
    axisX->setLabelFormat("%i");
    chart_->addAxis(axisX, Qt::AlignBottom);
    priceSeries_->attachAxis(axisX);

    QValueAxis* axisY = new QValueAxis();
    axisY->setTitleText("价格");
    axisY->setLabelFormat("%.2f");
    chart_->addAxis(axisY, Qt::AlignLeft);
    priceSeries_->attachAxis(axisY);

    // 创建图表视图
    chartView_ = new QChartView(chart_);
    chartView_->setRenderHint(QPainter::Antialiasing);
    chartLayout->addWidget(chartView_);

    mainLayout_->addWidget(chartGroup_);

    // 2. 控制区域
    controlGroup_ = new QGroupBox("分析控制", this);
    QHBoxLayout* controlLayout = new QHBoxLayout(controlGroup_);

    startButton_ = new QPushButton("开始分析", this);
    stopButton_ = new QPushButton("停止分析", this);
    progressBar_ = new QProgressBar(this);
    progressBar_->setRange(0, 100);
    progressBar_->setValue(0);

    controlLayout->addWidget(startButton_);
    controlLayout->addWidget(stopButton_);
    controlLayout->addWidget(progressBar_);

    mainLayout_->addWidget(controlGroup_);

    // 3. 统计区域
    statsGroup_ = new QGroupBox("统计信息", this);
    QVBoxLayout* statsLayout = new QVBoxLayout(statsGroup_);

    completedLabel_ = new QLabel("已完成: 0", this);
    failedLabel_ = new QLabel("失败数: 0", this);
    statusLabel_ = new QLabel("状态: 就绪", this);

    statsLayout->addWidget(completedLabel_);
    statsLayout->addWidget(failedLabel_);
    statsLayout->addWidget(statusLabel_);

    mainLayout_->addWidget(statsGroup_);

    // 连接按钮信号
    connect(startButton_, &QPushButton::clicked, [this]() {
        statusLabel_->setText("状态: 运行中...");
        progressBar_->setValue(0);
        // TODO: 开始分析
    });

    connect(stopButton_, &QPushButton::clicked, [this]() {
        statusLabel_->setText("状态: 已停止");
        // TODO: 停止分析
    });

    // 添加示例数据（测试）
    priceSeries_->append(0, 10.5);
    priceSeries_->append(1, 11.2);
    priceSeries_->append(2, 10.8);
    priceSeries_->append(3, 12.0);
    priceSeries_->append(4, 11.5);

    axisX->setRange(0, 4);
    axisY->setRange(10, 12);
}

}  // namespace stock_gui