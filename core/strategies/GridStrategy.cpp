#include "GridStrategy.h"
#include <cmath>
#include <algorithm>

namespace core {

GridStrategy::GridStrategy(const std::map<std::string, double>& params)
    : StrategyBase("GRID", "网格交易策略")
{
    // 设置默认参数
    setParameter("grid_size", 5.0);
    setParameter("base_price", 0.0); // 0 表示使用当前价格
    setParameter("max_grids", 10);
    
    // 应用用户参数
    setParameters(params);
}

TradeSignal GridStrategy::analyze(
    const std::string& tsCode,
    const std::vector<StockData>& data,
    const Portfolio& portfolio
) {
    double gridSize = getParameter("grid_size", 5.0);
    double basePrice = getParameter("base_price", 0.0);
    int maxGrids = static_cast<int>(getParameter("max_grids", 10));
    
    // 检查数据是否足够
    if (!hasEnoughData(data, 2)) {
        return createSignal(tsCode, Signal::NONE, SignalStrength::WEAK, 0.0, 0,
                          "数据不足");
    }
    
    // 获取当前价格和前一天价格
    double currentPrice = data.back().close;
    double prevPrice = data[data.size() - 2].close;
    
    // 如果没有设置基准价格，使用当前价格
    if (basePrice <= 0) {
        basePrice = currentPrice;
    }
    
    // 计算网格价格
    auto gridPrices = calculateGridPrices(basePrice, gridSize, maxGrids);
    
    // 找到当前价格和前一天价格最近的网格
    double currentGrid = findNearestGrid(currentPrice, gridPrices);
    double prevGrid = findNearestGrid(prevPrice, gridPrices);
    
    // 价格下穿网格线（买入信号）
    if (currentPrice < currentGrid && prevPrice >= prevGrid && currentGrid < prevGrid) {
        double availableCash = portfolio.getCash();
        int quantity = static_cast<int>(availableCash * 0.2 / currentPrice / 100) * 100; // 使用20%资金
        
        if (quantity > 0) {
            return createSignal(
                tsCode,
                Signal::BUY,
                SignalStrength::MEDIUM,
                currentPrice,
                quantity,
                "价格下穿网格线 " + std::to_string(currentGrid) + "，买入信号"
            );
        }
    }
    
    // 价格上穿网格线（卖出信号）
    if (currentPrice > currentGrid && prevPrice <= prevGrid && currentGrid > prevGrid) {
        auto position = portfolio.getPosition(tsCode);
        if (position && position->getQuantity() > 0) {
            // 卖出部分持仓
            int quantity = std::min(position->getQuantity(), 
                                   static_cast<int>(position->getQuantity() * 0.3));
            quantity = (quantity / 100) * 100; // 调整为100的倍数
            
            if (quantity > 0) {
                return createSignal(
                    tsCode,
                    Signal::SELL,
                    SignalStrength::MEDIUM,
                    currentPrice,
                    quantity,
                    "价格上穿网格线 " + std::to_string(currentGrid) + "，卖出信号"
                );
            }
        }
    }
    
    // 无明确信号
    return createSignal(tsCode, Signal::HOLD, SignalStrength::WEAK, currentPrice, 0, "无明确交易信号");
}

bool GridStrategy::validateParameters() const {
    double gridSize = getParameter("grid_size", 5.0);
    int maxGrids = static_cast<int>(getParameter("max_grids", 10));
    
    if (gridSize <= 0 || gridSize > 50) {
        return false;
    }
    
    if (maxGrids <= 0 || maxGrids > 100) {
        return false;
    }
    
    return true;
}

std::vector<double> GridStrategy::calculateGridPrices(
    double basePrice,
    double gridSize,
    int maxGrids
) const {
    std::vector<double> gridPrices;
    double gridStep = basePrice * (gridSize / 100.0);
    
    // 生成上下网格
    for (int i = -maxGrids; i <= maxGrids; ++i) {
        gridPrices.push_back(basePrice + i * gridStep);
    }
    
    return gridPrices;
}

double GridStrategy::findNearestGrid(
    double currentPrice,
    const std::vector<double>& gridPrices
) const {
    if (gridPrices.empty()) {
        return currentPrice;
    }
    
    double nearest = gridPrices[0];
    double minDiff = std::abs(currentPrice - nearest);
    
    for (double gridPrice : gridPrices) {
        double diff = std::abs(currentPrice - gridPrice);
        if (diff < minDiff) {
            minDiff = diff;
            nearest = gridPrice;
        }
    }
    
    return nearest;
}

} // namespace core

