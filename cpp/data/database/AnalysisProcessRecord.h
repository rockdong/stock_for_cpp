#ifndef DATA_ANALYSIS_PROCESS_RECORD_H
#define DATA_ANALYSIS_PROCESS_RECORD_H

#include <string>
#include <vector>
#include <optional>
#include <nlohmann/json.hpp>

namespace data {

/**
 * @brief 单个周期的过程数据点
 * 
 * 包含 K 线数据和技术指标
 */
struct ProcessDataPoint {
    std::string time;                           // 日期 (YYYY-MM-DD)
    double open = 0.0;                          // 开盘价
    double high = 0.0;                          // 最高价
    double low = 0.0;                           // 最低价
    double close = 0.0;                         // 收盘价
    int64_t volume = 0;                         // 成交量
    
    // 可选技术指标
    std::optional<double> ema17;                // EMA17
    std::optional<double> ema25;                // EMA25
    std::optional<double> macd;                 // MACD
    std::optional<double> rsi;                  // RSI
    
    ProcessDataPoint() = default;
    
    /**
     * @brief 转换为 JSON
     */
    nlohmann::json toJson() const;
    
    /**
     * @brief 从 JSON 解析
     */
    static ProcessDataPoint fromJson(const nlohmann::json& j);
};

/**
 * @brief 分析过程记录
 * 
 * 存储策略分析时的详细过程数据
 */
struct AnalysisProcessRecord {
    int id = 0;                                 // 数据库主键
    std::string ts_code;                        // 股票代码 (如 000001.SZ)
    std::string stock_name;                     // 股票名称
    std::string strategy_name;                  // 策略名称
    std::string trade_date;                     // 交易日期 (YYYYMMDD)
    std::string freq = "d";                     // 周期 (d=日线, w=周线, m=月线)
    std::string signal = "NONE";                // 信号类型 (BUY/SELL/HOLD/NONE)
    std::vector<ProcessDataPoint> data;         // 过程数据 (最多10条)
    std::string created_at;                     // 创建时间
    std::string expires_at;                     // 过期时间
    
    AnalysisProcessRecord() = default;
    
    /**
     * @brief 将 data 序列化为 JSON 字符串
     */
    std::string dataToJson() const;
    
    /**
     * @brief 从 JSON 字符串解析 data
     */
    void dataFromJson(const std::string& jsonStr);
    
    /**
     * @brief 转换为字符串表示
     */
    std::string toString() const;
};

// JSON 序列化函数 (用于 nlohmann/json)
void to_json(nlohmann::json& j, const ProcessDataPoint& p);
void from_json(const nlohmann::json& j, ProcessDataPoint& p);

void to_json(nlohmann::json& j, const AnalysisProcessRecord& r);
void from_json(const nlohmann::json& j, AnalysisProcessRecord& r);

} // namespace data

#endif // DATA_ANALYSIS_PROCESS_RECORD_H