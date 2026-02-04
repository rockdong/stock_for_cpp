#include "Strategy.h"
#include <sstream>
#include <iomanip>
#include <ctime>

namespace core {

std::string TradeSignal::toString() const {
    std::ostringstream oss;
    oss << "TradeSignal[tsCode=" << tsCode
        << ", signal=";
    
    switch (signal) {
        case Signal::BUY: oss << "BUY"; break;
        case Signal::SELL: oss << "SELL"; break;
        case Signal::HOLD: oss << "HOLD"; break;
        case Signal::NONE: oss << "NONE"; break;
    }
    
    oss << ", strength=";
    switch (strength) {
        case SignalStrength::WEAK: oss << "WEAK"; break;
        case SignalStrength::MEDIUM: oss << "MEDIUM"; break;
        case SignalStrength::STRONG: oss << "STRONG"; break;
    }
    
    oss << std::fixed << std::setprecision(2);
    oss << ", price=" << price
        << ", quantity=" << quantity
        << ", reason=" << reason
        << ", timestamp=" << timestamp
        << "]";
    
    return oss.str();
}

// StrategyBase implementation
StrategyBase::StrategyBase(const std::string& name, const std::string& description)
    : name_(name)
    , description_(description)
{
}

void StrategyBase::setParameters(const std::map<std::string, double>& params) {
    parameters_ = params;
}

std::map<std::string, double> StrategyBase::getParameters() const {
    return parameters_;
}

bool StrategyBase::validateParameters() const {
    // 基类默认验证通过，子类可以重写
    return true;
}

double StrategyBase::getParameter(const std::string& key, double defaultValue) const {
    auto it = parameters_.find(key);
    if (it != parameters_.end()) {
        return it->second;
    }
    return defaultValue;
}

void StrategyBase::setParameter(const std::string& key, double value) {
    parameters_[key] = value;
}

bool StrategyBase::hasEnoughData(const std::vector<StockData>& data, size_t minSize) const {
    return data.size() >= minSize;
}

TradeSignal StrategyBase::createSignal(
    const std::string& tsCode,
    Signal signal,
    SignalStrength strength,
    double price,
    int quantity,
    const std::string& reason
) const {
    TradeSignal tradeSignal;
    tradeSignal.tsCode = tsCode;
    tradeSignal.signal = signal;
    tradeSignal.strength = strength;
    tradeSignal.price = price;
    tradeSignal.quantity = quantity;
    tradeSignal.reason = reason;
    
    // 设置时间戳
    std::time_t now = std::time(nullptr);
    char buf[20];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    tradeSignal.timestamp = buf;
    
    return tradeSignal;
}

} // namespace core

