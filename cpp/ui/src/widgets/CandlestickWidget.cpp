/**
 * @file CandlestickWidget.cpp
 * @brief K线图表控件实现
 * @author StockLens Team
 * @version 1.0.0
 */

#include "CandlestickWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSlider>
#include <QLabel>

namespace ui {

CandlestickWidget::CandlestickWidget(QWidget* parent)
    : QWidget(parent)
{
    setupChartView();
    setupAxes();
    connectInteractions();
}

void CandlestickWidget::setupChartView() {
    // 创建图表和视图
    m_chart = new QChart();
    m_chart->setTitle("K线图表");
    m_chart->setAnimationOptions(QChart::SeriesAnimations);
    
    m_chartView = new QChartView(m_chart);
    m_chartView->setRenderHint(QPainter::Antialiasing);
    m_chartView->setRubberBand(QChartView::RectangleRubberBand);
    
    // 创建数据适配器
    m_adapter = new ChartDataAdapter(this);
    
    // 布局
    auto* layout = new QVBoxLayout(this);
    layout->addWidget(m_chartView);
    
    // 控制面板（缩放和范围调整）
    auto* controlPanel = new QHBoxLayout();
    
    auto* rangeLabel = new QLabel("显示数量:");
    auto* rangeSlider = new QSlider(Qt::Horizontal);
    rangeSlider->setRange(50, 500);
    rangeSlider->setValue(m_visibleRange);
    rangeSlider->setTickInterval(50);
    
    auto* clearBtn = new QPushButton("清除");
    auto* zoomInBtn = new QPushButton("放大");
    auto* zoomOutBtn = new QPushButton("缩小");
    
    controlPanel->addWidget(rangeLabel);
    controlPanel->addWidget(rangeSlider);
    controlPanel->addStretch();
    controlPanel->addWidget(clearBtn);
    controlPanel->addWidget(zoomInBtn);
    controlPanel->addWidget(zoomOutBtn);
    
    layout->addLayout(controlPanel);
    
    // 连接信号槽
    connect(rangeSlider, &QSlider::valueChanged, this, &CandlestickWidget::setVisibleRange);
    connect(clearBtn, &QPushButton::clicked, this, &CandlestickWidget::clearChart);
    connect(zoomInBtn, &QPushButton::clicked, [this]() { m_chart->zoomIn(); });
    connect(zoomOutBtn, &QPushButton::clicked, [this]() { m_chart->zoomOut(); });
}

void CandlestickWidget::setupAxes() {
    // 时间轴（X轴）
    m_timeAxis = new QDateTimeAxis();
    m_timeAxis->setFormat("yyyy-MM-dd");
    m_timeAxis->setTitleText("日期");
    m_chart->addAxis(m_timeAxis, Qt::AlignBottom);
    
    // 价格轴（Y轴）
    m_priceAxis = new QValueAxis();
    m_priceAxis->setTitleText("价格");
    m_priceAxis->setLabelFormat("%.2f");
    m_chart->addAxis(m_priceAxis, Qt::AlignLeft);
}

void CandlestickWidget::connectInteractions() {
    // 点击图表时的交互
    connect(m_chartView, &QChartView::rubberBandChanged, this, [this]() {
        // 用户缩放后重新调整轴范围
        m_chart->zoomReset();
    });
}

void CandlestickWidget::setStockData(
    const QString& tsCode,
    const std::vector<core::StockData>& candles) {
    
    m_currentTsCode = tsCode;
    m_candles = candles;
    
    // 清除旧数据
    if (m_candlestickSeries) {
        m_chart->removeSeries(m_candlestickSeries);
        delete m_candlestickSeries;
    }
    
    // 创建蜡烛图系列
    m_candlestickSeries = m_adapter->createCandlestickSeries(candles);
    m_chart->addSeries(m_candlestickSeries);
    m_candlestickSeries->attachAxis(m_timeAxis);
    m_candlestickSeries->attachAxis(m_priceAxis);
    
    // 设置标题
    m_chart->setTitle(QString("%1 K线图表").arg(tsCode));
    
    // 设置轴范围
    auto visibleCount = std::min(m_visibleRange, static_cast<int>(candles.size()));
    auto startIdx = candles.size() - visibleCount;
    
    auto startIt = candles.begin() + startIdx;
    auto endIt = candles.end();
    
    std::vector<core::StockData> visibleData(startIt, endIt);
    m_adapter->setupDateTimeAxis(m_timeAxis, visibleData);
    
    // 计算 Y轴范围（最近可见数据）
    double minPrice = std::numeric_limits<double>::max();
    double maxPrice = std::numeric_limits<double>::min();
    
    for (const auto& c : visibleData) {
        minPrice = std::min(minPrice, c.low);
        maxPrice = std::max(maxPrice, c.high);
    }
    
    // 添加 5% 的边距
    double margin = (maxPrice - minPrice) * 0.05;
    m_priceAxis->setRange(minPrice - margin, maxPrice + margin);
    
    // 重新应用 MA 指标
    for (auto* maSeries : m_maSeries) {
        m_chart->removeSeries(maSeries);
        delete maSeries;
    }
    m_maSeries.clear();
}

void CandlestickWidget::setAnalysisResults(
    const std::vector<core::AnalysisResult>& results) {
    
    m_results = results;
    
    // 清除旧信号系列
    if (m_signalSeries) {
        m_chart->removeSeries(m_signalSeries);
        delete m_signalSeries;
    }
    
    // 创建新的信号系列
    m_signalSeries = m_adapter->createSignalSeries(results, m_candles);
    m_chart->addSeries(m_signalSeries);
    m_signalSeries->attachAxis(m_timeAxis);
    m_signalSeries->attachAxis(m_priceAxis);
}

void CandlestickWidget::addMAIndicator(int period) {
    if (m_candles.empty()) return;
    
    auto* maSeries = m_adapter->createMASeries(m_candles, period);
    m_chart->addSeries(maSeries);
    maSeries->attachAxis(m_timeAxis);
    maSeries->attachAxis(m_priceAxis);
    
    m_maSeries.push_back(maSeries);
}

void CandlestickWidget::clearMAIndicators() {
    for (auto* series : m_maSeries) {
        m_chart->removeSeries(series);
        delete series;
    }
    m_maSeries.clear();
}

void CandlestickWidget::clearChart() {
    // 清除所有系列
    if (m_candlestickSeries) {
        m_chart->removeSeries(m_candlestickSeries);
        delete m_candlestickSeries;
        m_candlestickSeries = nullptr;
    }
    
    if (m_signalSeries) {
        m_chart->removeSeries(m_signalSeries);
        delete m_signalSeries;
        m_signalSeries = nullptr;
    }
    
    clearMAIndicators();
    
    m_candles.clear();
    m_results.clear();
    m_currentTsCode.clear();
    
    m_chart->setTitle("K线图表");
}

void CandlestickWidget::setVisibleRange(int count) {
    m_visibleRange = count;
    
    // 如果已有数据，重新渲染
    if (!m_candles.empty()) {
        setStockData(m_currentTsCode, m_candles);
        
        // 重新应用信号
        if (!m_results.empty()) {
            setAnalysisResults(m_results);
        }
        
        // 重新应用 MA
        for (auto* series : m_maSeries) {
            int period = series->name().replace("MA", "").toInt();
            m_chart->removeSeries(series);
            delete series;
        }
        m_maSeries.clear();
        
        // TODO: 记住之前的 MA 周期设置
    }
}

/**
 * @brief 导出图表为 PNG 图片
 * @param fileName 文件路径
 * @return 是否成功
 */
bool CandlestickWidget::exportToPNG(const QString& fileName) {
    if (!m_chartView) {
        return false;
    }
    
    QPixmap pixmap(m_chartView->size());
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    m_chartView->render(&painter);
    painter.end();
    
    return pixmap.save(fileName);
}

} // namespace ui