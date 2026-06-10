/**
 * @file CandlestickWidget.h
 * @brief K线图表控件
 * @author StockLens Team
 * @version 1.0.0
 */

#ifndef CANDLESTICK_WIDGET_H
#define CANDLESTICK_WIDGET_H

#include <QWidget>
#include <QtCharts>
#include <vector>

#include "adapters/ChartDataAdapter.h"
#include "../../core/Stock.h"
#include "../../core/AnalysisResult.h"

namespace ui {

/**
 * @brief K线图表控件
 * 
 * 显示股票的 K线图表，支持：
 * - 蜡烛图（OHLC）
 * - MA 指标线叠加
 * - 买卖信号标记
 * - 交互式缩放和平移
 */
class CandlestickWidget : public QWidget {
    Q_OBJECT

public:
    explicit CandlestickWidget(QWidget* parent = nullptr);
    ~CandlestickWidget() override = default;

    /**
     * @brief 设置股票数据并渲染图表
     * @param tsCode 股票代码
     * @param candles K线数据数组
     */
    void setStockData(const QString& tsCode,
                      const std::vector<core::StockData>& candles);

    /**
     * @brief 设置分析结果（买卖信号）
     * @param results 分析结果数组
     */
    void setAnalysisResults(const std::vector<core::AnalysisResult>& results);

    /**
     * @brief 添加 MA 指标线
     * @param period MA 周期（如 5、10、20）
     */
    void addMAIndicator(int period);

    /**
     * @brief 清除所有 MA 指标线
     */
    void clearMAIndicators();

    /**
     * @brief 清除图表
     */
    void clearChart();

    /**
     * @brief 设置显示范围（最近 N 根 K线）
     * @param count 显示数量（默认 100）
     */
    void setVisibleRange(int count);

signals:
    /**
     * @brief 用户点击图表时发出
     * @param timestamp 点击的时间戳
     * @param price 点击的价格位置
     */
    void pointClicked(qreal timestamp, qreal price);

protected:
    void setupChartView();
    void setupAxes();
    void connectInteractions();

private:
    QChartView* m_chartView = nullptr;
    QChart* m_chart = nullptr;
    QDateTimeAxis* m_timeAxis = nullptr;
    QValueAxis* m_priceAxis = nullptr;
    
    ChartDataAdapter* m_adapter = nullptr;
    
    QString m_currentTsCode;
    std::vector<core::StockData> m_candles;
    std::vector<core::AnalysisResult> m_results;
    
    // 图表系列
    QCandlestickSeries* m_candlestickSeries = nullptr;
    QScatterSeries* m_signalSeries = nullptr;
    std::vector<QLineSeries*> m_maSeries;
    
    int m_visibleRange = 100;  // 默认显示 100 根 K线
};

} // namespace ui

#endif // CANDLESTICK_WIDGET_H