#pragma once

#include <vector>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <limits>

namespace utils {

/**
 * @brief 数学工具类
 * 
 * 提供统计计算、金融计算等数学功能
 */
class MathUtil {
public:
    // ==================== 基础统计 ====================
    
    /**
     * @brief 计算平均值
     * @param values 数值列表
     * @return 平均值
     */
    static double mean(const std::vector<double>& values);
    
    /**
     * @brief 计算中位数
     * @param values 数值列表
     * @return 中位数
     */
    static double median(const std::vector<double>& values);
    
    /**
     * @brief 计算众数
     * @param values 数值列表
     * @return 众数
     */
    static double mode(const std::vector<double>& values);
    
    /**
     * @brief 计算方差
     * @param values 数值列表
     * @param isSample 是否为样本方差，默认 true
     * @return 方差
     */
    static double variance(const std::vector<double>& values, bool isSample = true);
    
    /**
     * @brief 计算标准差
     * @param values 数值列表
     * @param isSample 是否为样本标准差，默认 true
     * @return 标准差
     */
    static double stddev(const std::vector<double>& values, bool isSample = true);
    
    /**
     * @brief 计算总和
     * @param values 数值列表
     * @return 总和
     */
    static double sum(const std::vector<double>& values);
    
    /**
     * @brief 计算最小值
     * @param values 数值列表
     * @return 最小值
     */
    static double min(const std::vector<double>& values);
    
    /**
     * @brief 计算最大值
     * @param values 数值列表
     * @return 最大值
     */
    static double max(const std::vector<double>& values);
    
    /**
     * @brief 计算范围（最大值 - 最小值）
     * @param values 数值列表
     * @return 范围
     */
    static double range(const std::vector<double>& values);
    
    /**
     * @brief 计算百分位数
     * @param values 数值列表
     * @param percentile 百分位（0-100）
     * @return 百分位数值
     */
    static double percentile(const std::vector<double>& values, double percentile);
    
    // ==================== 相关性分析 ====================
    
    /**
     * @brief 计算协方差
     * @param x 数值列表 X
     * @param y 数值列表 Y
     * @param isSample 是否为样本协方差，默认 true
     * @return 协方差
     */
    static double covariance(const std::vector<double>& x, const std::vector<double>& y, bool isSample = true);
    
    /**
     * @brief 计算相关系数（皮尔逊相关系数）
     * @param x 数值列表 X
     * @param y 数值列表 Y
     * @return 相关系数（-1 到 1）
     */
    static double correlation(const std::vector<double>& x, const std::vector<double>& y);
    
    // ==================== 金融计算 ====================
    
    /**
     * @brief 计算收益率
     * @param startValue 起始值
     * @param endValue 结束值
     * @return 收益率（百分比）
     */
    static double returnRate(double startValue, double endValue);
    
    /**
     * @brief 计算对数收益率
     * @param startValue 起始值
     * @param endValue 结束值
     * @return 对数收益率
     */
    static double logReturn(double startValue, double endValue);
    
    /**
     * @brief 计算年化收益率
     * @param totalReturn 总收益率
     * @param years 年数
     * @return 年化收益率
     */
    static double annualizedReturn(double totalReturn, double years);
    
    /**
     * @brief 计算夏普比率
     * @param returns 收益率列表
     * @param riskFreeRate 无风险利率，默认 0
     * @return 夏普比率
     */
    static double sharpeRatio(const std::vector<double>& returns, double riskFreeRate = 0.0);
    
    /**
     * @brief 计算最大回撤
     * @param values 价格或净值列表
     * @return 最大回撤（百分比）
     */
    static double maxDrawdown(const std::vector<double>& values);
    
    /**
     * @brief 计算波动率（年化）
     * @param returns 收益率列表
     * @param periodsPerYear 每年的周期数（日线252，周线52，月线12）
     * @return 年化波动率
     */
    static double volatility(const std::vector<double>& returns, int periodsPerYear = 252);
    
