#ifndef CORE_PORTFOLIO_H
#define CORE_PORTFOLIO_H

#include <string>
#include <map>
#include <memory>
#include <vector>
#include "Position.h"
#include "Trade.h"

namespace core {

/**
 * @brief 投资组合
 * 
 * 管理多只股票的持仓和交易
 * 遵循单一职责原则（SRP）
 */
class Portfolio {
public:
    /**
     * @brief 构造函数
     * @param name 组合名称
     * @param initialCash 初始资金
     */
    explicit Portfolio(const std::string& name = "Default", double initialCash = 0.0);
    
    // Getters
    std::string getName() const { return name_; }
    double getInitialCash() const { return initialCash_; }
    double getCash() const { return cash_; }
    double getTotalValue() const { return totalValue_; }
    double getTotalProfit() const { return totalProfit_; }
    double getTotalProfitRate() const { return totalProfitRate_; }
    const std::map<std::string, PositionPtr>& getPositions() const { return positions_; }
    const std::vector<TradePtr>& getTrades() const { return trades_; }
    
    /**
     * @brief 获取指定股票的持仓
     * @param tsCode 股票代码
     * @return 持仓指针，如果不存在返回 nullptr
     */
    PositionPtr getPosition(const std::string& tsCode) const;
    
    /**
     * @brief 添加资金
     * @param amount 金额
     */
    void addCash(double amount);
    
    /**
     * @brief 买入股票
     * @param tsCode 股票代码
     * @param price 买入价格
     * @param quantity 买入数量
     * @param commission 手续费
     * @return 是否成功
     */
    bool buy(const std::string& tsCode, double price, int quantity, double commission = 0.0);
    
    /**
     * @brief 卖出股票
     * @param tsCode 股票代码
     * @param price 卖出价格
     * @param quantity 卖出数量
     * @param commission 手续费
     * @return 是否成功
     */
    bool sell(const std::string& tsCode, double price, int quantity, double commission = 0.0);
    
    /**
     * @brief 更新股票价格
     * @param tsCode 股票代码
     * @param price 当前价格
     */
    void updatePrice(const std::string& tsCode, double price);
    
    /**
     * @brief 更新所有持仓价格
     * @param prices 股票代码到价格的映射
     */
    void updatePrices(const std::map<std::string, double>& prices);
    
    /**
     * @brief 计算总价值和盈亏
     */
    void calculate();
    
    /**
     * @brief 获取持仓数量
     */
    size_t getPositionCount() const { return positions_.size(); }
    
    /**
     * @brief 获取交易数量
     */
    size_t getTradeCount() const { return trades_.size(); }
    
    /**
     * @brief 清空所有持仓（用于回测重置）
     */
    void clear();
    
    /**
     * @brief 重置为初始状态
     */
    void reset();
    
    /**
     * @brief 转换为字符串表示
     */
    std::string toString() const;
    
    /**
     * @brief 获取持仓摘要
     */
    std::string getPositionsSummary() const;

private:
    std::string name_;                          // 组合名称
    double initialCash_;                        // 初始资金
    double cash_;                               // 当前现金
    double totalValue_;                         // 总价值（现金 + 持仓市值）
    double totalProfit_;                        // 总盈亏
    double totalProfitRate_;                    // 总盈亏率
    std::map<std::string, PositionPtr> positions_;  // 持仓映射
    std::vector<TradePtr> trades_;              // 所有交易记录
    
    /**
     * @brief 获取或创建持仓
     * @param tsCode 股票代码
     * @return 持仓指针
     */
    PositionPtr getOrCreatePosition(const std::string& tsCode);
};

using PortfolioPtr = std::shared_ptr<Portfolio>;

} // namespace core

#endif // CORE_PORTFOLIO_H

