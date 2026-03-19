#ifndef CORE_PARAMETER_OPTIMIZER_H
#define CORE_PARAMETER_OPTIMIZER_H

#include <string>
#include <vector>
#include <map>
#include "PredictionHistory.h"
#include "AccuracyAnalyzer.h"

namespace core {

struct ParameterAdjustment {
    std::string param_name;         // 参数名称
    double current_value;           // 当前值
    double suggested_value;         // 建议值
    double adjustment_reason;       // 调整原因（效果提升预期）
    std::string reason_description; // 原因描述
};

struct OptimizationIteration {
    std::string iteration_id;                       // 迭代ID
    std::string date;                               // 迭代日期
    double accuracy_before;                         // 优化前准确率
    double accuracy_target;                         // 目标准确率
    std::vector<ParameterAdjustment> adjustments;   // 参数调整列表
    std::string trigger_reason;                     // 触发原因
    std::string assigned_to;                        // 分配给谁（backend-dev）
    bool applied = false;                           // 是否已应用
    double accuracy_after = 0.0;                    // 应用后准确率
};

struct AlgorithmUpdateRequest {
    std::string request_id;
    std::string date;
    double current_accuracy;
    double target_accuracy;
    std::string background;
    std::vector<ParameterAdjustment> parameter_changes;
    std::vector<std::string> new_feature_requests;
    std::string expected_improvement;
    std::string priority;  // "high", "medium", "low"
};

class ParameterOptimizer {
public:
    ParameterOptimizer();
    
    void setHistory(const PredictionHistory& history);
    void setAnalyzer(const AccuracyAnalyzer& analyzer);
    
    std::vector<ParameterAdjustment> suggestParameterAdjustments() const;
    
    OptimizationIteration createOptimizationIteration() const;
    
    AlgorithmUpdateRequest generateUpdateRequest() const;
    
    std::string generateBackendDevRequest() const;
    
    bool saveIterationRecord(const OptimizationIteration& iteration) const;
    std::vector<OptimizationIteration> loadIterationHistory() const;

private:
    const PredictionHistory* history_ = nullptr;
    const AccuracyAnalyzer* analyzer_ = nullptr;
    
    std::string iterationFilePath_ = "data/optimization_iterations.json";
    
    static constexpr double TARGET_ACCURACY = 1.0;  // 目标：100% 准确率
    static constexpr double EMA_DIFF_THRESHOLD_DEFAULT = 0.005;
    static constexpr double VOLUME_RATIO_DEFAULT = 1.2;
    
    double optimizeEmaDiffThreshold() const;
    double optimizeVolumeRatio() const;
    std::map<std::string, double> optimizeWeights() const;
    
    std::string generateRequestId() const;
    std::string getCurrentDate() const;
};

} // namespace core

#endif // CORE_PARAMETER_OPTIMIZER_H