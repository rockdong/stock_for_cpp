#include "StringUtil.h"
#include <stdexcept>
#include <iomanip>

namespace utils {

// ==================== 修剪 ====================

std::string StringUtil::ltrim(const std::string& str) {
    auto it = std::find_if(str.begin(), str.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    });
    return std::string(it, str.end());
}

std::string StringUtil::rtrim(const std::string& str) {
    auto it = std::find_if(str.rbegin(), str.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    });
    return std::string(str.begin(), it.base());
}

std::string StringUtil::trim(const std::string& str) {
    return ltrim(rtrim(str));
}

std::string StringUtil::ltrim(const std::string& str, const std::string& chars) {
    auto it = std::find_if(str.begin(), str.end(), [&chars](char ch) {
        return chars.find(ch) == std::string::npos;
    });
    return std::string(it, str.end());
}

std::string StringUtil::rtrim(const std::string& str, const std::string& chars) {
    auto it = std::find_if(str.rbegin(), str.rend(), [&chars](char ch) {
        return chars.find(ch) == std::string::npos;
    });
    return std::string(str.begin(), it.base());
}

std::string StringUtil::trim(const std::string& str, const std::string& chars) {
    return ltrim(rtrim(str, chars), chars);
}

// ==================== 分割与连接 ====================

std::vector<std::string> StringUtil::split(const std::string& str, const std::string& delimiter, bool skipEmpty) {
    std::vector<std::string> result;
    if (str.empty()) {
        return result;
    }
    
    size_t start = 0;
    size_t end = str.find(delimiter);
    
    while (end != std::string::npos) {
        std::string token = str.substr(start, end - start);
        if (!skipEmpty || !token.empty()) {
            result.push_back(token);
        }
        start = end + delimiter.length();
        end = str.find(delimiter, start);
    }
    
    std::string token = str.substr(start);
    if (!skipEmpty || !token.empty()) {
        result.push_back(token);
    }
    
    return result;
}

std::vector<std::string> StringUtil::split(const std::string& str, char delimiter, bool skipEmpty) {
    return split(str, std::string(1, delimiter), skipEmpty);
}

std::string StringUtil::join(const std::vector<std::string>& parts, const std::string& delimiter) {
    if (parts.empty()) {
        return "";
    }
    
    std::ostringstream oss;
    for (size_t i = 0; i < parts.size(); ++i) {
        if (i > 0) {
            oss << delimiter;
        }
        oss << parts[i];
    }
    return oss.str();
}

// ==================== 大小写转换 ====================

std::string StringUtil::toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

std::string StringUtil::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string StringUtil::capitalize(const std::string& str) {
    if (str.empty()) {
        return str;
    }
    std::string result = str;
    result[0] = std::toupper(result[0]);
    return result;
}

// ==================== 查找与替换 ====================

bool StringUtil::startsWith(const std::string& str, const std::string& prefix) {
    if (prefix.length() > str.length()) {
        return false;
    }
    return str.compare(0, prefix.length(), prefix) == 0;
}

bool StringUtil::endsWith(const std::string& str, const std::string& suffix) {
    if (suffix.length() > str.length()) {
        return false;
    }
    return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
}

bool StringUtil::contains(const std::string& str, const std::string& substr) {
    return str.find(substr) != std::string::npos;
}

std::string StringUtil::replaceAll(const std::string& str, const std::string& from, const std::string& to) {
    if (from.empty()) {
        return str;
    }
    
    std::string result = str;
    size_t pos = 0;
    
    while ((pos = result.find(from, pos)) != std::string::npos) {
        result.replace(pos, from.length(), to);
        pos += to.length();
    }
    
    return result;
}

std::string StringUtil::replaceFirst(const std::string& str, const std::string& from, const std::string& to) {
    if (from.empty()) {
        return str;
    }
    
    std::string result = str;
    size_t pos = result.find(from);
    
    if (pos != std::string::npos) {
        result.replace(pos, from.length(), to);
    }
    
    return result;
}

// ==================== 判断 ====================

