#include "TradeCalendar.h"
#include "TimeUtil.h"
#include <ctime>

namespace scheduler {

bool TradeCalendar::isTradingDay(const std::string& date) {
    int dayOfWeek = getDayOfWeek(date);
    return dayOfWeek >= 1 && dayOfWeek <= 5;
}

bool TradeCalendar::isTradingDayToday() {
    std::string today = utils::TimeUtil::today();
    return isTradingDay(today);
}

int TradeCalendar::getDayOfWeek(const std::string& date) {
    std::tm tm = {};
    if (sscanf(date.c_str(), "%d-%d-%d", &tm.tm_year, &tm.tm_mon, &tm.tm_mday) != 3) {
        return -1;
    }
    tm.tm_year -= 1900;
    tm.tm_mon -= 1;
    
    std::mktime(&tm);
    return tm.tm_wday;
}

} // namespace scheduler
