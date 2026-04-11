#ifndef CORE_STRATEGIES_SURGE_SIGNAL_STRATEGY_H
#define CORE_STRATEGIES_SURGE_SIGNAL_STRATEGY_H

#include "../Strategy.h"

namespace core {

/**
 * @brief 暴涨预兆策略
 * 
 * 基于个股暴涨预兆与量化指标体系实现的多维度信号检测策略。
 * 检测以下关键信号：
 * 
 * 量价维度：
 * - 倍量试盘：成交量达前5日均量2倍以上（⭐⭐⭐⭐⭐）
 * - 缩量回调：价跌量缩至前日50%以下（⭐⭐⭐⭐）
 * 
 * 技术维度：
 * - 均线粘合：5/10/20日均线偏离度≤2%（⭐⭐⭐⭐）
 * - MACD底背离：股价新低+MACD不新低（⭐⭐⭐⭐⭐）
 * - OBV底背离：股价新低+OBV不新低（⭐⭐⭐⭐）
 * 
 * 参数：
 * - volume_multiplier: 倍量试盘倍数（默认 2.0）
 * - shrink_ratio: 缩量回调比例（默认 0.5）
 * - ma_deviation_threshold: 均线粘合偏离度阈值（默认 0.02）
 * - lookback_period: 底背离检测周期（默认 20）
 */
class SurgeSignalStrategy : public StrategyBase {
public:
    explicit SurgeSignalStrategy(const std::map<std::string, double>& params = {});
    
    std::optional<AnalysisResult> analyze(
        const std::string& tsCode,
        const std::vector<StockData>& data
    ) override;
    
    bool validateParameters() const override;

private:
    /**
     * @brief 信号检测结果
     */
    struct SignalResult {
        bool detected = false;
        double strength = 0.0;        // 信号强度 (0-1)
        std::string description;       // 信号描述
    };
    
    /**
     * @brief 综合检测结果
     */
    struct SurgeAnalysis {
        SignalResult volumeTest;       // 倍量试盘
        SignalResult shrinkBack;       // 缩量回调
        SignalResult maConvergence;    // 均线粘合
        SignalResult macdDivergence;   // MACD底背离
        SignalResult obvDivergence;    // OBV底背离
        int totalScore = 0;            // 总分
        double confidence = 0.0;       // 置信度
    };
    
    /**
     * @brief 计算移动平均线
     */
    std::vector<double> calculateMA(const std::vector<double>& prices, int period) const;
    
    /**
     * @brief 计算EMA
     */
    std::vector<double> calculateEMA(const std::vector<double>& prices, int period) const;
    
    /**
     * @brief 计算MACD
     */
    struct MACDResult {
        std::vector<double> macd;
        std::vector<double> signal;
        std::vector<double> histogram;
    };
    MACDResult calculateMACD(const std::vector<double>& prices, int fast, int slow, int signal) const;
    
    /**
     * @brief 计算OBV
     */
    std::vector<double> calculateOBV(const std::vector<StockData>& data) const;
    
    /**
     * @brief 检测倍量试盘
     */
    SignalResult detectVolumeTest(const std::vector<StockData>& data) const;
    
    /**
     * @brief 检测缩量回调
     */
    SignalResult detectShrinkBack(const std::vector<StockData>& data) const;
    
    /**
     * @brief 检测均线粘合
     */
    SignalResult detectMAConvergence(const std::vector<double>& prices) const;
    
    /**
     * @brief 检测MACD底背离
     */
    SignalResult detectMACDDivergence(const std::vector<double>& prices) const;
    
    /**
     * @brief 检测OBV底背离
     */
    SignalResult detectOBVDivergence(const std::vector<StockData>& data) const;
    
    /**
     * @brief 综合分析
     */
    SurgeAnalysis performAnalysis(const std::string& tsCode, const std::vector<StockData>& data) const;
    
    /**
     * @brief 根据信号数量评估信号强度
     */
    SignalStrength evaluateSignalStrength(int score, double confidence) const;
};

} // namespace core

#endif // CORE_STRATEGIES_SURGE_SIGNAL_STRATEGY_H