    /**
     * @brief 计算复合年增长率（CAGR）
     * @param startValue 起始值
     * @param endValue 结束值
     * @param years 年数
     * @return CAGR
     */
    static double cagr(double startValue, double endValue, double years);
    
    // ==================== 数值处理 ====================
    
    /**
     * @brief 四舍五入到指定小数位
     * @param value 数值
     * @param decimals 小数位数
     * @return 四舍五入后的值
     */
    static double round(double value, int decimals = 2);
    
    /**
     * @brief 向上取整到指定小数位
     * @param value 数值
     * @param decimals 小数位数
     * @return 向上取整后的值
     */
    static double ceil(double value, int decimals = 2);
    
    /**
     * @brief 向下取整到指定小数位
     * @param value 数值
     * @param decimals 小数位数
     * @return 向下取整后的值
     */
    static double floor(double value, int decimals = 2);
    
    /**
     * @brief 限制数值在指定范围内
     * @param value 数值
     * @param minValue 最小值
     * @param maxValue 最大值
     * @return 限制后的值
     */
    static double clamp(double value, double minValue, double maxValue);
    
    /**
     * @brief 判断两个浮点数是否相等（考虑精度）
     * @param a 数值 A
     * @param b 数值 B
     * @param epsilon 精度，默认 1e-9
     * @return true 相等，false 不相等
     */
    static bool equals(double a, double b, double epsilon = 1e-9);
    
    /**
     * @brief 判断数值是否为零（考虑精度）
     * @param value 数值
     * @param epsilon 精度，默认 1e-9
     * @return true 为零，false 不为零
     */
    static bool isZero(double value, double epsilon = 1e-9);
    
    // ==================== 数组操作 ====================
    
    /**
     * @brief 归一化（Min-Max 归一化）
     * @param values 数值列表
     * @param minValue 目标最小值，默认 0
     * @param maxValue 目标最大值，默认 1
     * @return 归一化后的列表
     */
    static std::vector<double> normalize(const std::vector<double>& values, double minValue = 0.0, double maxValue = 1.0);
    
    /**
     * @brief 标准化（Z-score 标准化）
     * @param values 数值列表
     * @return 标准化后的列表
     */
    static std::vector<double> standardize(const std::vector<double>& values);
    
    /**
     * @brief 计算累积和
     * @param values 数值列表
     * @return 累积和列表
     */
    static std::vector<double> cumsum(const std::vector<double>& values);
    
    /**
     * @brief 计算差分
     * @param values 数值列表
     * @param period 差分周期，默认 1
     * @return 差分后的列表
     */
    static std::vector<double> diff(const std::vector<double>& values, int period = 1);
    
    /**
     * @brief 计算百分比变化
     * @param values 数值列表
     * @return 百分比变化列表
     */
    static std::vector<double> pctChange(const std::vector<double>& values);
    
    // ==================== 移动计算 ====================
    
    /**
     * @brief 计算移动平均
     * @param values 数值列表
     * @param window 窗口大小
     * @return 移动平均列表
     */
    static std::vector<double> movingAverage(const std::vector<double>& values, int window);
    
    /**
     * @brief 计算移动标准差
     * @param values 数值列表
     * @param window 窗口大小
     * @return 移动标准差列表
     */
    static std::vector<double> movingStddev(const std::vector<double>& values, int window);
    
    /**
     * @brief 计算移动最大值
     * @param values 数值列表
     * @param window 窗口大小
     * @return 移动最大值列表
     */
    static std::vector<double> movingMax(const std::vector<double>& values, int window);
    
    /**
     * @brief 计算移动最小值
     * @param values 数值列表
     * @param window 窗口大小
     * @return 移动最小值列表
     */
    static std::vector<double> movingMin(const std::vector<double>& values, int window);

private:
    static constexpr double NaN = std::numeric_limits<double>::quiet_NaN();
};

} // namespace utils

