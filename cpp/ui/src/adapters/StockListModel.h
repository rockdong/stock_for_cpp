/**
 * @file StockListModel.h
 * @brief 股票列表 Qt 数据模型适配器
 * @author StockLens Team
 * @version 1.0.0
 */

#pragma once

#include <QAbstractItemModel>
#include <QString>
#include <vector>

// 核心库类型（Phase 3 集成）
#include "../../core/Stock.h"

namespace ui {

/**
 * @struct StockData
 * @brief 股票数据结构（Qt 视图，用于显示）
 * 注意：这是一个 Qt 视图专用的简化结构，避免与 core::StockData 冲突
 */
struct StockData {
    QString tsCode;      // 股票代码
    QString name;        // 股票名称
    QString industry;    // 所属行业
    QString market;      // 市场（沪/深）
    double lastPrice = 0.0;  // 最新价
    QString signal;      // 信号（BUY/SELL/HOLD）
};

} // namespace ui

// 全局前向声明（用于 Qt 模型类）
using StockData = ui::StockData;

/**
 * @class StockListModel
 * @brief 股票列表数据模型（继承 QAbstractItemModel）
 */
class StockListModel : public QAbstractItemModel {
    Q_OBJECT

public:
    enum Column {
        ColTsCode = 0,
        ColName = 1,
        ColIndustry = 2,
        ColLastPrice = 3,
        ColSignal = 4,
        ColCount = 5
    };

    explicit StockListModel(QObject* parent = nullptr);
    ~StockListModel();

    // QAbstractItemModel 必需接口
    QModelIndex index(int row, int column, 
                      const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& child) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, 
                  int role = Qt::DisplayRole) const override;
    
    // 表头
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    // 数据操作
    void setStocks(const std::vector<StockData>& stocks);
    
    // ========== Phase 3：支持核心库 Stock 结构 ==========
    void setStocks(const std::vector<core::Stock>& stocks);  // 直接接收核心库数据
    
    void clear();
    QString getTsCode(const QModelIndex& index) const;

    // 排序
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

private:
    std::vector<StockData> m_stocks;  // Qt 视图数据
};