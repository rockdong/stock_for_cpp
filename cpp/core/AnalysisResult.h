#ifndef CORE_ANALYSIS_RESULT_H
#define CORE_ANALYSIS_RESULT_H

#include <string>

namespace core {

/**
 * @brief 信号强度枚举
 * 
 * 用于表示交易信号的质量等级
 */
enum class SignalStrength {
    WEAK,       // 弱信号：置信度 < 40
    MEDIUM,     // 中等信号：置信度 40-69
    STRONG      // 强信号：置信度 >= 70
};

/**
 * @brief 将信号强度转换为字符串
 * @param strength 信号强度枚举值
 * @return 字符串表示
 */
inline std::string strengthToString(SignalStrength strength) {
    switch (strength) {
        case SignalStrength::WEAK: return "WEAK";
        case SignalStrength::MEDIUM: return "MEDIUM";
        case SignalStrength::STRONG: return "STRONG";
        default: return "UNKNOWN";
    }
}

/**
 * @brief 分析结果（核心数据结构）
 * 
 * 记录策略分析的结果，用于：
 * - 策略分析输出
 * - 数据库存储
 * - 历史记录查询
 * 
 * 设计原则：
 * - 简洁明了：只包含必要字段
 * - 业务字段：不包含数据库特定字段（如 id, created_at）
 */
struct AnalysisResult {
    std::string ts_code;        // 股票代码（如 000001.SZ）
    std::string strategy_name;  // 策略名称
    std::string trade_date;     // 分析日期（格式：YYYYMMDD）
    std::string label;          // 标签（如：买入、卖出、持有等）
    std::string opt;            // 操作类型（buy/sell/hold）
    std::string freq;           // 频率（d=日线, w=周线, m=月线）
    
    // 新增字段：信号强度评估
    SignalStrength strength = SignalStrength::MEDIUM;  // 信号强度
    double confidence = 50.0;                          // 置信度（0-100）
    std::string risk_warning;                          // 风险提示
    
    AnalysisResult() = default;
    
    AnalysisResult(
        const std::string& tsCode,
        const std::string& strategyName,
        const std::string& tradeDate,
        const std::string& label,
        const std::string& opt = "",
        const std::string& freq = "d"
    ) : ts_code(tsCode)
      , strategy_name(strategyName)
      , trade_date(tradeDate)
      , label(label)
      , opt(opt)
      , freq(freq)
      , strength(SignalStrength::MEDIUM)
      , confidence(50.0)
      , risk_warning("")
    {}
    
    /**
     * @brief 转换为字符串表示
     */
    std::string toString() const;
};

/**
 * @brief 分析结果数据库实体（扩展核心结构）
 * 
 * 在核心 AnalysisResult 结构基础上，添加数据库特定字段
 * 用于数据持久化
 */
struct AnalysisResultEntity {
    int id = 0;                     // 数据库主键
    AnalysisResult result;          // 核心分析结果
    std::string created_at;         // 创建时间
    std::string updated_at;         // 更新时间
};

} // namespace core

#endif // CORE_ANALYSIS_RESULT_H

