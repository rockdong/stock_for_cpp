#ifndef CORE_TRADE_H
#define CORE_TRADE_H

#include <string>
#include <memory>
#include "Stock.h"

namespace core {

/**
 * @brief 交易类型枚举
 */
enum class TradeType {
    BUY,    // 买入
    SELL    // 卖出
};

/**
 * @brief 交易状态枚举
 */
enum class TradeStatus {
    PENDING,    // 待成交
    COMPLETED,  // 已成交
    CANCELLED   // 已取消
};

/**
 * @brief 交易记录
 * 
 * 记录单次交易的详细信息
 * 遵循单一职责原则（SRP）
 */
class Trade {
public:
    /**
     * @brief 构造函数
     * @param tsCode 股票代码
     * @param type 交易类型
     * @param price 交易价格
     * @param quantity 交易数量
     * @param tradeTime 交易时间
     */
    Trade(const std::string& tsCode,
          TradeType type,
          double price,
          int quantity,
          const std::string& tradeTime = "");
    
    // Getters
    int getId() const { return id_; }
    std::string getTsCode() const { return tsCode_; }
    TradeType getType() const { return type_; }
    double getPrice() const { return price_; }
    int getQuantity() const { return quantity_; }
    double getAmount() const { return amount_; }
    double getCommission() const { return commission_; }
    double getTotalCost() const { return totalCost_; }
    std::string getTradeTime() const { return tradeTime_; }
    TradeStatus getStatus() const { return status_; }
    std::string getNotes() const { return notes_; }
    
    // Setters
    void setId(int id) { id_ = id; }
    void setCommission(double commission);
    void setStatus(TradeStatus status) { status_ = status; }
    void setNotes(const std::string& notes) { notes_ = notes; }
    
    /**
     * @brief 获取交易类型字符串
     */
    std::string getTypeString() const;
    
    /**
     * @brief 获取交易状态字符串
     */
    std::string getStatusString() const;
    
    /**
     * @brief 计算盈亏（相对于另一笔交易）
     * @param other 另一笔交易
     * @return 盈亏金额
     */
    double calculateProfit(const Trade& other) const;
    
    /**
     * @brief 转换为字符串表示
     */
    std::string toString() const;

private:
    int id_;                    // 交易ID
    std::string tsCode_;        // 股票代码
    TradeType type_;            // 交易类型
    double price_;              // 交易价格
    int quantity_;              // 交易数量
    double amount_;             // 交易金额（价格 * 数量）
    double commission_;         // 手续费
    double totalCost_;          // 总成本（金额 + 手续费）
    std::string tradeTime_;     // 交易时间
    TradeStatus status_;        // 交易状态
    std::string notes_;         // 备注
    
    /**
     * @brief 计算交易金额和总成本
     */
    void calculateAmounts();
};

using TradePtr = std::shared_ptr<Trade>;

/**
 * @brief Trade 建造者模式
 * 
 * 使用建造者模式构建复杂的 Trade 对象
 * 提供流式接口，调用更优雅
 */
class TradeBuilder {
public:
    TradeBuilder() = default;
    
    TradeBuilder& tsCode(const std::string& tsCode) {
        tsCode_ = tsCode;
        return *this;
    }
    
    TradeBuilder& type(TradeType type) {
        type_ = type;
        return *this;
    }
    
    TradeBuilder& buy() {
        type_ = TradeType::BUY;
        return *this;
    }
    
    TradeBuilder& sell() {
        type_ = TradeType::SELL;
        return *this;
    }
    
    TradeBuilder& price(double price) {
        price_ = price;
        return *this;
    }
    
    TradeBuilder& quantity(int quantity) {
        quantity_ = quantity;
        return *this;
    }
    
    TradeBuilder& tradeTime(const std::string& tradeTime) {
        tradeTime_ = tradeTime;
        return *this;
    }
    
    TradeBuilder& commission(double commission) {
        commission_ = commission;
        return *this;
    }
    
    TradeBuilder& notes(const std::string& notes) {
        notes_ = notes;
        return *this;
    }
    
    /**
     * @brief 构建 Trade 对象
     */
    TradePtr build() const;

private:
    std::string tsCode_;
    TradeType type_ = TradeType::BUY;
    double price_ = 0.0;
    int quantity_ = 0;
    std::string tradeTime_;
    double commission_ = 0.0;
    std::string notes_;
};

} // namespace core

#endif // CORE_TRADE_H

