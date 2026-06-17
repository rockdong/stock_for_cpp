#include "StockListModel.h"
#include "../../core/Stock.h"  // 核心库股票数据结构

#include <QDebug>

namespace stock_gui {

StockListModel::StockListModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

int StockListModel::rowCount(const QModelIndex& parent) const {
    // 如果是父节点（树形结构），返回0
    if (parent.isValid()) {
        return 0;
    }
    // 否则返回股票数量
    return static_cast<int>(stocks_.size());
}

QVariant StockListModel::data(const QModelIndex& index, int role) const {
    // 检查索引有效性
    if (!index.isValid()) {
        return QVariant();
    }

    // 检查行号范围
    int row = index.row();
    if (row < 0 || row >= static_cast<int>(stocks_.size())) {
        return QVariant();
    }

    // 获取股票数据
    const core::Stock& stock = stocks_[row];

    // 根据角色返回对应数据
    switch (role) {
        case Qt::DisplayRole:
            // 默认显示：股票名称
            return QString::fromStdString(stock.name);

        case Qt::ToolTipRole:
            // 工具提示：股票代码 + 名称
            return QString("%1 - %2")
                .arg(QString::fromStdString(stock.ts_code))
                .arg(QString::fromStdString(stock.name));

        case TsCodeRole:
            // 股票代码
            return QString::fromStdString(stock.ts_code);

        case NameRole:
            // 股票名称
            return QString::fromStdString(stock.name);

        case IndustryRole:
            // 行业
            return QString::fromStdString(stock.industry);

        case MarketRole:
            // 市场
            return QString::fromStdString(stock.market);

        case ListDateRole:
            // 上市日期
            return QString::fromStdString(stock.list_date);

        default:
            return QVariant();
    }
}

QHash<int, QByteArray> StockListModel::roleNames() const {
    // 定义角色名称（用于 QML 或高级视图）
    QHash<int, QByteArray> roles;
    roles[TsCodeRole] = "tsCode";
    roles[NameRole] = "name";
    roles[IndustryRole] = "industry";
    roles[MarketRole] = "market";
    roles[ListDateRole] = "listDate";
    return roles;
}

void StockListModel::setStocks(const std::vector<core::Stock>& stocks) {
    // 通知视图开始重置数据
    beginResetModel();

    // 更新数据
    stocks_ = stocks;

    // 通知视图数据重置完成
    endResetModel();

    qDebug() << "StockListModel: 更新股票数据，数量:" << stocks_.size();
}

void StockListModel::clear() {
    beginResetModel();
    stocks_.clear();
    endResetModel();
    qDebug() << "StockListModel: 清空股票数据";
}

core::Stock StockListModel::getStockAt(int row) const {
    if (row < 0 || row >= static_cast<int>(stocks_.size())) {
        qWarning() << "StockListModel: 行号越界:" << row;
        // 返回空的股票数据
        return core::Stock{};
    }
    return stocks_[row];
}

int StockListModel::findStockByCode(const QString& tsCode) const {
    std::string code = tsCode.toStdString();
    for (int i = 0; i < static_cast<int>(stocks_.size()); ++i) {
        if (stocks_[i].ts_code == code) {
            return i;
        }
    }
    return -1;  // 未找到
}

}  // namespace stock_gui