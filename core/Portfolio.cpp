#include "Portfolio.h"
#include <sstream>
#include <iomanip>
#include <stdexcept>

namespace core {

Portfolio::Portfolio(const std::string& name, double initialCash)
    : name_(name)
    , initialCash_(initialCash)
    , cash_(initialCash)
    , totalValue_(initialCash)
    , totalProfit_(0.0)
    , totalProfitRate_(0.0)
{
    if (initialCash < 0) {
        throw std::invalid_argument("Initial cash cannot be negative");
    }
}

PositionPtr Portfolio::getPosition(const std::string& tsCode) const {
    auto it = positions_.find(tsCode);
    if (it != positions_.end()) {
        return it->second;
    }
    return nullptr;
}

void Portfolio::addCash(double amount) {
    if (amount < 0) {
        throw std::invalid_argument("Amount cannot be negative");
    }
    cash_ += amount;
    initialCash_ += amount;
    calculate();
}

bool Portfolio::buy(const std::string& tsCode, double price, int quantity, double commission) {
    if (price <= 0 || quantity <= 0) {
        return false;
    }
    
    double totalCost = price * quantity + commission;
    
    // 检查资金是否足够
    if (totalCost > cash_) {
        return false;
    }
    
    // 扣除现金
    cash_ -= totalCost;
    
    // 获取或创建持仓
    auto position = getOrCreatePosition(tsCode);
    
    // 创建交易记录
    auto trade = TradeBuilder()
        .tsCode(tsCode)
        .buy()
        .price(price)
        .quantity(quantity)
        .commission(commission)
        .build();
    
    trade->setStatus(TradeStatus::COMPLETED);
    
    // 添加到持仓和交易记录
    position->addTrade(trade);
    trades_.push_back(trade);
    
    // 重新计算
    calculate();
    
    return true;
}

bool Portfolio::sell(const std::string& tsCode, double price, int quantity, double commission) {
    if (price <= 0 || quantity <= 0) {
        return false;
    }
    
    // 检查持仓是否存在
    auto position = getPosition(tsCode);
    if (!position) {
        return false;
    }
    
    // 检查持仓数量是否足够
    if (position->getQuantity() < quantity) {
        return false;
    }
    
    // 增加现金
    double totalAmount = price * quantity - commission;
    cash_ += totalAmount;
    
    // 创建交易记录
    auto trade = TradeBuilder()
        .tsCode(tsCode)
        .sell()
        .price(price)
        .quantity(quantity)
        .commission(commission)
        .build();
    
    trade->setStatus(TradeStatus::COMPLETED);
    
    // 添加到持仓和交易记录
    position->addTrade(trade);
    trades_.push_back(trade);
    
    // 如果持仓清空，从持仓列表中移除
    if (position->isEmpty()) {
        positions_.erase(tsCode);
    }
    
    // 重新计算
    calculate();
    
    return true;
}

void Portfolio::updatePrice(const std::string& tsCode, double price) {
    auto position = getPosition(tsCode);
    if (position) {
        position->updatePrice(price);
        calculate();
    }
}

void Portfolio::updatePrices(const std::map<std::string, double>& prices) {
    for (const auto& [tsCode, price] : prices) {
        auto position = getPosition(tsCode);
        if (position) {
            position->updatePrice(price);
        }
    }
    calculate();
}

void Portfolio::calculate() {
    // 计算总市值
    double totalMarketValue = 0.0;
    double totalCost = 0.0;
    
    for (const auto& [tsCode, position] : positions_) {
        totalMarketValue += position->getMarketValue();
        totalCost += position->getTotalCost();
    }
    
    // 总价值 = 现金 + 持仓市值
    totalValue_ = cash_ + totalMarketValue;
    
    // 总盈亏 = 总价值 - 初始资金
    totalProfit_ = totalValue_ - initialCash_;
    
    // 总盈亏率
    totalProfitRate_ = initialCash_ > 0 ? (totalProfit_ / initialCash_) * 100.0 : 0.0;
}

void Portfolio::clear() {
    positions_.clear();
    trades_.clear();
    cash_ = initialCash_;
    totalValue_ = initialCash_;
    totalProfit_ = 0.0;
    totalProfitRate_ = 0.0;
}

void Portfolio::reset() {
    clear();
}

PositionPtr Portfolio::getOrCreatePosition(const std::string& tsCode) {
    auto it = positions_.find(tsCode);
    if (it != positions_.end()) {
        return it->second;
    }
    
    // 创建新持仓
    auto position = std::make_shared<Position>(tsCode);
    positions_[tsCode] = position;
    return position;
}

std::string Portfolio::toString() const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    oss << "Portfolio[name=" << name_
        << ", initialCash=" << initialCash_
        << ", cash=" << cash_
        << ", totalValue=" << totalValue_
        << ", totalProfit=" << totalProfit_
        << ", totalProfitRate=" << totalProfitRate_ << "%"
        << ", positions=" << positions_.size()
        << ", trades=" << trades_.size()
        << "]";
    return oss.str();
}

std::string Portfolio::getPositionsSummary() const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    oss << "Portfolio: " << name_ << "\n";
    oss << "Cash: " << cash_ << "\n";
    oss << "Total Value: " << totalValue_ << "\n";
    oss << "Total Profit: " << totalProfit_ << " (" << totalProfitRate_ << "%)\n";
    oss << "\nPositions:\n";
    
    if (positions_.empty()) {
        oss << "  No positions\n";
    } else {
        for (const auto& [tsCode, position] : positions_) {
            oss << "  " << position->toString() << "\n";
        }
    }
    
    return oss.str();
}

} // namespace core

