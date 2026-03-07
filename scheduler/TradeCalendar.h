#pragma once

#include <string>

namespace scheduler {

/**
 * @brief 交易日历
 * 
 * 判断当前日期是否为交易日
 */
class TradeCalendar {
public:
    /**
     * @brief 判断是否为交易日
     * @param date 日期字符串，格式 YYYY-MM-DD
     * @return true 是交易日，false 非交易日
     */
    static bool isTradingDay(const std::string& date);
    
    /**
     * @brief 判断当前日期是否为交易日
     * @return true 是交易日，false 非交易日
     */
    static bool isTradingDayToday();
    
    /**
     * @brief 获取星期几
     * @param date 日期字符串，格式 YYYY-MM-DD
     * @return 0=周日, 1=周一, ..., 6=周六
     */
    static int getDayOfWeek(const std::string& date);
};

} // namespace scheduler
