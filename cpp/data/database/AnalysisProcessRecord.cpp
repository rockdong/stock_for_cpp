#include "AnalysisProcessRecord.h"
#include <sstream>

namespace data {

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
    if (macd_signal.has_value()) j["macd_signal"] = *macd_signal;
    if (macd_hist.has_value()) j["macd_hist"] = *macd_hist;
    if (rsi.has_value()) j["rsi"] = *rsi;
    if (boll_upper.has_value()) j["boll_upper"] = *boll_upper;
    if (boll_lower.has_value()) j["boll_lower"] = *boll_lower;
    
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
    if (j.contains("macd_signal") && !j["macd_signal"].is_null()) p.macd_signal = j["macd_signal"].get<double>();
    if (j.contains("macd_hist") && !j["macd_hist"].is_null()) p.macd_hist = j["macd_hist"].get<double>();
    if (j.contains("rsi") && !j["rsi"].is_null()) p.rsi = j["rsi"].get<double>();
    if (j.contains("boll_upper") && !j["boll_upper"].is_null()) p.boll_upper = j["boll_upper"].get<double>();
    if (j.contains("boll_lower") && !j["boll_lower"].is_null()) p.boll_lower = j["boll_lower"].get<double>();
    
    return p;
}

nlohmann::json StrategyFreqData::toJson() const {
    nlohmann::json j;
    j["freq"] = freq;
    j["signal"] = signal;
    j["candles"] = candles;
    return j;
}

StrategyFreqData StrategyFreqData::fromJson(const nlohmann::json& j) {
    StrategyFreqData f;
    f.freq = j.value("freq", "d");
    f.signal = j.value("signal", "NONE");
    if (j.contains("candles") && j["candles"].is_array()) {
        f.candles = j["candles"].get<std::vector<ProcessDataPoint>>();
    }
    return f;
}

nlohmann::json StrategyData::toJson() const {
    nlohmann::json j;
    j["name"] = name;
    j["freqs"] = freqs;
    return j;
}

StrategyData StrategyData::fromJson(const nlohmann::json& j) {
    StrategyData s;
    s.name = j.value("name", "");
    if (j.contains("freqs") && j["freqs"].is_array()) {
        s.freqs = j["freqs"].get<std::vector<StrategyFreqData>>();
    }
    return s;
}

std::string StockProcessRecord::dataToJson() const {
    nlohmann::json j;
    j["strategies"] = strategies;
    return j.dump();
}

void StockProcessRecord::dataFromJson(const std::string& jsonStr) {
    auto j = nlohmann::json::parse(jsonStr);
    if (j.contains("strategies") && j["strategies"].is_array()) {
        strategies = j["strategies"].get<std::vector<StrategyData>>();
    }
}

std::string StockProcessRecord::toString() const {
    std::ostringstream oss;
    oss << "StockProcessRecord{"
        << "ts_code=" << ts_code
        << ", stock_name=" << stock_name
        << ", trade_date=" << trade_date
        << ", strategies=" << strategies.size()
        << "}";
    return oss.str();
}

void to_json(nlohmann::json& j, const ProcessDataPoint& p) {
    j = p.toJson();
}

void from_json(const nlohmann::json& j, ProcessDataPoint& p) {
    p = ProcessDataPoint::fromJson(j);
}

void to_json(nlohmann::json& j, const StrategyFreqData& f) {
    j = f.toJson();
}

void from_json(const nlohmann::json& j, StrategyFreqData& f) {
    f = StrategyFreqData::fromJson(j);
}

void to_json(nlohmann::json& j, const StrategyData& s) {
    j = s.toJson();
}

void from_json(const nlohmann::json& j, StrategyData& s) {
    s = StrategyData::fromJson(j);
}

void to_json(nlohmann::json& j, const StockProcessRecord& r) {
    j = nlohmann::json{
        {"id", r.id},
        {"ts_code", r.ts_code},
        {"stock_name", r.stock_name},
        {"trade_date", r.trade_date},
        {"strategies", r.strategies},
        {"created_at", r.created_at},
        {"expires_at", r.expires_at}
    };
}

void from_json(const nlohmann::json& j, StockProcessRecord& r) {
    r.id = j.value("id", 0);
    r.ts_code = j.value("ts_code", "");
    r.stock_name = j.value("stock_name", "");
    r.trade_date = j.value("trade_date", "");
    r.created_at = j.value("created_at", "");
    r.expires_at = j.value("expires_at", "");
    
    if (j.contains("strategies") && j["strategies"].is_array()) {
        r.strategies = j["strategies"].get<std::vector<StrategyData>>();
    }
}

} // namespace data