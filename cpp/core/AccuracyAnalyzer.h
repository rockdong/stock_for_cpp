#ifndef CORE_ACCURACY_ANALYZER_H
#define CORE_ACCURACY_ANALYZER_H

#include <string>
#include <vector>
#include <map>
#include "PredictionHistory.h"

namespace core {

struct AccuracyStats {
    int total_predictions = 0;          // 总预测次数
    int accurate_predictions = 0;       // 准确预测次数
    double accuracy_rate = 0.0;         // 准确率
    double avg_predicted_gain = 0.0;    // 平均预测涨幅
    double avg_actual_gain = 0.0;       // 平均实际涨幅
    double avg_accuracy_score = 0.0;    // 平均准确度评分
    
    // 按方向统计
    int up_predictions = 0;             // 预测上涨次数
    int up_accurate = 0;                // 上涨预测准确次数
    int down_predictions = 0;           // 预测下跌次数
    int down_accurate = 0;              // 下跌预测准确次数
    
    // 按置信度统计
    double high_confidence_accuracy = 0.0;  // 高置信度预测准确率
    double low_confidence_accuracy = 0.0;   // 低置信度预测准确率
};

struct FailurePattern {
    std::string pattern_name;           // 模式名称
    std::string description;            // 模式描述
    int occurrence_count = 0;           // 出现次数
    double failure_rate = 0.0;          // 失败率
    std::map<std::string, double> common_indicators;  // 失败时的常见指标值
};

class AccuracyAnalyzer {
public:
    AccuracyAnalyzer();
    explicit AccuracyAnalyzer(const PredictionHistory& history);
    
    void setHistory(const PredictionHistory& history);
    
    AccuracyStats calculateOverallStats() const;
    AccuracyStats calculateStatsByDateRange(
        const std::string& startDate,
        const std::string& endDate
    ) const;
    AccuracyStats calculateStatsByStock(const std::string& tsCode) const;
    AccuracyStats calculateStatsByStrategy(const std::string& strategyName) const;
    
    std::vector<FailurePattern> identifyFailurePatterns() const;
    
    std::map<std::string, double> analyzeIndicatorCorrelation() const;
    
    bool shouldTriggerOptimization() const;
    
    std::string generateOptimizationReport() const;

private:
    const PredictionHistory* history_;
    
    static constexpr double OPTIMIZATION_THRESHOLD = 0.70;
    static constexpr int MIN_PREDICTIONS_FOR_ANALYSIS = 10;
    
    double calculateDirectionAccuracy(const std::vector<PredictionRecord>& records) const;
    std::map<std::string, double> extractCommonPatterns(
        const std::vector<PredictionRecord>& failures
    ) const;
};

} // namespace core

#endif // CORE_ACCURACY_ANALYZER_H