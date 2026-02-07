#ifndef CORE_ANALYSIS_RESULT_H
#define CORE_ANALYSIS_RESULT_H

#include <string>

namespace core {

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
    
    AnalysisResult() = default;
    
    AnalysisResult(
        const std::string& tsCode,
        const std::string& strategyName,
        const std::string& tradeDate,
        const std::string& label
    ) : ts_code(tsCode)
      , strategy_name(strategyName)
      , trade_date(tradeDate)
      , label(label)
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

