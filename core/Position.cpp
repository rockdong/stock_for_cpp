#include "Position.h"
#include <sstream>
#include <iomanip>
#include <stdexcept>

namespace core {

Position::Position(const std::string& tsCode)
    : tsCode_(tsCode)
    , quantity_(0)
    , avgCost_(0.0)
    , totalCost_(0.0)
    , currentPrice_(0.0)
    , marketValue_(0.0)
    , profit_(0.0)
    , profitRate_(0.0)
{
}

void Position::updatePrice(double price) {
    if (price < 0) {
        throw std::invalid_argument("Price cannot be negative");
    }
    currentPrice_ = price;
    updateMarketValue();
    calculateProfit();
}

void Position::addTrade(TradePtr trade) {
    if (!trade) {
        throw std::invalid_argument("Trade cannot be null");
    }
    
    if (trade->getTsCode() != tsCode_) {
        throw std::invalid_argument("Trade tsCode does not match position tsCode");
    }
    
    trades_.push_back(trade);
    
    // 更新持仓数量和成本
    if (trade->getType() == TradeType::BUY) {
        totalCost_ += trade->getTotalCost();
        quantity_ += trade->getQuantity();
    } else if (trade->getType() == TradeType::SELL) {
        // 卖出时减少持仓
        quantity_ -= trade->getQuantity();
        if (quantity_ < 0) {
            throw std::runtime_error("Sell quantity exceeds current position");
        }
        // 按比例减少总成本
        if (quantity_ + trade->getQuantity() > 0) {
            double costRatio = static_cast<double>(quantity_) / (quantity_ + trade->getQuantity());
            totalCost_ *= costRatio;
        } else {
            totalCost_ = 0.0;
        }
    }
    
    recalculateAvgCost();
    updateMarketValue();
    calculateProfit();
}

void Position::buy(double price, int quantity, double commission) {
    auto trade = TradeBuilder()
        .tsCode(tsCode_)
        .buy()
        .price(price)
        .quantity(quantity)
        .commission(commission)
        .build();
    
    trade->setStatus(TradeStatus::COMPLETED);
    addTrade(trade);
}

void Position::sell(double price, int quantity, double commission) {
    if (quantity > quantity_) {
        throw std::runtime_error("Sell quantity exceeds current position");
    }
    
    auto trade = TradeBuilder()
        .tsCode(tsCode_)
        .sell()
        .price(price)
        .quantity(quantity)
        .commission(commission)
        .build();
    
    trade->setStatus(TradeStatus::COMPLETED);
    addTrade(trade);
}

void Position::calculateProfit() {
    if (quantity_ > 0 && currentPrice_ > 0) {
        profit_ = marketValue_ - totalCost_;
        profitRate_ = totalCost_ > 0 ? (profit_ / totalCost_) * 100.0 : 0.0;
    } else {
        profit_ = 0.0;
        profitRate_ = 0.0;
    }
}

void Position::recalculateAvgCost() {
    if (quantity_ > 0) {
        avgCost_ = totalCost_ / quantity_;
    } else {
        avgCost_ = 0.0;
    }
}

void Position::updateMarketValue() {
    marketValue_ = currentPrice_ * quantity_;
}

std::string Position::toString() const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    oss << "Position[tsCode=" << tsCode_
        << ", quantity=" << quantity_
        << ", avgCost=" << avgCost_
        << ", totalCost=" << totalCost_
        << ", currentPrice=" << currentPrice_
        << ", marketValue=" << marketValue_
        << ", profit=" << profit_
        << ", profitRate=" << profitRate_ << "%"
        << ", trades=" << trades_.size()
        << "]";
    return oss.str();
}

} // namespace core