bool StringUtil::isBlank(const std::string& str) {
    return str.empty() || trim(str).empty();
}

bool StringUtil::isNumeric(const std::string& str) {
    if (str.empty()) {
        return false;
    }
    
    size_t start = 0;
    if (str[0] == '-' || str[0] == '+') {
        if (str.length() == 1) {
            return false;
        }
        start = 1;
    }
    
    bool hasDecimal = false;
    for (size_t i = start; i < str.length(); ++i) {
        if (str[i] == '.') {
            if (hasDecimal) {
                return false;
            }
            hasDecimal = true;
        } else if (!std::isdigit(str[i])) {
            return false;
        }
    }
    
    return true;
}

bool StringUtil::isInteger(const std::string& str) {
    if (str.empty()) {
        return false;
    }
    
    size_t start = 0;
    if (str[0] == '-' || str[0] == '+') {
        if (str.length() == 1) {
            return false;
        }
        start = 1;
    }
    
    for (size_t i = start; i < str.length(); ++i) {
        if (!std::isdigit(str[i])) {
            return false;
        }
    }
    
    return true;
}

bool StringUtil::isFloat(const std::string& str) {
    return isNumeric(str) && contains(str, ".");
}

bool StringUtil::isAlpha(const std::string& str) {
    if (str.empty()) {
        return false;
    }
    
    return std::all_of(str.begin(), str.end(), [](unsigned char ch) {
        return std::isalpha(ch);
    });
}

bool StringUtil::isAlphaNumeric(const std::string& str) {
    if (str.empty()) {
        return false;
    }
    
    return std::all_of(str.begin(), str.end(), [](unsigned char ch) {
        return std::isalnum(ch);
    });
}

// ==================== 类型转换 ====================

int StringUtil::toInt(const std::string& str, int defaultValue) {
    try {
        return std::stoi(trim(str));
    } catch (...) {
        return defaultValue;
    }
}

long StringUtil::toLong(const std::string& str, long defaultValue) {
    try {
        return std::stol(trim(str));
    } catch (...) {
        return defaultValue;
    }
}

double StringUtil::toDouble(const std::string& str, double defaultValue) {
    try {
        return std::stod(trim(str));
    } catch (...) {
        return defaultValue;
    }
}

bool StringUtil::toBool(const std::string& str, bool defaultValue) {
    std::string lower = toLower(trim(str));
    if (lower == "true" || lower == "1" || lower == "yes" || lower == "on") {
        return true;
    }
    if (lower == "false" || lower == "0" || lower == "no" || lower == "off") {
        return false;
    }
    return defaultValue;
}

std::string StringUtil::toString(int value) {
    return std::to_string(value);
}

std::string StringUtil::toString(long value) {
    return std::to_string(value);
}

std::string StringUtil::toString(double value, int precision) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(precision) << value;
    return oss.str();
}

std::string StringUtil::toString(bool value) {
    return value ? "true" : "false";
}

// ==================== 格式化 ====================

std::string StringUtil::padLeft(const std::string& str, size_t length, char fillChar) {
    if (str.length() >= length) {
        return str;
    }
    return std::string(length - str.length(), fillChar) + str;
}

std::string StringUtil::padRight(const std::string& str, size_t length, char fillChar) {
    if (str.length() >= length) {
        return str;
    }
    return str + std::string(length - str.length(), fillChar);
}

std::string StringUtil::repeat(const std::string& str, size_t count) {
    std::ostringstream oss;
    for (size_t i = 0; i < count; ++i) {
        oss << str;
    }
    return oss.str();
}

std::string StringUtil::reverse(const std::string& str) {
    return std::string(str.rbegin(), str.rend());
}

std::string StringUtil::truncate(const std::string& str, size_t maxLength, const std::string& suffix) {
    if (str.length() <= maxLength) {
        return str;
    }
    
    if (maxLength <= suffix.length()) {
        return str.substr(0, maxLength);
    }
    
    return str.substr(0, maxLength - suffix.length()) + suffix;
}

} // namespace utils

