/**
 * @file ChartWindow.h
 * @brief K线图表独立窗口头文件
 * @author StockLens Team
 * @version 2.0.0
 */

#pragma once

#include <QMainWindow>
#include <QWidget>
#include <QLineEdit>
#include <QDateEdit>
#include <QPushButton>

// 前置声明
namespace data {
    class StockDAO;
    class PriceDAO;
}

// 直接包含完整定义(不再使用前置声明)
#include "widgets/CandlestickWidget.h"

/**
 * @class ChartWindow
 * @brief K线图表独立窗口 - 单一职责: K线图表显示
 * 
 * 负责功能:
 * - 显示 K线图表
 * - 股票代码和时间范围选择
 * - 图表导出为 PNG 图片
 */
class ChartWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit ChartWindow(QWidget* parent = nullptr);
    ~ChartWindow() = default;

    /**
     * @brief 设置 DAO 对象 (由 ApplicationManager 共享)
     * @param stockDAO 股票数据访问对象
     * @param priceDAO 价格数据访问对象
     */
    void setDAOs(data::StockDAO* stockDAO, data::PriceDAO* priceDAO);

private slots:
    /**
     * @brief 加载图表按钮点击
     */
    void onLoadChart();

    /**
     * @brief 导出图表按钮点击
     */
    void onExportChart();

private:
    /**
     * @brief 设置 UI 布局
     */
    void setupUI();

    /**
     * @brief 加载 K线数据
     * @param tsCode 股票代码
     */
    void loadChartData(const QString& tsCode);

    // ========== UI 组件 ==========

    // 股票代码输入框
    QLineEdit* m_tsCodeInput = nullptr;

    // 时间范围选择
    QDateEdit* m_startDateEdit = nullptr;
    QDateEdit* m_endDateEdit = nullptr;

    // K线图表组件
    ui::CandlestickWidget* m_candlestickWidget = nullptr;

    // ========== DAO 对象 ==========

    data::StockDAO* m_stockDAO = nullptr;
    data::PriceDAO* m_priceDAO = nullptr;
};