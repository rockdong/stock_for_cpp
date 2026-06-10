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

// 前向声明（核心库类型）
// 实际实现时将链接 core_lib

/**
 * @struct StockData
 * @brief 股票数据结构（Qt 视图）
 */
struct StockData {
    QString tsCode;      // 股票代码
    QString name;        // 股票名称
    QString industry;    // 所属行业
    QString market;      // 市场（沪/深）
    double lastPrice = 0.0;  // 最新价
    QString signal;      // 信号（BUY/SELL/HOLD）
};

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
    void clear();
    QString getTsCode(const QModelIndex& index) const;

    // 排序
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

private:
    std::vector<StockData> m_stocks;
};