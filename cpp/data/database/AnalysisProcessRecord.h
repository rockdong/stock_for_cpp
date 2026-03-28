#ifndef DATA_ANALYSIS_PROCESS_RECORD_H
#define DATA_ANALYSIS_PROCESS_RECORD_H

#include <string>
#include <vector>
#include <optional>
#include <nlohmann/json.hpp>

namespace data {

struct ProcessDataPoint {
    std::string time;
    double open = 0.0;
    double high = 0.0;
    double low = 0.0;
    double close = 0.0;
    int64_t volume = 0;
    
    std::optional<double> ema17;
    std::optional<double> ema25;
    std::optional<double> macd;
    std::optional<double> macd_signal;
    std::optional<double> macd_hist;
    std::optional<double> rsi;
    std::optional<double> boll_upper;
    std::optional<double> boll_lower;
    
    ProcessDataPoint() = default;
    
    nlohmann::json toJson() const;
    static ProcessDataPoint fromJson(const nlohmann::json& j);
};

struct StrategyFreqData {
    std::string freq;
    std::string signal;
    std::vector<ProcessDataPoint> candles;
    
    StrategyFreqData() = default;
    
    nlohmann::json toJson() const;
    static StrategyFreqData fromJson(const nlohmann::json& j);
};

struct StrategyData {
    std::string name;
    std::vector<StrategyFreqData> freqs;
    
    StrategyData() = default;
    
    nlohmann::json toJson() const;
    static StrategyData fromJson(const nlohmann::json& j);
};

struct StockProcessRecord {
    int id = 0;
    std::string ts_code;
    std::string stock_name;
    std::string trade_date;
    std::vector<StrategyData> strategies;
    std::string created_at;
    std::string expires_at;
    
    StockProcessRecord() = default;
    
    std::string dataToJson() const;
    void dataFromJson(const std::string& jsonStr);
    std::string toString() const;
};

void to_json(nlohmann::json& j, const ProcessDataPoint& p);
void from_json(const nlohmann::json& j, ProcessDataPoint& p);

void to_json(nlohmann::json& j, const StrategyFreqData& f);
void from_json(const nlohmann::json& j, StrategyFreqData& f);

void to_json(nlohmann::json& j, const StrategyData& s);
void from_json(const nlohmann::json& j, StrategyData& s);

void to_json(nlohmann::json& j, const StockProcessRecord& r);
void from_json(const nlohmann::json& j, StockProcessRecord& r);

} // namespace data

#endif // DATA_ANALYSIS_PROCESS_RECORD_H