/**
 * @file ChartWindow.cpp
 * @brief K线图表独立窗口实现
 */

#include "ChartWindow.h"
#include "widgets/CandlestickWidget.h"   // 使用 CMake 配置的包含路径
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QFileDialog>
#include <QStatusBar>      // QStatusBar 完整定义
#include <QMessageBox>
#include <QFileInfo>

ChartWindow::ChartWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("K线图表 - Stock for C++");
    resize(900, 700);
    setupUI();
}

void ChartWindow::setDAOs(data::StockDAO* stockDAO, data::PriceDAO* priceDAO) {
    m_stockDAO = stockDAO;
    m_priceDAO = priceDAO;
}

void ChartWindow::setupUI() {
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

    // ========== 股票选择面板 ==========
    QGroupBox* selectGroup = new QGroupBox("股票选择", this);
    QHBoxLayout* selectLayout = new QHBoxLayout(selectGroup);
    selectLayout->addWidget(new QLabel("股票代码:", this));

    m_tsCodeInput = new QLineEdit(this);
    m_tsCodeInput->setPlaceholderText("如: 000001.SZ");
    selectLayout->addWidget(m_tsCodeInput);

    selectLayout->addWidget(new QLabel("开始日期:", this));
    m_startDateEdit = new QDateEdit(this);
    m_startDateEdit->setCalendarPopup(true);
    selectLayout->addWidget(m_startDateEdit);

    selectLayout->addWidget(new QLabel("结束日期:", this));
    m_endDateEdit = new QDateEdit(this);
    m_endDateEdit->setCalendarPopup(true);
    selectLayout->addWidget(m_endDateEdit);

    QPushButton* loadBtn = new QPushButton("加载图表", this);
    connect(loadBtn, &QPushButton::clicked, this, &ChartWindow::onLoadChart);
    selectLayout->addWidget(loadBtn);

    QPushButton* exportBtn = new QPushButton("导出PNG", this);
    connect(exportBtn, &QPushButton::clicked, this, &ChartWindow::onExportChart);
    selectLayout->addWidget(exportBtn);

    selectLayout->addStretch();
    mainLayout->addWidget(selectGroup);

    // ========== K线图表显示 ==========
    QGroupBox* chartGroup = new QGroupBox("K线图表", this);
    QVBoxLayout* chartLayout = new QVBoxLayout(chartGroup);

    m_candlestickWidget = new ui::CandlestickWidget(this);
    chartLayout->addWidget(m_candlestickWidget);

    mainLayout->addWidget(chartGroup);
    setCentralWidget(centralWidget);
}

void ChartWindow::onLoadChart() {
    QString tsCode = m_tsCodeInput->text();
    if (tsCode.isEmpty()) {
        QMessageBox::warning(this, "加载失败", "请输入股票代码");
        return;
    }

    loadChartData(tsCode);
}

void ChartWindow::loadChartData(const QString& tsCode) {
    statusBar()->showMessage(QString("正在加载 %1 K线数据...").arg(tsCode));

    // TODO: 调用 m_priceDAO 加载真实数据
    // 当前实现：使用模拟数据
    std::vector<core::StockData> mockData;

    double basePrice = 10.0;
    for (int i = 0; i < 60; ++i) {
        core::StockData candle;
        candle.ts_code = tsCode.toStdString();
        candle.trade_date = QString("202601%1").arg(i + 1, 2, 10, QChar('0')).toStdString();
        candle.open = basePrice + (rand() % 100 - 50) * 0.02;
        candle.close = candle.open + (rand() % 100 - 50) * 0.02;
        candle.high = std::max(candle.open, candle.close) + rand() % 20 * 0.02;
        candle.low = std::min(candle.open, candle.close) - rand() % 20 * 0.02;
        candle.pre_close = basePrice;
        candle.change = candle.close - candle.pre_close;
        candle.pct_chg = (candle.change / candle.pre_close) * 100.0;
        candle.volume = rand() % 10000 + 5000;
        candle.amount = candle.volume * candle.close;
        mockData.push_back(candle);
        basePrice = candle.close;
    }

    m_candlestickWidget->setStockData(tsCode, mockData);
    statusBar()->showMessage(QString("已加载 %1 K线数据").arg(tsCode));
}

void ChartWindow::onExportChart() {
    QString fileName = QFileDialog::getSaveFileName(this, "导出图表", QDir::homePath() + "/chart.png", "PNG图片(*.png)");
    if (fileName.isEmpty()) return;

    if (m_candlestickWidget->exportToPNG(fileName)) {
        QMessageBox::information(this, "导出成功", QString("图表已导出到: %1").arg(fileName));
    } else {
        QMessageBox::warning(this, "导出失败", "无法保存图表");
    }
}