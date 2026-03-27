#include "AnalysisProcessRecord.h"
#include <sstream>

namespace data {

// ProcessDataPoint implementation

nlohmann::json ProcessDataPoint::toJson() const {
    nlohmann::json j;
    j["time"] = time;
    j["open"] = open;
    j["high"] = high;
    j["low"] = low;
    j["close"] = close;
    j["volume"] = volume;
    
    if (ema17.has_value()) j["ema17"] = *ema17;
    if (ema25.has_value()) j["ema25"] = *ema25;
    if (macd.has_value()) j["macd"] = *macd;
    if (rsi.has_value()) j["rsi"] = *rsi;
    
    return j;
}

ProcessDataPoint ProcessDataPoint::fromJson(const nlohmann::json& j) {
    ProcessDataPoint p;
    p.time = j.value("time", "");
    p.open = j.value("open", 0.0);
    p.high = j.value("high", 0.0);
    p.low = j.value("low", 0.0);
    p.close = j.value("close", 0.0);
    p.volume = j.value("volume", 0);
    
    if (j.contains("ema17") && !j["ema17"].is_null()) p.ema17 = j["ema17"].get<double>();
    if (j.contains("ema25") && !j["ema25"].is_null()) p.ema25 = j["ema25"].get<double>();
    if (j.contains("macd") && !j["macd"].is_null()) p.macd = j["macd"].get<double>();
    if (j.contains("rsi") && !j["rsi"].is_null()) p.rsi = j["rsi"].get<double>();
    
    return p;
}

// AnalysisProcessRecord implementation

std::string AnalysisProcessRecord::dataToJson() const {
    nlohmann::json j = data;
    return j.dump();
}

void AnalysisProcessRecord::dataFromJson(const std::string& jsonStr) {
    auto j = nlohmann::json::parse(jsonStr);
    data = j.get<std::vector<ProcessDataPoint>>();
}

std::string AnalysisProcessRecord::toString() const {
    std::ostringstream oss;
    oss << "AnalysisProcessRecord{"
        << "ts_code=" << ts_code
        << ", strategy=" << strategy_name
        << ", date=" << trade_date
        << ", signal=" << signal
        << ", data_points=" << data.size()
        << "}";
    return oss.str();
}

// JSON serialization functions

void to_json(nlohmann::json& j, const ProcessDataPoint& p) {
    j = p.toJson();
}

void from_json(const nlohmann::json& j, ProcessDataPoint& p) {
    p = ProcessDataPoint::fromJson(j);
}

void to_json(nlohmann::json& j, const AnalysisProcessRecord& r) {
    j = nlohmann::json{
        {"id", r.id},
        {"ts_code", r.ts_code},
        {"stock_name", r.stock_name},
        {"strategy_name", r.strategy_name},
        {"trade_date", r.trade_date},
        {"freq", r.freq},
        {"signal", r.signal},
        {"data", r.data},
        {"created_at", r.created_at},
        {"expires_at", r.expires_at}
    };
}

void from_json(const nlohmann::json& j, AnalysisProcessRecord& r) {
    r.id = j.value("id", 0);
    r.ts_code = j.value("ts_code", "");
    r.stock_name = j.value("stock_name", "");
    r.strategy_name = j.value("strategy_name", "");
    r.trade_date = j.value("trade_date", "");
    r.freq = j.value("freq", "d");
    r.signal = j.value("signal", "NONE");
    r.created_at = j.value("created_at", "");
    r.expires_at = j.value("expires_at", "");
    
    if (j.contains("data") && j["data"].is_array()) {
        r.data = j["data"].get<std::vector<ProcessDataPoint>>();
    }
}

} // namespace data