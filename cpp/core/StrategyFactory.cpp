#include "StrategyFactory.h"
#include "strategies/MACrossStrategy.h"
#include "strategies/MACDStrategy.h"
#include "strategies/RSIStrategy.h"
#include "strategies/BOLLStrategy.h"
#include "strategies/GridStrategy.h"
#include "strategies/EMA17TO25Strategy.h"
#include "strategies/EMA17BreakoutStrategy.h"
#include "strategies/EMAConvergenceStrategy.h"
#include "strategies/EMA25Greater17PriceMatchStrategy.h"
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
        
        case StrategyType::EMA17TO25:
            return std::make_shared<EMA17TO25Strategy>(params);
        
        case StrategyType::EMA17_BREAKOUT:
            return std::make_shared<EMA17BreakoutStrategy>(params);
        
        case StrategyType::EMA_CONVERGENCE:
            return std::make_shared<EMAConvergenceStrategy>(params);
        
        case StrategyType::EMA25_GREATER_17_PRICE_MATCH:
            return std::make_shared<EMA25Greater17PriceMatchStrategy>(params);
        
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
    } else if (name == "EMA17TO25" || name == "EMA17_25" || name == "ema17to25") {
        return create(StrategyType::EMA17TO25, params);
    } else if (name == "EMA17_BREAKOUT" || name == "EMA17BREAKOUT" || name == "ema17_breakout") {
        return create(StrategyType::EMA17_BREAKOUT, params);
    } else if (name == "EMA_CONVERGENCE" || name == "EMACONVERGENCE" || name == "ema_convergence") {
        return create(StrategyType::EMA_CONVERGENCE, params);
    } else if (name == "EMA25_GREATER_17_PRICE_MATCH" || name == "EMA25_GREATER17" || name == "ema25_greater_17_match") {
        return create(StrategyType::EMA25_GREATER_17_PRICE_MATCH, params);
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
        "GRID",
        "EMA17TO25",
        "EMA17_BREAKOUT",
        "EMA_CONVERGENCE"
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
        case StrategyType::EMA17TO25: return "EMA17TO25";
        case StrategyType::EMA17_BREAKOUT: return "EMA17_BREAKOUT";
        case StrategyType::EMA_CONVERGENCE: return "EMA_CONVERGENCE";
        case StrategyType::EMA25_GREATER_17_PRICE_MATCH: return "EMA25_GREATER_17_PRICE_MATCH";
        case StrategyType::CUSTOM: return "CUSTOM";
        default: return "UNKNOWN";
    }
}

} // namespace core

