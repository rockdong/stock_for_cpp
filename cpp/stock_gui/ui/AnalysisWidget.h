#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QGroupBox>
#include <QChartView>
#include <QLineSeries>

// Qt Charts
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>

namespace stock_gui {

/**
 * @brief 分析界面 - 显示股价图表和分析结果
 *
 * 功能:
 * - Qt Charts 图表展示（股价走势）
 * - 进度条显示
 * - 统计信息
 * - 开始/停止按钮
 */
class AnalysisWidget : public QWidget {
    Q_OBJECT

public:
    explicit AnalysisWidget(QWidget* parent = nullptr);
    ~AnalysisWidget() override;

private:
    void SetupUI();

    // UI 组件
    QVBoxLayout* mainLayout_;
    QGroupBox* chartGroup_;
    QChartView* chartView_;
    QChart* chart_;
    QLineSeries* priceSeries_;

    QGroupBox* controlGroup_;
    QPushButton* startButton_;
    QPushButton* stopButton_;
    QProgressBar* progressBar_;

    QGroupBox* statsGroup_;
    QLabel* completedLabel_;
    QLabel* failedLabel_;
    QLabel* statusLabel_;
};

}  // namespace stock_gui