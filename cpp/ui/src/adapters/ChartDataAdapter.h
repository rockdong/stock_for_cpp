/**
 * @file ChartDataAdapter.h
 * @brief 图表数据适配器 - 将核心库数据转换为 Qt Charts 格式
 * @author StockLens Team
 * @version 1.0.0
 */

#ifndef CHART_DATA_ADAPTER_H
#define CHART_DATA_ADAPTER_H

#include <QObject>
#include <QtCharts>
#include <vector>

// 引用核心库数据结构（相对路径）
#include "../../core/Stock.h"
#include "../../core/AnalysisResult.h"

namespace ui {

/**
 * @brief 图表数据适配器
 * 
 * 负责将核心库的 StockData / AnalysisResult 转换为 Qt Charts 的数据系列
 */
class ChartDataAdapter : public QObject {
    Q_OBJECT

public:
    explicit ChartDataAdapter(QObject* parent = nullptr);
    ~ChartDataAdapter() override = default;

    /**
     * @brief 将 StockData 数组转换为蜡烛图系列
     * @param candles K线数据数组
     * @return QCandlestickSeries 蜡烛图系列
     */
    QCandlestickSeries* createCandlestickSeries(
        const std::vector<core::StockData>& candles);

    /**
     * @brief 创建收盘价折线系列（用于叠加在蜡烛图上）
     * @param candles K线数据数组
     * @return QLineSeries 收盘价系列
     */
    QLineSeries* createCloseSeries(
        const std::vector<core::StockData>& candles);

    /**
     * @brief 创建 MA 指标系列
     * @param candles K线数据数组
     * @param period MA 周期（如 5、10、20）
     * @return QLineSeries MA 系列线
     */
    QLineSeries* createMASeries(
        const std::vector<core::StockData>& candles, int period);

    /**
     * @brief 创建买卖信号标记系列
     * @param results 分析结果数组
     * @param candles K线数据数组（用于定位日期）
     * @return QScatterSeries 信号标记系列
     */
    QScatterSeries* createSignalSeries(
        const std::vector<core::AnalysisResult>& results,
        const std::vector<core::StockData>& candles);

    /**
     * @brief 设置图表轴的时间戳范围
     * @param axis X轴（时间轴）
     * @param candles K线数据数组
     */
    void setupDateTimeAxis(QDateTimeAxis* axis,
                           const std::vector<core::StockData>& candles);

private:
    /**
     * @brief 将 YYYYMMDD 格式转换为 QDateTime
     */
    QDateTime parseTradeDate(const std::string& dateStr);

    /**
     * @brief 计算 MA 值
     */
    double calculateMA(const std::vector<core::StockData>& candles,
                       int index, int period);
};

} // namespace ui

#endif // CHART_DATA_ADAPTER_H