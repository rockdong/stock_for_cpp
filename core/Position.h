#ifndef CORE_POSITION_H
#define CORE_POSITION_H

#include <string>
#include <vector>
#include <memory>
#include "Trade.h"

namespace core {

/**
 * @brief 持仓信息
 * 
 * 记录单只股票的持仓情况
 * 遵循单一职责原则（SRP）
 */
class Position {
public:
    /**
     * @brief 构造函数
     * @param tsCode 股票代码
     */
    explicit Position(const std::string& tsCode);
    
    // Getters
    std::string getTsCode() const { return tsCode_; }
    int getQuantity() const { return quantity_; }
    double getAvgCost() const { return avgCost_; }
    double getTotalCost() const { return totalCost_; }
    double getCurrentPrice() const { return currentPrice_; }
    double getMarketValue() const { return marketValue_; }
    double getProfit() const { return profit_; }
    double getProfitRate() const { return profitRate_; }
    const std::vector<TradePtr>& getTrades() const { return trades_; }
    
    /**
     * @brief 更新当前价格
     * @param price 当前价格
     */
    void updatePrice(double price);
    
    /**
     * @brief 添加交易
     * @param trade 交易记录
     */
    void addTrade(TradePtr trade);
    
    /**
     * @brief 买入
     * @param price 买入价格
     * @param quantity 买入数量
     * @param commission 手续费
     */
    void buy(double price, int quantity, double commission = 0.0);
    
    /**
     * @brief 卖出
     * @param price 卖出价格
     * @param quantity 卖出数量
     * @param commission 手续费
     */
    void sell(double price, int quantity, double commission = 0.0);
    
    /**
     * @brief 是否为空仓
     */
    bool isEmpty() const { return quantity_ == 0; }
    
    /**
     * @brief 计算盈亏
     */
    void calculateProfit();
    
    /**
     * @brief 转换为字符串表示
     */
    std::string toString() const;

private:
    std::string tsCode_;            // 股票代码
    int quantity_;                  // 持仓数量
    double avgCost_;                // 平均成本
    double totalCost_;              // 总成本
    double currentPrice_;           // 当前价格
    double marketValue_;            // 市值
    double profit_;                 // 盈亏
    double profitRate_;             // 盈亏率
    std::vector<TradePtr> trades_;  // 交易记录
    
    /**
     * @brief 重新计算平均成本
     */
    void recalculateAvgCost();
    
    /**
     * @brief 更新市值
     */
    void updateMarketValue();
};

using PositionPtr = std::shared_ptr<Position>;

} // namespace core

#endif // CORE_POSITION_H

