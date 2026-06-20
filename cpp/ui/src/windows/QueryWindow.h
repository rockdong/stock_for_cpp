/**
 * @file QueryWindow.h
 * @brief 数据查询独立窗口头文件
 * @author StockLens Team
 * @version 2.0.0
 */

#pragma once

#include <QMainWindow>
#include <QWidget>
#include <QLineEdit>
#include <QDateEdit>
#include <QPushButton>
#include <QTableWidget>

namespace data {
    class StockDAO;
    class PriceDAO;
}

/**
 * @class QueryWindow
 * @brief 数据查询独立窗口 - 单一职责: 数据查询
 * 
 * 负责功能:
 * - 股票查询 (按关键词搜索)
 * - 价格查询 (指定股票代码和日期范围)
 * - 分析结果查询 (指定策略和时间范围)
 * - 导出查询结果到 CSV 文件
 */
class QueryWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit QueryWindow(QWidget* parent = nullptr);
    ~QueryWindow() = default;

    /**
     * @brief 设置 DAO 对象 (由 ApplicationManager 共享)
     * @param stockDAO 股票数据访问对象
     * @param priceDAO 价格数据访问对象
     */
    void setDAOs(data::StockDAO* stockDAO, data::PriceDAO* priceDAO);

private slots:
    /**
     * @brief 股票查询按钮点击
     */
    void onStockQuery();

    /**
     * @brief 价格查询按钮点击
     */
    void onPriceQuery();

    /**
     * @brief 导出查询结果按钮点击
     */
    void onExportResults();

private:
    /**
     * @brief 设置 UI 布局
     */
    void setupUI();

    // ========== UI 组件 ==========

    // 股票查询输入框
    QLineEdit* m_stockKeywordInput = nullptr;

    // 价格查询输入框
    QLineEdit* m_priceTsCodeInput = nullptr;
    QDateEdit* m_startDateEdit = nullptr;
    QDateEdit* m_endDateEdit = nullptr;

    // 查询结果显示表格
    QTableWidget* m_resultsTable = nullptr;

    // ========== DAO 对象 ==========

    data::StockDAO* m_stockDAO = nullptr;
    data::PriceDAO* m_priceDAO = nullptr;
};