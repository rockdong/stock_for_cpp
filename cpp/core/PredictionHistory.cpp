#include "PredictionHistory.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include "../thirdparty/json/include/nlohmann/json.hpp"

namespace core {

using json = nlohmann::json;

PredictionHistory::PredictionHistory() 
    : dataPath_("data/signal_predictions.json") {}

PredictionHistory::PredictionHistory(const std::string& dataPath)
    : dataPath_(dataPath) {
    loadFromJson();
}

std::string PredictionHistory::generateSignalId(const std::string& date) {
    std::ostringstream oss;
    oss << "BS-" << date << "-" << std::setw(3) << std::setfill('0') << nextSignalId_++;
    return oss.str();
}

std::string PredictionHistory::getCurrentTimestamp() const {
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
    return oss.str();
}

std::string PredictionHistory::addPrediction(const PredictionRecord& record) {
    PredictionRecord newRecord = record;
    if (newRecord.signal_id.empty()) {
        newRecord.signal_id = generateSignalId(newRecord.trade_date);
    }
    if (newRecord.created_at.empty()) {
        newRecord.created_at = getCurrentTimestamp();
    }
    predictions_.push_back(newRecord);
    saveToJson();
    return newRecord.signal_id;
}

bool PredictionHistory::verifyPrediction(
    const std::string& signalId,
    double actualGain,
    double actualDirection
) {
    auto it = std::find_if(predictions_.begin(), predictions_.end(),
        [&signalId](const PredictionRecord& r) { return r.signal_id == signalId; });
    
    if (it == predictions_.end()) {
        return false;
    }
    
    it->actual_gain = actualGain;
    it->actual_direction = actualDirection;
    it->verified = true;
    it->verified_at = getCurrentTimestamp();
    
    double directionMatch = (it->predicted_direction * actualDirection > 0) ? 1.0 : 0.0;
    double gainError = std::abs(it->predicted_gain - actualGain);
    it->accuracy_score = directionMatch * (1.0 - std::min(gainError, 1.0));
    it->accurate = it->accuracy_score > 0.5;
    
    saveToJson();
    return true;
}

std::vector<PredictionRecord> PredictionHistory::getUnverifiedPredictions() const {
    std::vector<PredictionRecord> result;
    std::copy_if(predictions_.begin(), predictions_.end(), std::back_inserter(result),
        [](const PredictionRecord& r) { return !r.verified; });
    return result;
}

std::vector<PredictionRecord> PredictionHistory::getPredictionsByDateRange(
    const std::string& startDate,
    const std::string& endDate
) const {
    std::vector<PredictionRecord> result;
    std::copy_if(predictions_.begin(), predictions_.end(), std::back_inserter(result),
        [&startDate, &endDate](const PredictionRecord& r) {
            return r.trade_date >= startDate && r.trade_date <= endDate;
        });
    return result;
}

std::vector<PredictionRecord> PredictionHistory::getPredictionsByStock(const std::string& tsCode) const {
    std::vector<PredictionRecord> result;
    std::copy_if(predictions_.begin(), predictions_.end(), std::back_inserter(result),
        [&tsCode](const PredictionRecord& r) { return r.ts_code == tsCode; });
    return result;
}

std::vector<PredictionRecord> PredictionHistory::getPredictionsByStrategy(const std::string& strategyName) const {
    std::vector<PredictionRecord> result;
    std::copy_if(predictions_.begin(), predictions_.end(), std::back_inserter(result),
        [&strategyName](const PredictionRecord& r) { return r.strategy_name == strategyName; });
    return result;
}

bool PredictionHistory::saveToJson(const std::string& filepath) const {
    std::string path = filepath.empty() ? dataPath_ : filepath;
    
    json j = json::array();
    for (const auto& record : predictions_) {
        json recordJson;
        recordJson["signal_id"] = record.signal_id;
        recordJson["ts_code"] = record.ts_code;
        recordJson["trade_date"] = record.trade_date;
        recordJson["strategy_name"] = record.strategy_name;
        recordJson["predicted_direction"] = record.predicted_direction;
        recordJson["predicted_gain"] = record.predicted_gain;
        recordJson["confidence"] = record.confidence;
        recordJson["indicators"] = record.indicators;
        recordJson["parameters"] = record.parameters;
        recordJson["verified"] = record.verified;
        recordJson["actual_gain"] = record.actual_gain;
        recordJson["actual_direction"] = record.actual_direction;
        recordJson["accurate"] = record.accurate;
        recordJson["accuracy_score"] = record.accuracy_score;
        recordJson["created_at"] = record.created_at;
        recordJson["verified_at"] = record.verified_at;
        j.push_back(recordJson);
    }
    
    std::ofstream file(path);
    if (!file.is_open()) {
        return false;
    }
    
    file << j.dump(2);
    return true;
}

bool PredictionHistory::loadFromJson(const std::string& filepath) {
    std::string path = filepath.empty() ? dataPath_ : filepath;
    
    std::ifstream file(path);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        json j;
        file >> j;
        
        predictions_.clear();
        for (const auto& item : j) {
            PredictionRecord record;
            record.signal_id = item.value("signal_id", "");
            record.ts_code = item.value("ts_code", "");
            record.trade_date = item.value("trade_date", "");
            record.strategy_name = item.value("strategy_name", "");
            record.predicted_direction = item.value("predicted_direction", 0.0);
            record.predicted_gain = item.value("predicted_gain", 0.0);
            record.confidence = item.value("confidence", 0.0);
            
            if (item.contains("indicators")) {
                record.indicators = item["indicators"].get<std::map<std::string, double>>();
            }
            if (item.contains("parameters")) {
                record.parameters = item["parameters"].get<std::map<std::string, double>>();
            }
            
            record.verified = item.value("verified", false);
            record.actual_gain = item.value("actual_gain", 0.0);
            record.actual_direction = item.value("actual_direction", 0.0);
            record.accurate = item.value("accurate", false);
            record.accuracy_score = item.value("accuracy_score", 0.0);
            record.created_at = item.value("created_at", "");
            record.verified_at = item.value("verified_at", "");
            
            predictions_.push_back(record);
        }
        
        int maxId = 0;
        for (const auto& r : predictions_) {
            size_t pos = r.signal_id.rfind('-');
            if (pos != std::string::npos) {
                int id = std::stoi(r.signal_id.substr(pos + 1));
                maxId = std::max(maxId, id);
            }
        }
        nextSignalId_ = maxId + 1;
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

} // namespace core