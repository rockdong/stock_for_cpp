#include "ParameterOptimizer.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include "../thirdparty/json/include/nlohmann/json.hpp"

namespace core {

using json = nlohmann::json;

ParameterOptimizer::ParameterOptimizer() {}

void ParameterOptimizer::setHistory(const PredictionHistory& history) {
    history_ = &history;
}

void ParameterOptimizer::setAnalyzer(const AccuracyAnalyzer& analyzer) {
    analyzer_ = &analyzer;
}

std::string ParameterOptimizer::getCurrentDate() const {
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y%m%d");
    return oss.str();
}

std::string ParameterOptimizer::generateRequestId() const {
    std::ostringstream oss;
    oss << "REQ-" << getCurrentDate() << "-" << std::setw(3) << std::setfill('0') 
        << (rand() % 1000);
    return oss.str();
}

double ParameterOptimizer::optimizeEmaDiffThreshold() const {
    if (!history_ || history_->size() < 5) {
        return EMA_DIFF_THRESHOLD_DEFAULT;
    }
    
    const auto& predictions = history_->getAllPredictions();
    
    double successSum = 0.0;
    int successCount = 0;
    double failureSum = 0.0;
    int failureCount = 0;
    
    for (const auto& p : predictions) {
        if (!p.verified) continue;
        
        auto diffIt = p.indicators.find("ema_diff_ratio");
        if (diffIt == p.indicators.end()) continue;
        
        if (p.accurate) {
            successSum += diffIt->second;
            successCount++;
        } else {
            failureSum += diffIt->second;
            failureCount++;
        }
    }
    
    if (successCount == 0) return EMA_DIFF_THRESHOLD_DEFAULT;
    
    double avgSuccessDiff = successSum / successCount;
    
    if (failureCount > 0) {
        double avgFailureDiff = failureSum / failureCount;
        return (avgSuccessDiff + avgFailureDiff) / 2;
    }
    
    return avgSuccessDiff * 0.8;
}

double ParameterOptimizer::optimizeVolumeRatio() const {
    if (!history_ || history_->size() < 5) {
        return VOLUME_RATIO_DEFAULT;
    }
    
    const auto& predictions = history_->getAllPredictions();
    
    double successSum = 0.0;
    int successCount = 0;
    
    for (const auto& p : predictions) {
        if (!p.verified || !p.accurate) continue;
        
        auto volIt = p.indicators.find("volume_ratio");
        if (volIt == p.indicators.end()) continue;
        
        successSum += volIt->second;
        successCount++;
    }
    
    if (successCount == 0) return VOLUME_RATIO_DEFAULT;
    
    return successSum / successCount;
}

std::map<std::string, double> ParameterOptimizer::optimizeWeights() const {
    std::map<std::string, double> weights;
    
    weights["ema_cross_weight"] = 0.4;
    weights["macd_weight"] = 0.3;
    weights["volume_weight"] = 0.2;
    weights["rsi_weight"] = 0.1;
    
    if (!analyzer_) return weights;
    
    auto correlations = analyzer_->analyzeIndicatorCorrelation();
    
    double totalCorr = 0.0;
    for (const auto& [key, value] : correlations) {
        totalCorr += value;
    }
    
    if (totalCorr > 0) {
        auto updateWeight = [&](const std::string& key, const std::string& corrKey) {
            auto it = correlations.find(corrKey);
            if (it != correlations.end()) {
                weights[key] = it->second / totalCorr;
            }
        };
        
        updateWeight("ema_cross_weight", "ema_diff_ratio_correlation");
        updateWeight("macd_weight", "macd_histogram_correlation");
        updateWeight("volume_weight", "volume_ratio_correlation");
        updateWeight("rsi_weight", "rsi_correlation");
    }
    
    return weights;
}

std::vector<ParameterAdjustment> ParameterOptimizer::suggestParameterAdjustments() const {
    std::vector<ParameterAdjustment> adjustments;
    
    if (!analyzer_) return adjustments;
    
    auto stats = analyzer_->calculateOverallStats();
    
    if (stats.accuracy_rate < 0.8) {
        ParameterAdjustment emaAdj;
        emaAdj.param_name = "ema_diff_threshold";
        emaAdj.current_value = EMA_DIFF_THRESHOLD_DEFAULT;
        emaAdj.suggested_value = optimizeEmaDiffThreshold();
        emaAdj.adjustment_reason = 0.1;
        emaAdj.reason_description = "收紧 EMA 差值阈值以减少 false positive";
        adjustments.push_back(emaAdj);
    }
    
    if (stats.accuracy_rate < 0.7) {
        ParameterAdjustment volAdj;
        volAdj.param_name = "volume_ratio_min";
        volAdj.current_value = VOLUME_RATIO_DEFAULT;
        volAdj.suggested_value = optimizeVolumeRatio();
        volAdj.adjustment_reason = 0.15;
        volAdj.reason_description = "提高成交量放大要求";
        adjustments.push_back(volAdj);
    }
    
    return adjustments;
}

OptimizationIteration ParameterOptimizer::createOptimizationIteration() const {
    OptimizationIteration iteration;
    
    iteration.iteration_id = "ITER-" + getCurrentDate() + "-001";
    iteration.date = getCurrentDate();
    
    if (analyzer_) {
        auto stats = analyzer_->calculateOverallStats();
        iteration.accuracy_before = stats.accuracy_rate;
    }
    
    iteration.accuracy_target = TARGET_ACCURACY;
    iteration.adjustments = suggestParameterAdjustments();
    
    if (analyzer_ && analyzer_->shouldTriggerOptimization()) {
        iteration.trigger_reason = "准确率低于阈值，需要优化";
    } else {
        iteration.trigger_reason = "定期优化";
    }
    
    iteration.assigned_to = "backend-dev";
    iteration.applied = false;
    
    return iteration;
}

AlgorithmUpdateRequest ParameterOptimizer::generateUpdateRequest() const {
    AlgorithmUpdateRequest request;
    
    request.request_id = generateRequestId();
    request.date = getCurrentDate();
    
    if (analyzer_) {
        auto stats = analyzer_->calculateOverallStats();
        request.current_accuracy = stats.accuracy_rate;
    }
    
    request.target_accuracy = TARGET_ACCURACY;
    request.background = "通过历史预测结果分析，需要优化算法参数以提高准确率";
    
    request.parameter_changes = suggestParameterAdjustments();
    
    auto stats = analyzer_->calculateOverallStats();
    if (stats.accuracy_rate < 0.6) {
        request.new_feature_requests.push_back("添加更多技术指标组合");
        request.new_feature_requests.push_back("增加市场情绪分析");
    }
    
    std::ostringstream oss;
    oss << "预计准确率提升至 " << std::fixed << std::setprecision(0)
        << (request.current_accuracy + 0.15) * 100 << "%";
    request.expected_improvement = oss.str();
    
    if (stats.accuracy_rate < 0.7) {
        request.priority = "high";
    } else if (stats.accuracy_rate < 0.8) {
        request.priority = "medium";
    } else {
        request.priority = "low";
    }
    
    return request;
}

std::string ParameterOptimizer::generateBackendDevRequest() const {
    auto request = generateUpdateRequest();
    
    std::ostringstream oss;
    
    oss << "## 🔧 预测算法参数调整请求\n\n";
    
    oss << "### 背景\n";
    oss << "- 当前准确率: " << std::fixed << std::setprecision(1) 
        << (request.current_accuracy * 100) << "%\n";
    oss << "- 目标准确率: " << (request.target_accuracy * 100) << "%\n";
    oss << "- " << request.background << "\n\n";
    
    oss << "### 需要调整的参数\n";
    oss << "| 参数 | 当前值 | 建议值 | 原因 |\n";
    oss << "|------|--------|--------|------|\n";
    
    for (const auto& adj : request.parameter_changes) {
        oss << "| " << adj.param_name 
            << " | " << adj.current_value 
            << " | " << adj.suggested_value 
            << " | " << adj.reason_description << " |\n";
    }
    
    if (!request.new_feature_requests.empty()) {
        oss << "\n### 新增功能需求\n";
        for (const auto& req : request.new_feature_requests) {
            oss << "- [ ] " << req << "\n";
        }
    }
    
    oss << "\n### 预期效果\n";
    oss << request.expected_improvement << "\n\n";
    
    oss << "### 优先级\n";
    oss << (request.priority == "high" ? "🔴 高" : 
            request.priority == "medium" ? "🟡 中" : "🟢 低") << "\n";
    
    return oss.str();
}

bool ParameterOptimizer::saveIterationRecord(const OptimizationIteration& iteration) const {
    json j;
    j["iteration_id"] = iteration.iteration_id;
    j["date"] = iteration.date;
    j["accuracy_before"] = iteration.accuracy_before;
    j["accuracy_target"] = iteration.accuracy_target;
    j["trigger_reason"] = iteration.trigger_reason;
    j["assigned_to"] = iteration.assigned_to;
    j["applied"] = iteration.applied;
    j["accuracy_after"] = iteration.accuracy_after;
    
    json adjustmentsJson = json::array();
    for (const auto& adj : iteration.adjustments) {
        json adjJson;
        adjJson["param_name"] = adj.param_name;
        adjJson["current_value"] = adj.current_value;
        adjJson["suggested_value"] = adj.suggested_value;
        adjJson["reason_description"] = adj.reason_description;
        adjustmentsJson.push_back(adjJson);
    }
    j["adjustments"] = adjustmentsJson;
    
    std::ofstream file(iterationFilePath_, std::ios::app);
    if (!file.is_open()) {
        return false;
    }
    
    file << j.dump() << "\n";
    return true;
}

std::vector<OptimizationIteration> ParameterOptimizer::loadIterationHistory() const {
    std::vector<OptimizationIteration> iterations;
    
    std::ifstream file(iterationFilePath_);
    if (!file.is_open()) {
        return iterations;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        try {
            json j = json::parse(line);
            
            OptimizationIteration iter;
            iter.iteration_id = j.value("iteration_id", "");
            iter.date = j.value("date", "");
            iter.accuracy_before = j.value("accuracy_before", 0.0);
            iter.accuracy_target = j.value("accuracy_target", 1.0);
            iter.trigger_reason = j.value("trigger_reason", "");
            iter.assigned_to = j.value("assigned_to", "");
            iter.applied = j.value("applied", false);
            iter.accuracy_after = j.value("accuracy_after", 0.0);
            
            if (j.contains("adjustments")) {
                for (const auto& adjJson : j["adjustments"]) {
                    ParameterAdjustment adj;
                    adj.param_name = adjJson.value("param_name", "");
                    adj.current_value = adjJson.value("current_value", 0.0);
                    adj.suggested_value = adjJson.value("suggested_value", 0.0);
                    adj.reason_description = adjJson.value("reason_description", "");
                    iter.adjustments.push_back(adj);
                }
            }
            
            iterations.push_back(iter);
        } catch (const std::exception& e) {
            continue;
        }
    }
    
    return iterations;
}

} // namespace core