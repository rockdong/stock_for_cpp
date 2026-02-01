#pragma once

#include <string>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <vector>

namespace utils {

/**
 * @brief 时间工具类
 * 
 * 提供日期时间的格式化、解析、计算等功能
 */
class TimeUtil {
public:
    // ==================== 格式化 ====================
    
    /**
     * @brief 获取当前时间字符串
     * @param format 格式字符串，默认 "YYYY-MM-DD HH:MM:SS"
     * @return 格式化的时间字符串
     */
    static std::string now(const std::string& format = "%Y-%m-%d %H:%M:%S");
    
    /**
     * @brief 获取当前日期字符串
     * @param format 格式字符串，默认 "YYYY-MM-DD"
     * @return 格式化的日期字符串
     */
    static std::string today(const std::string& format = "%Y-%m-%d");
    
    /**
     * @brief 时间戳转字符串
     * @param timestamp 时间戳（秒）
     * @param format 格式字符串
     * @return 格式化的时间字符串
     */
    static std::string timestampToString(time_t timestamp, const std::string& format = "%Y-%m-%d %H:%M:%S");
    
    /**
     * @brief 格式化时间
     * @param tm 时间结构体
     * @param format 格式字符串
     * @return 格式化的时间字符串
     */
    static std::string format(const std::tm& tm, const std::string& format = "%Y-%m-%d %H:%M:%S");
    
    // ==================== 解析 ====================
    
    /**
     * @brief 字符串转时间戳
     * @param timeStr 时间字符串
     * @param format 格式字符串
     * @return 时间戳（秒），解析失败返回 -1
     */
    static time_t stringToTimestamp(const std::string& timeStr, const std::string& format = "%Y-%m-%d %H:%M:%S");
    
    /**
     * @brief 解析日期字符串
     * @param dateStr 日期字符串，如 "2024-01-01"
     * @return 时间结构体
     */
    static std::tm parseDate(const std::string& dateStr);
    
    /**
     * @brief 解析时间字符串
     * @param timeStr 时间字符串，如 "2024-01-01 12:30:45"
     * @return 时间结构体
     */
    static std::tm parseDateTime(const std::string& timeStr);
    
    // ==================== 计算 ====================
    
    /**
     * @brief 计算两个日期之间的天数差
     * @param date1 日期1，格式 "YYYY-MM-DD"
     * @param date2 日期2，格式 "YYYY-MM-DD"
     * @return 天数差（date2 - date1）
     */
    static int daysBetween(const std::string& date1, const std::string& date2);
    
    /**
     * @brief 日期加减天数
     * @param date 日期字符串，格式 "YYYY-MM-DD"
     * @param days 天数（正数为加，负数为减）
     * @return 新的日期字符串
     */
    static std::string addDays(const std::string& date, int days);
    
    /**
     * @brief 日期加减月数
     * @param date 日期字符串，格式 "YYYY-MM-DD"
     * @param months 月数（正数为加，负数为减）
     * @return 新的日期字符串
     */
    static std::string addMonths(const std::string& date, int months);
    
    /**
     * @brief 日期加减年数
     * @param date 日期字符串，格式 "YYYY-MM-DD"
     * @param years 年数（正数为加，负数为减）
     * @return 新的日期字符串
     */
    static std::string addYears(const std::string& date, int years);
    
    // ==================== 判断 ====================
    
    /**
     * @brief 判断是否为闰年
     * @param year 年份
     * @return true 为闰年，false 为平年
     */
    static bool isLeapYear(int year);
    
    /**
     * @brief 判断日期是否有效
     * @param dateStr 日期字符串，格式 "YYYY-MM-DD"
     * @return true 有效，false 无效
     */
    static bool isValidDate(const std::string& dateStr);
    
    /**
     * @brief 判断是否为工作日（周一到周五）
     * @param dateStr 日期字符串，格式 "YYYY-MM-DD"
     * @return true 为工作日，false 为周末
     */
    static bool isWeekday(const std::string& dateStr);
    
    /**
     * @brief 判断是否为周末（周六或周日）
     * @param dateStr 日期字符串，格式 "YYYY-MM-DD"
     * @return true 为周末，false 为工作日
     */
    static bool isWeekend(const std::string& dateStr);
    
    // ==================== 获取信息 ====================
    
    /**
     * @brief 获取星期几（0=周日，1=周一，...，6=周六）
     * @param dateStr 日期字符串，格式 "YYYY-MM-DD"
     * @return 星期几（0-6）
     */
    static int getDayOfWeek(const std::string& dateStr);
    
    /**
     * @brief 获取一年中的第几天（1-366）
     * @param dateStr 日期字符串，格式 "YYYY-MM-DD"
     * @return 一年中的第几天
     */
    static int getDayOfYear(const std::string& dateStr);
    
    /**
     * @brief 获取某月的天数
     * @param year 年份
     * @param month 月份（1-12）
     * @return 该月的天数
     */
    static int getDaysInMonth(int year, int month);
    
    /**
     * @brief 获取当前时间戳（秒）
     * @return 时间戳
     */
    static time_t getCurrentTimestamp();
    
    /**
     * @brief 获取当前时间戳（毫秒）
     * @return 时间戳（毫秒）
     */
    static long long getCurrentTimestampMs();
    
    // ==================== 日期范围 ====================
    
    /**
     * @brief 生成日期范围
     * @param startDate 开始日期，格式 "YYYY-MM-DD"
     * @param endDate 结束日期，格式 "YYYY-MM-DD"
     * @return 日期列表
     */
    static std::vector<std::string> getDateRange(const std::string& startDate, const std::string& endDate);
    
    /**
     * @brief 获取本周的开始日期（周一）
     * @return 日期字符串
     */
    static std::string getWeekStart();
    
    /**
     * @brief 获取本周的结束日期（周日）
     * @return 日期字符串
     */
    static std::string getWeekEnd();
    
    /**
     * @brief 获取本月的开始日期
     * @return 日期字符串
     */
    static std::string getMonthStart();
    
    /**
     * @brief 获取本月的结束日期
     * @return 日期字符串
     */
    static std::string getMonthEnd();
    
    /**
     * @brief 获取本年的开始日期
     * @return 日期字符串
     */
    static std::string getYearStart();
    
    /**
     * @brief 获取本年的结束日期
     * @return 日期字符串
     */
    static std::string getYearEnd();

private:
    /**
     * @brief 辅助函数：将 tm 结构体转换为时间戳
     */
    static time_t tmToTimestamp(const std::tm& tm);
    
    /**
     * @brief 辅助函数：将时间戳转换为 tm 结构体
     */
    static std::tm timestampToTm(time_t timestamp);
};

} // namespace utils

