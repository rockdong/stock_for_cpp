#include "KLineAggregator.h"
#include <map>
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace utils {

int KLineAggregator::getYear(const std::string& date) {
    if (date.length() < 4) return 0;
    return std::stoi(date.substr(0, 4));
}

int KLineAggregator::getMonth(const std::string& date) {
    if (date.length() < 6) return 0;
    return std::stoi(date.substr(4, 2));
}

int KLineAggregator::getDay(const std::string& date) {
    if (date.length() < 8) return 0;
    return std::stoi(date.substr(6, 2));
}

int KLineAggregator::getWeek(const std::string& date) {
    int y = getYear(date);
    int m = getMonth(date);
    int d = getDay(date);
    
    int a = (14 - m) / 12;
    int y2 = y + 4800 - a;
    int m2 = m + 12 * a - 3;
    int jd = d + (153 * m2 + 2) / 5 + 365 * y2 + y2 / 4 - y2 / 100 + y2 / 400;
    
    int wd = (jd + 1) % 7;
    if (wd == 0) wd = 7;
    
    int doy = jd - (d + (153 * (m + 12 * ((14 - m) / 12) - 3) + 2) / 5 + 365 * (y + 4800 - (14 - m) / 12) + (y + 4800 - (14 - m) / 12) / 4 - (y + 4800 - (14 - m) / 12) / 100 + (y + 4800 - (14 - m) / 12) / 400);
    
    return (doy - wd + 10) / 7;
}

std::string KLineAggregator::getWeekKey(const std::string& date) {
    int y = getYear(date);
    int w = getWeek(date);
    std::ostringstream oss;
    oss << y << "-W" << std::setfill('0') << std::setw(2) << w;
    return oss.str();
}

std::string KLineAggregator::getMonthKey(const std::string& date) {
    if (date.length() < 6) return "";
    return date.substr(0, 6);
}

core::StockData KLineAggregator::aggregatePeriod(
    const std::vector<core::StockData>& periodData,
    const std::string& periodEndDate) {
    
    if (periodData.empty()) {
        return core::StockData{};
    }
    
    core::StockData result;
    result.ts_code = periodData[0].ts_code;
    result.trade_date = periodEndDate;
    result.open = periodData.front().open;
    result.close = periodData.back().close;
    result.high = periodData[0].high;
    result.low = periodData[0].low;
    result.volume = 0;
    result.amount = 0;
    
    for (const auto& d : periodData) {
        result.high = std::max(result.high, d.high);
        result.low = std::min(result.low, d.low);
        result.volume += d.volume;
        result.amount += d.amount;
    }
    
    if (periodData.size() > 0) {
        result.pre_close = periodData.front().pre_close;
        result.change = result.close - result.pre_close;
        result.pct_chg = (result.pre_close > 0) ? (result.change / result.pre_close * 100) : 0;
    }
    
    return result;
}

std::vector<core::StockData> KLineAggregator::aggregateToWeekly(
    const std::vector<core::StockData>& dailyData) {
    
    if (dailyData.empty()) return {};
    
    std::map<std::string, std::vector<core::StockData>> weekGroups;
    
    for (const auto& d : dailyData) {
        std::string weekKey = getWeekKey(d.trade_date);
        weekGroups[weekKey].push_back(d);
    }
    
    std::vector<core::StockData> result;
    for (auto& [weekKey, weekData] : weekGroups) {
        std::sort(weekData.begin(), weekData.end(), 
            [](const core::StockData& a, const core::StockData& b) {
                return a.trade_date < b.trade_date;
            });
        
        std::string weekEndDate = weekData.back().trade_date;
        result.push_back(aggregatePeriod(weekData, weekEndDate));
    }
    
    std::sort(result.begin(), result.end(),
        [](const core::StockData& a, const core::StockData& b) {
            return a.trade_date < b.trade_date;
        });
    
    return result;
}

std::vector<core::StockData> KLineAggregator::aggregateToMonthly(
    const std::vector<core::StockData>& dailyData) {
    
    if (dailyData.empty()) return {};
    
    std::map<std::string, std::vector<core::StockData>> monthGroups;
    
    for (const auto& d : dailyData) {
        std::string monthKey = getMonthKey(d.trade_date);
        monthGroups[monthKey].push_back(d);
    }
    
    std::vector<core::StockData> result;
    for (auto& [monthKey, monthData] : monthGroups) {
        std::sort(monthData.begin(), monthData.end(),
            [](const core::StockData& a, const core::StockData& b) {
                return a.trade_date < b.trade_date;
            });
        
        std::string monthEndDate = monthData.back().trade_date;
        result.push_back(aggregatePeriod(monthData, monthEndDate));
    }
    
    std::sort(result.begin(), result.end(),
        [](const core::StockData& a, const core::StockData& b) {
            return a.trade_date < b.trade_date;
        });
    
    return result;
}

}