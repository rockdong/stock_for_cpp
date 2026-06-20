#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QTableView>
#include <QComboBox>
#include <QDateEdit>
#include <QLabel>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDateTime>
#include <QStandardItemModel>

/**
 * @brief 数据查询面板 - 查询股票数据、价格数据和分析结果
 *
 * 功能：
 * - 股票基本信息查询（按股票代码、名称搜索）
 * - 价格数据查询（按日期范围、股票代码查询）
 * - 分析结果查询（按策略、日期查询）
 * - 数据导出功能（导出查询结果为 CSV）
 */
class QueryPanel : public QWidget {
    Q_OBJECT

public:
    explicit QueryPanel(QWidget* parent = nullptr);
    ~QueryPanel();

    /**
     * @brief 设置查询结果数据
     */
    void setQueryResults(const QStringList& headers, const QList<QStringList>& data);

    /**
     * @brief 清空查询结果
     */
    void clearResults();

signals:
    /**
     * @brief 股票查询信号
     */
    void stockQueryRequested(const QString& keyword);

    /**
     * @brief 价格查询信号
     */
    void priceQueryRequested(const QString& tsCode, const QDateTime& startDate, const QDateTime& endDate);

    /**
     * @brief 分析结果查询信号
     */
    void analysisQueryRequested(const QString& strategy, const QDateTime& startDate, const QDateTime& endDate);

    /**
     * @brief 导出数据信号
     */
    void exportDataRequested(const QString& format);

private:
    void setupUI();
    void setupResultsTable();
    bool validateQueryInput();

    // ========== 股票查询区域 ==========

    QLineEdit* m_stockKeywordEdit = nullptr;  // 股票代码/名称搜索框
    QPushButton* m_queryStockBtn = nullptr;   // 查询股票按钮

    // ========== 价格查询区域 ==========

    QLineEdit* m_priceTsCodeEdit = nullptr;   // 股票代码输入
    QDateEdit* m_priceStartDate = nullptr;    // 开始日期
    QDateEdit* m_priceEndDate = nullptr;      // 结束日期
    QPushButton* m_queryPriceBtn = nullptr;   // 查询价格按钮

    // ========== 分析结果查询区域 ==========

    QComboBox* m_strategyCombo = nullptr;     // 策略选择下拉框
    QDateEdit* m_analysisStartDate = nullptr; // 开始日期
    QDateEdit* m_analysisEndDate = nullptr;   // 结束日期
    QPushButton* m_queryAnalysisBtn = nullptr; // 查询分析结果按钮

    // ========== 查询结果区域 ==========

    QTableView* m_resultsTable = nullptr;     // 查询结果表格
    QStandardItemModel* m_resultsModel = nullptr;  // 结果数据模型
    QPushButton* m_exportBtn = nullptr;       // 导出结果按钮

    // ========== 状态显示 ==========

    QLabel* m_statusLabel = nullptr;          // 状态文本
    QLabel* m_countLabel = nullptr;           // 结果数量显示
};