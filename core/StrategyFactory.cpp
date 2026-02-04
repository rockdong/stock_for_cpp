#include "StrategyFactory.h"
#include "strategies/MACrossStrategy.h"
#include "strategies/MACDStrategy.h"
#include "strategies/RSIStrategy.h"
#include "strategies/BOLLStrategy.h"
#include "strategies/GridStrategy.h"
#include <stdexcept>

namespace core {

// 静态成员初始化
std::map<std::string, std::function<StrategyPtr(const std::map<std::string, double>&)>> 
    StrategyFactory::creators_;

StrategyPtr StrategyFactory::create(StrategyType type, const std::map<std::string, double>& params) {
    switch (type) {
        case StrategyType::MA_CROSS:
            return std::make_shared<MACrossStrategy>(params);
        
        case StrategyType::MACD:
            return std::make_shared<MACDStrategy>(params);
        
        case StrategyType::RSI:
            return std::make_shared<RSIStrategy>(params);
        
        case StrategyType::BOLL:
            return std::make_shared<BOLLStrategy>(params);
        
        case StrategyType::GRID:
            return std::make_shared<GridStrategy>(params);
        
        default:
            throw std::invalid_argument("Unknown strategy type");
    }
}

StrategyPtr StrategyFactory::create(const std::string& name, const std::map<std::string, double>& params) {
    // 首先检查自定义策略
    auto it = creators_.find(name);
    if (it != creators_.end()) {
        return it->second(params);
    }
    
    // 内置策略
    if (name == "MA_CROSS" || name == "MACross") {
        return create(StrategyType::MA_CROSS, params);
    } else if (name == "MACD") {
        return create(StrategyType::MACD, params);
    } else if (name == "RSI") {
        return create(StrategyType::RSI, params);
    } else if (name == "BOLL" || name == "Bollinger") {
        return create(StrategyType::BOLL, params);
    } else if (name == "GRID" || name == "Grid") {
        return create(StrategyType::GRID, params);
    }
    
    throw std::invalid_argument("Unknown strategy name: " + name);
}

void StrategyFactory::registerStrategy(
    const std::string& name,
    std::function<StrategyPtr(const std::map<std::string, double>&)> creator
) {
    creators_[name] = creator;
}

std::vector<std::string> StrategyFactory::getSupportedStrategies() {
    std::vector<std::string> strategies = {
        "MA_CROSS",
        "MACD",
        "RSI",
        "BOLL",
        "GRID"
    };
    
    // 添加自定义策略
    for (const auto& [name, _] : creators_) {
        strategies.push_back(name);
    }
    
    return strategies;
}

std::string StrategyFactory::getStrategyName(StrategyType type) {
    switch (type) {
        case StrategyType::MA_CROSS: return "MA_CROSS";
        case StrategyType::MACD: return "MACD";
        case StrategyType::RSI: return "RSI";
        case StrategyType::BOLL: return "BOLL";
        case StrategyType::GRID: return "GRID";
        case StrategyType::CUSTOM: return "CUSTOM";
        default: return "UNKNOWN";
    }
}

} // namespace core

