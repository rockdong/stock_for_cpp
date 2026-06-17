#pragma once

#include <QAbstractListModel>
#include <QString>
#include <QVariant>
#include <vector>

// 包含核心库数据结构（完整定义）
#include "../../core/Stock.h"  // 股票数据结构

namespace stock_gui {

/**
 * @brief 股票列表数据模型 - Qt数据适配器
 *
 * 功能:
 * - 将核心库的 std::vector<core::Stock> 转换为 Qt 数据模型
 * - 提供 QAbstractListModel 接口，用于 QListView/QTableView 显示
 * - 支持股票代码、名称、行业等字段的显示
 * - 提供实时更新和查询功能
 */
class StockListModel : public QAbstractListModel {
    Q_OBJECT

public:
    // 自定义角色枚举（用于数据访问）
    enum Roles {
        TsCodeRole = Qt::UserRole + 1,   // 股票代码
        NameRole = Qt::UserRole + 2,      // 股票名称
        IndustryRole = Qt::UserRole + 3,  // 行业
        MarketRole = Qt::UserRole + 4,    // 市场
        ListDateRole = Qt::UserRole + 5   // 上市日期
    };

    explicit StockListModel(QObject* parent = nullptr);
    ~StockListModel() override = default;

    // QAbstractListModel 必须实现的虚函数
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // 数据设置接口（核心库 -> Qt 模型）
    void setStocks(const std::vector<core::Stock>& stocks);
    void clear();

    // 查询接口
    core::Stock getStockAt(int row) const;
    int findStockByCode(const QString& tsCode) const;

private:
    std::vector<core::Stock> stocks_;  // 存储股票数据
};

}  // namespace stock_gui