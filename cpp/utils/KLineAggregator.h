#ifndef UTILS_KLINE_AGGREGATOR_H
#define UTILS_KLINE_AGGREGATOR_H

#include <vector>
#include <string>
#include "../core/Stock.h"

namespace utils {

class KLineAggregator {
public:
    static std::vector<core::StockData> aggregateToWeekly(
        const std::vector<core::StockData>& dailyData);

    static std::vector<core::StockData> aggregateToMonthly(
        const std::vector<core::StockData>& dailyData);

    static std::vector<core::StockData> aggregatePartial(
        const std::vector<core::StockData>& dailyData,
        const std::string& lastWeeklyDate,
        const std::string& lastMonthlyDate);

private:
    static int getYear(const std::string& date);
    static int getMonth(const std::string& date);
    static int getWeek(const std::string& date);
    static int getDay(const std::string& date);
    
    static std::string getWeekKey(const std::string& date);
    static std::string getMonthKey(const std::string& date);
    
    static core::StockData aggregatePeriod(
        const std::vector<core::StockData>& periodData,
        const std::string& periodEndDate);
};

} // namespace utils

#endif // UTILS_KLINE_AGGREGATOR_H