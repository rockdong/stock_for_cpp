#include "ChartDataDAO.h"
#include "Connection.h"
#include "Logger.h"
#include "ChartDataTable.h"
#include <sstream>
#include <iomanip>
#include <nlohmann/json.hpp>

namespace data {

namespace {
    std::string doubleToString(double value) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(4) << value;
        return oss.str();
    }
}

std::string ChartDataDAO::toJson(const std::vector<ChartCandle>& candles) {
    nlohmann::json j = nlohmann::json::array();
    
    for (const auto& c : candles) {
        nlohmann::json item;
        item["trade_date"] = c.trade_date;
        item["open"] = c.open;
        item["high"] = c.high;
        item["low"] = c.low;
        item["close"] = c.close;
        item["volume"] = c.volume;
        item["ema17"] = c.ema17;
        item["ema25"] = c.ema25;
        j.push_back(item);
    }
    
    return j.dump();
}

std::vector<ChartCandle> ChartDataDAO::fromJson(const std::string& json) {
    std::vector<ChartCandle> candles;
    
    try {
        nlohmann::json j = nlohmann::json::parse(json);
        
        for (const auto& item : j) {
            ChartCandle c;
            c.trade_date = item["trade_date"];
            c.open = item["open"];
            c.high = item["high"];
            c.low = item["low"];
            c.close = item["close"];
            c.volume = item["volume"];
            c.ema17 = item["ema17"];
            c.ema25 = item["ema25"];
            candles.push_back(c);
        }
    } catch (const std::exception& e) {
        LOG_ERROR("解析图表数据 JSON 失败: " + std::string(e.what()));
    }
    
    return candles;
}

bool ChartDataDAO::save(const ChartData& data) {
    auto& conn = Connection::getInstance();
    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return false;
    }
    
    try {
        ChartDataTable table;
        auto db = conn.getDb();
        
        std::string jsonData = toJson(data.candles);
        
        (*db)(sqlpp::sqlite3::insert_or_replace_into(table).set(
            table.tsCode = data.ts_code,
            table.freq = data.freq,
            table.analysisDate = data.analysis_date,
            table.data = jsonData
        ));
        
        LOG_DEBUG("保存图表数据成功: " + data.ts_code + " - " + data.freq);
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("保存图表数据失败: " + std::string(e.what()));
        return false;
    }
}

std::optional<ChartData> ChartDataDAO::findByTsCodeAndFreq(
    const std::string& ts_code,
    const std::string& freq
) {
    auto& conn = Connection::getInstance();
    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return std::nullopt;
    }
    
    try {
        ChartDataTable table;
        auto db = conn.getDb();
        
        for (const auto& row : (*db)(
            sqlpp::select(all_of(table))
                .from(table)
                .where(table.tsCode == ts_code and table.freq == freq)
                .order_by(table.analysisDate.desc())
                .limit(1u)
        )) {
            ChartData data;
            data.ts_code = row.tsCode;
            data.freq = row.freq;
            data.analysis_date = row.analysisDate;
            data.candles = fromJson(row.data);
            return data;
        }
        
        return std::nullopt;
    } catch (const std::exception& e) {
        LOG_ERROR("查询图表数据失败: " + std::string(e.what()));
        return std::nullopt;
    }
}

bool ChartDataDAO::remove(const std::string& ts_code, const std::string& freq) {
    auto& conn = Connection::getInstance();
    if (!conn.isConnected()) {
        LOG_ERROR("数据库未连接");
        return false;
    }
    
    try {
        ChartDataTable table;
        auto db = conn.getDb();
        
        (*db)(sqlpp::remove_from(table)
            .where(table.tsCode == ts_code and table.freq == freq));
        
        LOG_DEBUG("删除图表数据成功: " + ts_code + " - " + freq);
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("删除图表数据失败: " + std::string(e.what()));
        return false;
    }
}

} // namespace data