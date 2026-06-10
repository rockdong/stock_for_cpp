/**
 * @file ChartDataAdapter.cpp
 * @brief 图表数据适配器实现
 * @author StockLens Team
 * @version 1.0.0
 */

#include "ChartDataAdapter.h"
#include <QCandlestickSeries>
#include <QCandlestickSet>
#include <QLineSeries>
#include <QScatterSeries>
#include <QDateTimeAxis>
#include <QDate>
#include <algorithm>

namespace ui {

ChartDataAdapter::ChartDataAdapter(QObject* parent)
    : QObject(parent)
{
}

QCandlestickSeries* ChartDataAdapter::createCandlestickSeries(
    const std::vector<core::StockData>& candles) {
    
    auto* series = new QCandlestickSeries();
    series->setName("K线");
    series->setIncreasingColor(QColor("#22c55e"));   // 绿色上涨
    series->setDecreasingColor(QColor("#ef4444"));   // 红色下跌
    
    for (const auto& candle : candles) {
        auto timestamp = parseTradeDate(candle.trade_date).toMSecsSinceEpoch();
        
        auto* set = new QCandlestickSet(
            candle.open,
            candle.high,
            candle.low,
            candle.close,
            timestamp
        );
        
        series->append(set);
    }
    
    return series;
}

QLineSeries* ChartDataAdapter::createCloseSeries(
    const std::vector<core::StockData>& candles) {
    
    auto* series = new QLineSeries();
    series->setName("收盘价");
    series->setColor(QColor("#3b82f6"));  // 蓝色
    series->setUseOpenGL(true);           // 启用 OpenGL 加速
    
    for (const auto& candle : candles) {
        auto timestamp = parseTradeDate(candle.trade_date).toMSecsSinceEpoch();
        series->append(timestamp, candle.close);
    }
    
    return series;
}

QLineSeries* ChartDataAdapter::createMASeries(
    const std::vector<core::StockData>& candles, int period) {
    
    auto* series = new QLineSeries();
    series->setName(QString("MA%1").arg(period));
    
    // 根据周期设置不同颜色
    switch (period) {
        case 5:  series->setColor(QColor("#f59e0b")); break;  // 橙色
        case 10: series->setColor(QColor("#8b5cf6")); break;  // 紫色
        case 20: series->setColor(QColor("#06b6d4")); break;  // 青色
        default: series->setColor(QColor("#64748b")); break;  // 灰色
    }
    
    series->setUseOpenGL(true);
    
    // 计算并添加 MA 数据点
    for (size_t i = period - 1; i < candles.size(); ++i) {
        double ma = calculateMA(candles, i, period);
        auto timestamp = parseTradeDate(candles[i].trade_date).toMSecsSinceEpoch();
        series->append(timestamp, ma);
    }
    
    return series;
}

QScatterSeries* ChartDataAdapter::createSignalSeries(
    const std::vector<core::AnalysisResult>& results,
    const std::vector<core::StockData>& candles) {
    
    auto* series = new QScatterSeries();
    series->setName("交易信号");
    series->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    series->setMarkerSize(12);
    
    for (const auto& result : results) {
        // 找到对应的 K线日期
        auto it = std::find_if(candles.begin(), candles.end(),
            [&](const core::StockData& c) {
                return c.trade_date == result.trade_date;
            });
        
        if (it != candles.end()) {
            auto timestamp = parseTradeDate(it->trade_date).toMSecsSinceEpoch();
            
            // 根据信号类型设置颜色和位置
            if (result.opt == "buy") {
                series->setColor(QColor("#22c55e"));  // 绿色买入
                series->append(timestamp, it->low * 0.98);  // 放在 K线下方
            } else if (result.opt == "sell") {
                series->setColor(QColor("#ef4444"));  // 红色卖出
                series->append(timestamp, it->high * 1.02); // 放在 K线上方
            }
        }
    }
    
    return series;
}

void ChartDataAdapter::setupDateTimeAxis(
    QDateTimeAxis* axis,
    const std::vector<core::StockData>& candles) {
    
    if (candles.empty()) return;
    
    // 设置轴的格式和范围
    axis->setFormat("yyyy-MM-dd");
    axis->setTitleText("日期");
    
    auto start = parseTradeDate(candles.front().trade_date);
    auto end = parseTradeDate(candles.back().trade_date);
    
    axis->setRange(start, end);
    axis->setTickCount(std::min(10, static_cast<int>(candles.size())));
}

QDateTime ChartDataAdapter::parseTradeDate(const std::string& dateStr) {
    // 格式：YYYYMMDD
    if (dateStr.length() != 8) {
        return QDateTime();
    }
    
    int year = std::stoi(dateStr.substr(0, 4));
    int month = std::stoi(dateStr.substr(4, 2));
    int day = std::stoi(dateStr.substr(6, 2));
    
    return QDateTime(QDate(year, month, day), QTime(0, 0, 0));
}

double ChartDataAdapter::calculateMA(
    const std::vector<core::StockData>& candles,
    int index, int period) {
    
    double sum = 0.0;
    for (int i = index - period + 1; i <= index; ++i) {
        sum += candles[i].close;
    }
    
    return sum / period;
}

} // namespace ui