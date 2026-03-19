#ifndef CORE_PREDICTION_HISTORY_H
#define CORE_PREDICTION_HISTORY_H

#include <string>
#include <vector>
#include <map>
#include <optional>
#include <ctime>
#include "AnalysisResult.h"

namespace core {

/**
 * @brief 预测结果记录
 * 
 * 记录每次预测的详细信息，包括预测时的指标值和实际结果
 */
struct PredictionRecord {
    std::string signal_id;              // 信号ID（如 BS-20240115-001）
    std::string ts_code;                // 股票代码
    std::string trade_date;             // 预测日期
    std::string strategy_name;          // 策略名称
    
    // 预测信息
    double predicted_direction;         // 预测方向（1=上涨, -1=下跌, 0=震荡）
    double predicted_gain;              // 预测涨幅
    double confidence;                  // 置信度（0-1）
    
    // 预测时的指标值
    std::map<std::string, double> indicators;  // 如 ema17, ema25, diff, macd 等
    
    // 预测时的参数
    std::map<std::string, double> parameters;  // 如 fast_period, slow_period 等
    
    // 实际结果（收盘后填充）
    bool verified = false;              // 是否已验证
    double actual_gain = 0.0;           // 实际涨幅
    double actual_direction = 0.0;      // 实际方向
    bool accurate = false;              // 预测是否准确
    double accuracy_score = 0.0;        // 准确度评分（0-1）
    
    std::string created_at;             // 创建时间
    std::string verified_at;            // 验证时间
    
    PredictionRecord() = default;
};

/**
 * @brief 预测历史管理器
 * 
 * 负责预测记录的存储、查询和分析
 * 支持从 JSON 文件读取和写入
 */
class PredictionHistory {
public:
    PredictionHistory();
    explicit PredictionHistory(const std::string& dataPath);
    
    /**
     * @brief 添加预测记录
     * @param record 预测记录
     * @return 信号ID
     */
    std::string addPrediction(const PredictionRecord& record);
    
    /**
     * @brief 验证预测结果
     * @param signalId 信号ID
     * @param actualGain 实际涨幅
     * @param actualDirection 实际方向
     */
    bool verifyPrediction(
        const std::string& signalId,
        double actualGain,
        double actualDirection
    );
    
    /**
     * @brief 获取未验证的预测
     */
    std::vector<PredictionRecord> getUnverifiedPredictions() const;
    
    /**
     * @brief 获取指定日期范围内的预测
     */
    std::vector<PredictionRecord> getPredictionsByDateRange(
        const std::string& startDate,
        const std::string& endDate
    ) const;
    
    /**
     * @brief 获取指定股票的预测历史
     */
    std::vector<PredictionRecord> getPredictionsByStock(const std::string& tsCode) const;
    
    /**
     * @brief 获取指定策略的预测历史
     */
    std::vector<PredictionRecord> getPredictionsByStrategy(const std::string& strategyName) const;
    
    /**
     * @brief 保存到 JSON 文件
     */
    bool saveToJson(const std::string& filepath = "") const;
    
    /**
     * @brief 从 JSON 文件加载
     */
    bool loadFromJson(const std::string& filepath = "");
    
    /**
     * @brief 获取所有预测记录
     */
    const std::vector<PredictionRecord>& getAllPredictions() const { return predictions_; }
    
    /**
     * @brief 获取预测记录数量
     */
    size_t size() const { return predictions_.size(); }
    
    /**
     * @brief 清空历史记录
     */
    void clear() { predictions_.clear(); }

private:
    std::string dataPath_;
    std::vector<PredictionRecord> predictions_;
    int nextSignalId_ = 1;
    
    std::string generateSignalId(const std::string& date);
    std::string getCurrentTimestamp() const;
};

} // namespace core

#endif // CORE_PREDICTION_HISTORY_H