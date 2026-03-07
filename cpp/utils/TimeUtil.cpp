#include "TimeUtil.h"
#include <cstring>
#include <algorithm>

namespace utils {

// ==================== 格式化 ====================

std::string TimeUtil::now(const std::string& format) {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    return timestampToString(time, format);
}

std::string TimeUtil::today(const std::string& format) {
    return now(format);
}

std::string TimeUtil::timestampToString(time_t timestamp, const std::string& format) {
    std::tm tm = timestampToTm(timestamp);
    return TimeUtil::format(tm, format);
}

std::string TimeUtil::format(const std::tm& tm, const std::string& format) {
    char buffer[128];
    std::strftime(buffer, sizeof(buffer), format.c_str(), &tm);
    return std::string(buffer);
}

// ==================== 解析 ====================

time_t TimeUtil::stringToTimestamp(const std::string& timeStr, const std::string& format) {
    std::tm tm = {};
    std::istringstream ss(timeStr);
    ss >> std::get_time(&tm, format.c_str());
    if (ss.fail()) {
        return -1;
    }
    return tmToTimestamp(tm);
}

std::tm TimeUtil::parseDate(const std::string& dateStr) {
    std::tm tm = {};
    std::istringstream ss(dateStr);
    ss >> std::get_time(&tm, "%Y-%m-%d");
    return tm;
}

std::tm TimeUtil::parseDateTime(const std::string& timeStr) {
    std::tm tm = {};
    std::istringstream ss(timeStr);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    return tm;
}

// ==================== 计算 ====================

int TimeUtil::daysBetween(const std::string& date1, const std::string& date2) {
    time_t t1 = stringToTimestamp(date1, "%Y-%m-%d");
    time_t t2 = stringToTimestamp(date2, "%Y-%m-%d");
    if (t1 == -1 || t2 == -1) {
        return 0;
    }
    return static_cast<int>((t2 - t1) / (24 * 3600));
}

std::string TimeUtil::addDays(const std::string& date, int days) {
    time_t timestamp = stringToTimestamp(date, "%Y-%m-%d");
    if (timestamp == -1) {
        return date;
    }
    timestamp += days * 24 * 3600;
    return timestampToString(timestamp, "%Y-%m-%d");
}

std::string TimeUtil::addMonths(const std::string& date, int months) {
    std::tm tm = parseDate(date);
    tm.tm_mon += months;
    
    // 处理月份溢出
    while (tm.tm_mon < 0) {
        tm.tm_mon += 12;
        tm.tm_year--;
    }
    while (tm.tm_mon >= 12) {
        tm.tm_mon -= 12;
        tm.tm_year++;
    }
    
    // 处理日期溢出（如1月31日加1个月）
    int daysInMonth = getDaysInMonth(tm.tm_year + 1900, tm.tm_mon + 1);
    if (tm.tm_mday > daysInMonth) {
        tm.tm_mday = daysInMonth;
    }
    
    return format(tm, "%Y-%m-%d");
}

std::string TimeUtil::addYears(const std::string& date, int years) {
    std::tm tm = parseDate(date);
    tm.tm_year += years;
    
    // 处理闰年2月29日的情况
    if (tm.tm_mon == 1 && tm.tm_mday == 29 && !isLeapYear(tm.tm_year + 1900)) {
        tm.tm_mday = 28;
    }
    
    return format(tm, "%Y-%m-%d");
}

// ==================== 判断 ====================

bool TimeUtil::isLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

bool TimeUtil::isValidDate(const std::string& dateStr) {
    std::tm tm = parseDate(dateStr);
    time_t timestamp = tmToTimestamp(tm);
    return timestamp != -1;
}

bool TimeUtil::isWeekday(const std::string& dateStr) {
    int dayOfWeek = getDayOfWeek(dateStr);
    return dayOfWeek >= 1 && dayOfWeek <= 5;
}

bool TimeUtil::isWeekend(const std::string& dateStr) {
    return !isWeekday(dateStr);
}

// ==================== 获取信息 ====================

int TimeUtil::getDayOfWeek(const std::string& dateStr) {
    std::tm tm = parseDate(dateStr);
    time_t timestamp = tmToTimestamp(tm);
    if (timestamp == -1) {
        return -1;
    }
    tm = timestampToTm(timestamp);
    return tm.tm_wday;
}

int TimeUtil::getDayOfYear(const std::string& dateStr) {
    std::tm tm = parseDate(dateStr);
    time_t timestamp = tmToTimestamp(tm);
    if (timestamp == -1) {
        return -1;
    }
    tm = timestampToTm(timestamp);
    return tm.tm_yday + 1;
}

int TimeUtil::getDaysInMonth(int year, int month) {
    if (month < 1 || month > 12) {
        return 0;
    }
    
    static const int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    if (month == 2 && isLeapYear(year)) {
        return 29;
    }
    
    return daysInMonth[month - 1];
}

time_t TimeUtil::getCurrentTimestamp() {
    return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
}

long long TimeUtil::getCurrentTimestampMs() {
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    return ms.count();
}

// ==================== 日期范围 ====================

std::vector<std::string> TimeUtil::getDateRange(const std::string& startDate, const std::string& endDate) {
    std::vector<std::string> dates;
    int days = daysBetween(startDate, endDate);
    
    if (days < 0) {
        return dates;
    }
    
    for (int i = 0; i <= days; ++i) {
        dates.push_back(addDays(startDate, i));
    }
    
    return dates;
}

std::string TimeUtil::getWeekStart() {
    std::string today = TimeUtil::today();
    int dayOfWeek = getDayOfWeek(today);
    
    // 周日是0，周一是1，调整为周一为起始
    int daysToMonday = (dayOfWeek == 0) ? -6 : -(dayOfWeek - 1);
    return addDays(today, daysToMonday);
}

std::string TimeUtil::getWeekEnd() {
    std::string weekStart = getWeekStart();
    return addDays(weekStart, 6);
}

std::string TimeUtil::getMonthStart() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm = timestampToTm(time);
    tm.tm_mday = 1;
    return format(tm, "%Y-%m-%d");
}

std::string TimeUtil::getMonthEnd() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm = timestampToTm(time);
    int daysInMonth = getDaysInMonth(tm.tm_year + 1900, tm.tm_mon + 1);
    tm.tm_mday = daysInMonth;
    return format(tm, "%Y-%m-%d");
}

std::string TimeUtil::getYearStart() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm = timestampToTm(time);
    tm.tm_mon = 0;
    tm.tm_mday = 1;
    return format(tm, "%Y-%m-%d");
}

std::string TimeUtil::getYearEnd() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm = timestampToTm(time);
    tm.tm_mon = 11;
    tm.tm_mday = 31;
    return format(tm, "%Y-%m-%d");
}

// ==================== 私有辅助函数 ====================

time_t TimeUtil::tmToTimestamp(const std::tm& tm) {
    std::tm tm_copy = tm;
    return std::mktime(&tm_copy);
}

std::tm TimeUtil::timestampToTm(time_t timestamp) {
    std::tm tm;
#ifdef _WIN32
    localtime_s(&tm, &timestamp);
#else
    localtime_r(&timestamp, &tm);
#endif
    return tm;
}

} // namespace utils

