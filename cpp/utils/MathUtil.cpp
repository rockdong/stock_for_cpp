#include "MathUtil.h"
#include <map>
#include <stdexcept>

namespace utils {

// ==================== 基础统计 ====================

double MathUtil::mean(const std::vector<double>& values) {
    if (values.empty()) {
        return NaN;
    }
    return sum(values) / values.size();
}

double MathUtil::median(const std::vector<double>& values) {
    if (values.empty()) {
        return NaN;
    }
    
    std::vector<double> sorted = values;
    std::sort(sorted.begin(), sorted.end());
    
    size_t n = sorted.size();
    if (n % 2 == 0) {
        return (sorted[n / 2 - 1] + sorted[n / 2]) / 2.0;
    } else {
        return sorted[n / 2];
    }
}

double MathUtil::mode(const std::vector<double>& values) {
    if (values.empty()) {
        return NaN;
    }
    
    std::map<double, int> frequency;
    for (double value : values) {
        frequency[value]++;
    }
    
    int maxCount = 0;
    double modeValue = values[0];
    
    for (const auto& pair : frequency) {
        if (pair.second > maxCount) {
            maxCount = pair.second;
            modeValue = pair.first;
        }
    }
    
    return modeValue;
}

double MathUtil::variance(const std::vector<double>& values, bool isSample) {
    if (values.empty() || (isSample && values.size() == 1)) {
        return NaN;
    }
    
    double avg = mean(values);
    double sumSquaredDiff = 0.0;
    
    for (double value : values) {
        double diff = value - avg;
        sumSquaredDiff += diff * diff;
    }
    
    size_t n = isSample ? values.size() - 1 : values.size();
    return sumSquaredDiff / n;
}

double MathUtil::stddev(const std::vector<double>& values, bool isSample) {
    double var = variance(values, isSample);
    return std::isnan(var) ? NaN : std::sqrt(var);
}

double MathUtil::sum(const std::vector<double>& values) {
    return std::accumulate(values.begin(), values.end(), 0.0);
}

double MathUtil::min(const std::vector<double>& values) {
    if (values.empty()) {
        return NaN;
    }
    return *std::min_element(values.begin(), values.end());
}

double MathUtil::max(const std::vector<double>& values) {
    if (values.empty()) {
        return NaN;
    }
    return *std::max_element(values.begin(), values.end());
}

double MathUtil::range(const std::vector<double>& values) {
    if (values.empty()) {
        return NaN;
    }
    return max(values) - min(values);
}

double MathUtil::percentile(const std::vector<double>& values, double percentile) {
    if (values.empty() || percentile < 0 || percentile > 100) {
        return NaN;
    }
    
    std::vector<double> sorted = values;
    std::sort(sorted.begin(), sorted.end());
    
    double index = (percentile / 100.0) * (sorted.size() - 1);
    size_t lower = static_cast<size_t>(std::floor(index));
    size_t upper = static_cast<size_t>(std::ceil(index));
    
    if (lower == upper) {
        return sorted[lower];
    }
    
    double weight = index - lower;
    return sorted[lower] * (1 - weight) + sorted[upper] * weight;
}

// ==================== 相关性分析 ====================

double MathUtil::covariance(const std::vector<double>& x, const std::vector<double>& y, bool isSample) {
    if (x.empty() || y.empty() || x.size() != y.size()) {
        return NaN;
    }
    
    if (isSample && x.size() == 1) {
        return NaN;
    }
    
    double meanX = mean(x);
    double meanY = mean(y);
    double sum = 0.0;
    
    for (size_t i = 0; i < x.size(); ++i) {
        sum += (x[i] - meanX) * (y[i] - meanY);
    }
    
    size_t n = isSample ? x.size() - 1 : x.size();
    return sum / n;
}

double MathUtil::correlation(const std::vector<double>& x, const std::vector<double>& y) {
    if (x.empty() || y.empty() || x.size() != y.size()) {
        return NaN;
    }
    
    double cov = covariance(x, y, true);
    double stdX = stddev(x, true);
    double stdY = stddev(y, true);
    
    if (isZero(stdX) || isZero(stdY)) {
        return NaN;
    }
    
    return cov / (stdX * stdY);
}

// ==================== 金融计算 ====================

double MathUtil::returnRate(double startValue, double endValue) {
    if (isZero(startValue)) {
        return NaN;
    }
    return ((endValue - startValue) / startValue) * 100.0;
}

double MathUtil::logReturn(double startValue, double endValue) {
    if (startValue <= 0 || endValue <= 0) {
        return NaN;
    }
    return std::log(endValue / startValue);
}

double MathUtil::annualizedReturn(double totalReturn, double years) {
    if (years <= 0) {
        return NaN;
    }
    return std::pow(1.0 + totalReturn, 1.0 / years) - 1.0;
}

double MathUtil::sharpeRatio(const std::vector<double>& returns, double riskFreeRate) {
    if (returns.empty()) {
        return NaN;
    }
    
    double avgReturn = mean(returns);
    double std = stddev(returns, true);
    
    if (isZero(std)) {
        return NaN;
    }
    
    return (avgReturn - riskFreeRate) / std;
}

double MathUtil::maxDrawdown(const std::vector<double>& values) {
    if (values.empty()) {
        return NaN;
    }
    
    double maxDD = 0.0;
    double peak = values[0];
    
    for (double value : values) {
        if (value > peak) {
            peak = value;
        }
        
        double drawdown = (peak - value) / peak;
        if (drawdown > maxDD) {
            maxDD = drawdown;
        }
    }
    
    return maxDD * 100.0;
}

double MathUtil::volatility(const std::vector<double>& returns, int periodsPerYear) {
    if (returns.empty() || periodsPerYear <= 0) {
        return NaN;
    }
    
    double std = stddev(returns, true);
    return std * std::sqrt(periodsPerYear);
}

double MathUtil::cagr(double startValue, double endValue, double years) {
    if (startValue <= 0 || endValue <= 0 || years <= 0) {
        return NaN;
    }
    return (std::pow(endValue / startValue, 1.0 / years) - 1.0) * 100.0;
}

// ==================== 数值处理 ====================

double MathUtil::round(double value, int decimals) {
    double multiplier = std::pow(10.0, decimals);
    return std::round(value * multiplier) / multiplier;
}

double MathUtil::ceil(double value, int decimals) {
    double multiplier = std::pow(10.0, decimals);
    return std::ceil(value * multiplier) / multiplier;
}

double MathUtil::floor(double value, int decimals) {
    double multiplier = std::pow(10.0, decimals);
    return std::floor(value * multiplier) / multiplier;
}

double MathUtil::clamp(double value, double minValue, double maxValue) {
    return std::max(minValue, std::min(value, maxValue));
}

bool MathUtil::equals(double a, double b, double epsilon) {
    return std::abs(a - b) < epsilon;
}

bool MathUtil::isZero(double value, double epsilon) {
    return std::abs(value) < epsilon;
}

// ==================== 数组操作 ====================

std::vector<double> MathUtil::normalize(const std::vector<double>& values, double minValue, double maxValue) {
    if (values.empty()) {
        return {};
    }
    
    double minVal = min(values);
    double maxVal = max(values);
    double rangeVal = maxVal - minVal;
    
    if (isZero(rangeVal)) {
        return std::vector<double>(values.size(), (minValue + maxValue) / 2.0);
    }
    
    std::vector<double> result;
    result.reserve(values.size());
    
    for (double value : values) {
        double normalized = (value - minVal) / rangeVal;
        result.push_back(minValue + normalized * (maxValue - minValue));
    }
    
    return result;
}

std::vector<double> MathUtil::standardize(const std::vector<double>& values) {
    if (values.empty()) {
        return {};
    }
    
    double avg = mean(values);
    double std = stddev(values, true);
    
    if (isZero(std)) {
        return std::vector<double>(values.size(), 0.0);
    }
    
    std::vector<double> result;
    result.reserve(values.size());
    
    for (double value : values) {
        result.push_back((value - avg) / std);
    }
    
    return result;
}

std::vector<double> MathUtil::cumsum(const std::vector<double>& values) {
    std::vector<double> result;
    result.reserve(values.size());
    
    double sum = 0.0;
    for (double value : values) {
        sum += value;
        result.push_back(sum);
    }
    
    return result;
}

std::vector<double> MathUtil::diff(const std::vector<double>& values, int period) {
    if (values.size() <= static_cast<size_t>(period)) {
        return {};
    }
    
    std::vector<double> result;
    result.reserve(values.size() - period);
    
    for (size_t i = period; i < values.size(); ++i) {
        result.push_back(values[i] - values[i - period]);
    }
    
    return result;
}

std::vector<double> MathUtil::pctChange(const std::vector<double>& values) {
    if (values.size() <= 1) {
        return {};
    }
    
    std::vector<double> result;
    result.reserve(values.size() - 1);
    
    for (size_t i = 1; i < values.size(); ++i) {
        if (isZero(values[i - 1])) {
            result.push_back(NaN);
        } else {
            result.push_back((values[i] - values[i - 1]) / values[i - 1] * 100.0);
        }
    }
    
    return result;
}

// ==================== 移动计算 ====================

std::vector<double> MathUtil::movingAverage(const std::vector<double>& values, int window) {
    if (values.size() < static_cast<size_t>(window) || window <= 0) {
        return {};
    }
    
    std::vector<double> result;
    result.reserve(values.size() - window + 1);
    
    for (size_t i = 0; i <= values.size() - window; ++i) {
        std::vector<double> windowValues(values.begin() + i, values.begin() + i + window);
        result.push_back(mean(windowValues));
    }
    
    return result;
}

std::vector<double> MathUtil::movingStddev(const std::vector<double>& values, int window) {
    if (values.size() < static_cast<size_t>(window) || window <= 0) {
        return {};
    }
    
    std::vector<double> result;
    result.reserve(values.size() - window + 1);
    
    for (size_t i = 0; i <= values.size() - window; ++i) {
        std::vector<double> windowValues(values.begin() + i, values.begin() + i + window);
        result.push_back(stddev(windowValues, true));
    }
    
    return result;
}

std::vector<double> MathUtil::movingMax(const std::vector<double>& values, int window) {
    if (values.size() < static_cast<size_t>(window) || window <= 0) {
        return {};
    }
    
    std::vector<double> result;
    result.reserve(values.size() - window + 1);
    
    for (size_t i = 0; i <= values.size() - window; ++i) {
        std::vector<double> windowValues(values.begin() + i, values.begin() + i + window);
        result.push_back(max(windowValues));
    }
    
    return result;
}

std::vector<double> MathUtil::movingMin(const std::vector<double>& values, int window) {
    if (values.size() < static_cast<size_t>(window) || window <= 0) {
        return {};
    }
    
    std::vector<double> result;
    result.reserve(values.size() - window + 1);
    
    for (size_t i = 0; i <= values.size() - window; ++i) {
        std::vector<double> windowValues(values.begin() + i, values.begin() + i + window);
        result.push_back(min(windowValues));
    }
    
    return result;
}

} // namespace utils

