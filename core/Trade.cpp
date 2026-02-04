#include "Trade.h"
#include <sstream>
#include <iomanip>
#include <ctime>

namespace core {

Trade::Trade(const std::string& tsCode,
             TradeType type,
             double price,
             int quantity,
             const std::string& tradeTime)
    : id_(0)
    , tsCode_(tsCode)
    , type_(type)
    , price_(price)
    , quantity_(quantity)
    , amount_(0.0)
    , commission_(0.0)
    , totalCost_(0.0)
    , tradeTime_(tradeTime)
    , status_(TradeStatus::PENDING)
    , notes_("")
{
    // 如果没有提供交易时间，使用当前时间
    if (tradeTime_.empty()) {
        std::time_t now = std::time(nullptr);
        char buf[20];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
        tradeTime_ = buf;
    }
    
    calculateAmounts();
}

void Trade::setCommission(double commission) {
    commission_ = commission;
    calculateAmounts();
}

void Trade::calculateAmounts() {
    amount_ = price_ * quantity_;
    totalCost_ = amount_ + commission_;
}

std::string Trade::getTypeString() const {
    switch (type_) {
        case TradeType::BUY:
            return "BUY";
        case TradeType::SELL:
            return "SELL";
        default:
            return "UNKNOWN";
    }
}

std::string Trade::getStatusString() const {
    switch (status_) {
        case TradeStatus::PENDING:
            return "PENDING";
        case TradeStatus::COMPLETED:
            return "COMPLETED";
        case TradeStatus::CANCELLED:
            return "CANCELLED";
        default:
            return "UNKNOWN";
    }
}

double Trade::calculateProfit(const Trade& other) const {
    // 计算两笔交易之间的盈亏
    // 假设 this 是卖出，other 是买入
    if (type_ == TradeType::SELL && other.type_ == TradeType::BUY) {
        return (price_ - other.price_) * quantity_ - commission_ - other.commission_;
    }
    // 假设 this 是买入，other 是卖出
    else if (type_ == TradeType::BUY && other.type_ == TradeType::SELL) {
        return (other.price_ - price_) * quantity_ - commission_ - other.commission_;
    }
    return 0.0;
}

std::string Trade::toString() const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    oss << "Trade[id=" << id_
        << ", tsCode=" << tsCode_
        << ", type=" << getTypeString()
        << ", price=" << price_
        << ", quantity=" << quantity_
        << ", amount=" << amount_
        << ", commission=" << commission_
        << ", totalCost=" << totalCost_
        << ", time=" << tradeTime_
        << ", status=" << getStatusString()
        << "]";
    return oss.str();
}

// TradeBuilder implementation
TradePtr TradeBuilder::build() const {
    auto trade = std::make_shared<Trade>(tsCode_, type_, price_, quantity_, tradeTime_);
    trade->setCommission(commission_);
    trade->setNotes(notes_);
    return trade;
}

} // namespace core

