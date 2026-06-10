/**
 * @file StockListModel.cpp
 * @brief 股票列表 Qt 数据模型实现
 * @author StockLens Team
 * @version 1.0.0
 */

#include "StockListModel.h"
#include <QColor>
#include <algorithm>

StockListModel::StockListModel(QObject* parent)
    : QAbstractItemModel(parent)
{
}

StockListModel::~StockListModel() {
}

QModelIndex StockListModel::index(int row, int column, 
                                    const QModelIndex& parent) const {
    if (parent.isValid() || row < 0 || row >= m_stocks.size() 
        || column < 0 || column >= ColCount) {
        return QModelIndex();
    }
    return createIndex(row, column);
}

QModelIndex StockListModel::parent(const QModelIndex& child) const {
    // 平铺列表，无父子关系
    return QModelIndex();
}

int StockListModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) {
        return 0;  // 无子节点
    }
    return static_cast<int>(m_stocks.size());
}

int StockListModel::columnCount(const QModelIndex& parent) const {
    return ColCount;
}

QVariant StockListModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= m_stocks.size()) {
        return QVariant();
    }

    const StockData& stock = m_stocks[index.row()];
    int col = index.column();

    // 显示角色
    if (role == Qt::DisplayRole) {
        switch (col) {
            case ColTsCode:
                return stock.tsCode;
            case ColName:
                return stock.name;
            case ColIndustry:
                return stock.industry;
            case ColLastPrice:
                return QString::number(stock.lastPrice, 'f', 2);
            case ColSignal:
                return stock.signal;
            default:
                return QVariant();
        }
    }

    // 文本对齐
    if (role == Qt::TextAlignmentRole) {
        if (col == ColLastPrice) {
            return QVariant(Qt::AlignRight | Qt::AlignVCenter);
        }
        return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
    }

    // 前景色（信号颜色）
    if (role == Qt::ForegroundRole) {
        if (col == ColSignal) {
            if (stock.signal == "BUY") {
                return QColor("#ef4444");  // 红色买入
            } else if (stock.signal == "SELL") {
                return QColor("#22c55e");  // 绿色卖出
            }
        }
    }

    return QVariant();
}

QVariant StockListModel::headerData(int section, Qt::Orientation orientation,
                                     int role) const {
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
        return QVariant();
    }

    switch (section) {
        case ColTsCode:
            return "代码";
        case ColName:
            return "名称";
        case ColIndustry:
            return "行业";
        case ColLastPrice:
            return "最新价";
        case ColSignal:
            return "信号";
        default:
            return QVariant();
    }
}

void StockListModel::setStocks(const std::vector<StockData>& stocks) {
    beginResetModel();
    m_stocks = stocks;
    endResetModel();
}

void StockListModel::clear() {
    beginResetModel();
    m_stocks.clear();
    endResetModel();
}

QString StockListModel::getTsCode(const QModelIndex& index) const {
    if (!index.isValid() || index.row() >= m_stocks.size()) {
        return QString();
    }
    return m_stocks[index.row()].tsCode;
}

void StockListModel::sort(int column, Qt::SortOrder order) {
    beginResetModel();
    
    std::sort(m_stocks.begin(), m_stocks.end(), 
        [column, order](const StockData& a, const StockData& b) {
            bool less = false;
            switch (column) {
                case ColTsCode:
                    less = a.tsCode < b.tsCode;
                    break;
                case ColName:
                    less = a.name < b.name;
                    break;
                case ColLastPrice:
                    less = a.lastPrice < b.lastPrice;
                    break;
                case ColSignal:
                    less = a.signal < b.signal;
                    break;
                default:
                    less = a.tsCode < b.tsCode;
            }
            return order == Qt::AscendingOrder ? less : !less;
        });
    
    endResetModel();
}