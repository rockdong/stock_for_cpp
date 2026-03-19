#include "AccuracyAnalyzer.h"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <sstream>
#include <iomanip>

namespace core {

AccuracyAnalyzer::AccuracyAnalyzer() : history_(nullptr) {}

AccuracyAnalyzer::AccuracyAnalyzer(const PredictionHistory& history) 
    : history_(&history) {}

void AccuracyAnalyzer::setHistory(const PredictionHistory& history) {
    history_ = &history;
}

AccuracyStats AccuracyAnalyzer::calculateOverallStats() const {
    AccuracyStats stats;
    if (!history_ || history_->size() == 0) return stats;
    
    const auto& predictions = history_->getAllPredictions();
    stats.total_predictions = static_cast<int>(predictions.size());
    
    double sumPredictedGain = 0.0;
    double sumActualGain = 0.0;
    double sumAccuracyScore = 0.0;
    int verifiedCount = 0;
    
    for (const auto& p : predictions) {
        if (p.verified) {
            verifiedCount++;
            if (p.accurate) stats.accurate_predictions++;
            
            sumPredictedGain += p.predicted_gain;
            sumActualGain += p.actual_gain;
            sumAccuracyScore += p.accuracy_score;
            
            if (p.predicted_direction > 0) {
                stats.up_predictions++;
                if (p.accurate) stats.up_accurate++;
            } else if (p.predicted_direction < 0) {
                stats.down_predictions++;
                if (p.accurate) stats.down_accurate++;
            }
        }
    }
    
    if (verifiedCount > 0) {
        stats.accuracy_rate = static_cast<double>(stats.accurate_predictions) / verifiedCount;
        stats.avg_predicted_gain = sumPredictedGain / verifiedCount;
        stats.avg_actual_gain = sumActualGain / verifiedCount;
        stats.avg_accuracy_score = sumAccuracyScore / verifiedCount;
    }
    
    return stats;
}

AccuracyStats AccuracyAnalyzer::calculateStatsByDateRange(
    const std::string& startDate,
    const std::string& endDate
) const {
    AccuracyStats stats;
    if (!history_) return stats;
    
    auto predictions = history_->getPredictionsByDateRange(startDate, endDate);
    
    stats.total_predictions = static_cast<int>(predictions.size());
    
    int verifiedCount = 0;
    for (const auto& p : predictions) {
        if (p.verified) {
            verifiedCount++;
            if (p.accurate) stats.accurate_predictions++;
        }
    }
    
    if (verifiedCount > 0) {
        stats.accuracy_rate = static_cast<double>(stats.accurate_predictions) / verifiedCount;
    }
    
    return stats;
}

AccuracyStats AccuracyAnalyzer::calculateStatsByStock(const std::string& tsCode) const {
    AccuracyStats stats;
    if (!history_) return stats;
    
    auto predictions = history_->getPredictionsByStock(tsCode);
    
    stats.total_predictions = static_cast<int>(predictions.size());
    
    int verifiedCount = 0;
    for (const auto& p : predictions) {
        if (p.verified) {
            verifiedCount++;
            if (p.accurate) stats.accurate_predictions++;
        }
    }
    
    if (verifiedCount > 0) {
        stats.accuracy_rate = static_cast<double>(stats.accurate_predictions) / verifiedCount;
    }
    
    return stats;
}

AccuracyStats AccuracyAnalyzer::calculateStatsByStrategy(const std::string& strategyName) const {
    AccuracyStats stats;
    if (!history_) return stats;
    
    auto predictions = history_->getPredictionsByStrategy(strategyName);
    
    stats.total_predictions = static_cast<int>(predictions.size());
    
    int verifiedCount = 0;
    for (const auto& p : predictions) {
        if (p.verified) {
            verifiedCount++;
            if (p.accurate) stats.accurate_predictions++;
        }
    }
    
    if (verifiedCount > 0) {
        stats.accuracy_rate = static_cast<double>(stats.accurate_predictions) / verifiedCount;
    }
    
    return stats;
}

std::vector<FailurePattern> AccuracyAnalyzer::identifyFailurePatterns() const {
    std::vector<FailurePattern> patterns;
    if (!history_) return patterns;
    
    const auto& predictions = history_->getAllPredictions();
    
    std::vector<PredictionRecord> failures;
    for (const auto& p : predictions) {
        if (p.verified && !p.accurate) {
            failures.push_back(p);
        }
    }
    
    if (failures.empty()) return patterns;
    
    FailurePattern highVolatilityPattern;
    highVolatilityPattern.pattern_name = "high_volatility";
    highVolatilityPattern.description = "高波动市场环境下预测失败";
    highVolatilityPattern.occurrence_count = 0;
    
    FailurePattern trendReversalPattern;
    trendReversalPattern.pattern_name = "trend_reversal";
    trendReversalPattern.description = "趋势反转时预测失败";
    trendReversalPattern.occurrence_count = 0;
    
    FailurePattern lowVolumePattern;
    lowVolumePattern.pattern_name = "low_volume";
    lowVolumePattern.description = "低成交量时预测失败";
    lowVolumePattern.occurrence_count = 0;
    
    for (const auto& f : failures) {
        auto volIt = f.indicators.find("volume_ratio");
        if (volIt != f.indicators.end() && volIt->second < 1.0) {
            lowVolumePattern.occurrence_count++;
        }
        
        auto macdIt = f.indicators.find("macd_histogram");
        if (macdIt != f.indicators.end() && std::abs(macdIt->second) < 0.01) {
            trendReversalPattern.occurrence_count++;
        }
    }
    
    if (lowVolumePattern.occurrence_count > 0) {
        lowVolumePattern.failure_rate = 
            static_cast<double>(lowVolumePattern.occurrence_count) / failures.size();
        patterns.push_back(lowVolumePattern);
    }
    
    if (trendReversalPattern.occurrence_count > 0) {
        trendReversalPattern.failure_rate = 
            static_cast<double>(trendReversalPattern.occurrence_count) / failures.size();
        patterns.push_back(trendReversalPattern);
    }
    
    return patterns;
}

std::map<std::string, double> AccuracyAnalyzer::analyzeIndicatorCorrelation() const {
    std::map<std::string, double> correlations;
    if (!history_) return correlations;
    
    const auto& predictions = history_->getAllPredictions();
    
    for (const auto& p : predictions) {
        if (!p.verified) continue;
        
        for (const auto& [key, value] : p.indicators) {
            std::string corrKey = key + "_correlation";
            if (correlations.find(corrKey) == correlations.end()) {
                correlations[corrKey] = 0.0;
            }
            
            if (p.accurate) {
                correlations[corrKey] += 1.0;
            }
        }
    }
    
    int verifiedCount = 0;
    for (const auto& p : predictions) {
        if (p.verified) verifiedCount++;
    }
    
    if (verifiedCount > 0) {
        for (auto& [key, value] : correlations) {
            value /= verifiedCount;
        }
    }
    
    return correlations;
}

bool AccuracyAnalyzer::shouldTriggerOptimization() const {
    if (!history_ || history_->size() < MIN_PREDICTIONS_FOR_ANALYSIS) {
        return false;
    }
    
    auto stats = calculateOverallStats();
    
    if (stats.accuracy_rate < OPTIMIZATION_THRESHOLD) {
        return true;
    }
    
    auto predictions = history_->getAllPredictions();
    int recentFailures = 0;
    int recentCount = 0;
    
    for (int i = static_cast<int>(predictions.size()) - 1; 
         i >= 0 && recentCount < 5; --i) {
        if (predictions[i].verified) {
            recentCount++;
            if (!predictions[i].accurate) {
                recentFailures++;
            }
        }
    }
    
    if (recentFailures >= 3) {
        return true;
    }
    
    return false;
}

std::string AccuracyAnalyzer::generateOptimizationReport() const {
    std::ostringstream oss;
    
    auto stats = calculateOverallStats();
    
    oss << "## 📊 预测准确率分析报告\n\n";
    oss << "### 整体统计\n";
    oss << "- 总预测次数: " << stats.total_predictions << "\n";
    oss << "- 准确预测次数: " << stats.accurate_predictions << "\n";
    oss << "- 当前准确率: " << std::fixed << std::setprecision(2) 
        << (stats.accuracy_rate * 100) << "%\n";
    oss << "- 目标准确率: 100%\n\n";
    
    if (shouldTriggerOptimization()) {
        oss << "### ⚠️ 需要优化\n";
        oss << "当前准确率低于阈值 (" << (OPTIMIZATION_THRESHOLD * 100) << "%)，建议进行参数优化。\n\n";
    }
    
    auto patterns = identifyFailurePatterns();
    if (!patterns.empty()) {
        oss << "### 失败模式分析\n";
        for (const auto& p : patterns) {
            oss << "- **" << p.pattern_name << "**: " << p.description 
                << " (出现 " << p.occurrence_count << " 次)\n";
        }
    }
    
    return oss.str();
}

double AccuracyAnalyzer::calculateDirectionAccuracy(
    const std::vector<PredictionRecord>& records
) const {
    if (records.empty()) return 0.0;
    
    int correct = 0;
    int total = 0;
    
    for (const auto& r : records) {
        if (r.verified) {
            total++;
            if (r.predicted_direction * r.actual_direction > 0) {
                correct++;
            }
        }
    }
    
    return total > 0 ? static_cast<double>(correct) / total : 0.0;
}

std::map<std::string, double> AccuracyAnalyzer::extractCommonPatterns(
    const std::vector<PredictionRecord>& failures
) const {
    std::map<std::string, double> patterns;
    
    if (failures.empty()) return patterns;
    
    std::map<std::string, double> sumValues;
    std::map<std::string, int> counts;
    
    for (const auto& f : failures) {
        for (const auto& [key, value] : f.indicators) {
            sumValues[key] += value;
            counts[key]++;
        }
    }
    
    for (const auto& [key, sum] : sumValues) {
        if (counts[key] > 0) {
            patterns[key + "_avg"] = sum / counts[key];
        }
    }
    
    return patterns;
}

} // namespace core