#ifndef CORE_STRATEGIES_GRID_STRATEGY_H
#define CORE_STRATEGIES_GRID_STRATEGY_H

#include "../Strategy.h"

namespace core {

/**
 * @brief 网格交易策略
 * 
 * 在价格区间内设置网格，价格下跌时买入，上涨时卖出
 * 
 * 参数：
 * - grid_size: 网格大小（百分比，默认 5.0）
 * - base_price: 基准价格（默认使用当前价格）
 * - max_grids: 最大网格数（默认 10）
 */
class GridStrategy : public StrategyBase {
public:
    explicit GridStrategy(const std::map<std::string, double>& params = {});
    
    TradeSignal analyze(
        const std::string& tsCode,
        const std::vector<StockData>& data,
        const Portfolio& portfolio
    ) override;
    
    bool validateParameters() const override;

private:
    /**
     * @brief 计算网格价格
     */
    std::vector<double> calculateGridPrices(double basePrice, double gridSize, int maxGrids) const;
    
    /**
     * @brief 找到最近的网格价格
     */
    double findNearestGrid(double currentPrice, const std::vector<double>& gridPrices) const;
};

} // namespace core

#endif // CORE_STRATEGIES_GRID_STRATEGY_H

