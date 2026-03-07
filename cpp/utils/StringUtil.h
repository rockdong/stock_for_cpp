#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <sstream>

namespace utils {

/**
 * @brief 字符串工具类
 * 
 * 提供字符串的修剪、分割、连接、转换等功能
 */
class StringUtil {
public:
    // ==================== 修剪 ====================
    
    /**
     * @brief 去除字符串左侧空白字符
     * @param str 输入字符串
     * @return 修剪后的字符串
     */
    static std::string ltrim(const std::string& str);
    
    /**
     * @brief 去除字符串右侧空白字符
     * @param str 输入字符串
     * @return 修剪后的字符串
     */
    static std::string rtrim(const std::string& str);
    
    /**
     * @brief 去除字符串两侧空白字符
     * @param str 输入字符串
     * @return 修剪后的字符串
     */
    static std::string trim(const std::string& str);
    
    /**
     * @brief 去除字符串左侧指定字符
     * @param str 输入字符串
     * @param chars 要去除的字符集
     * @return 修剪后的字符串
     */
    static std::string ltrim(const std::string& str, const std::string& chars);
    
    /**
     * @brief 去除字符串右侧指定字符
     * @param str 输入字符串
     * @param chars 要去除的字符集
     * @return 修剪后的字符串
     */
    static std::string rtrim(const std::string& str, const std::string& chars);
    
    /**
     * @brief 去除字符串两侧指定字符
     * @param str 输入字符串
     * @param chars 要去除的字符集
     * @return 修剪后的字符串
     */
    static std::string trim(const std::string& str, const std::string& chars);
    
    // ==================== 分割与连接 ====================
    
    /**
     * @brief 分割字符串
     * @param str 输入字符串
     * @param delimiter 分隔符
     * @param skipEmpty 是否跳过空字符串，默认 false
     * @return 分割后的字符串列表
     */
    static std::vector<std::string> split(const std::string& str, const std::string& delimiter, bool skipEmpty = false);
    
    /**
     * @brief 分割字符串（按字符）
     * @param str 输入字符串
     * @param delimiter 分隔符字符
     * @param skipEmpty 是否跳过空字符串，默认 false
     * @return 分割后的字符串列表
     */
    static std::vector<std::string> split(const std::string& str, char delimiter, bool skipEmpty = false);
    
    /**
     * @brief 连接字符串列表
     * @param parts 字符串列表
     * @param delimiter 分隔符
     * @return 连接后的字符串
     */
    static std::string join(const std::vector<std::string>& parts, const std::string& delimiter);
    
    // ==================== 大小写转换 ====================
    
    /**
     * @brief 转换为大写
     * @param str 输入字符串
     * @return 大写字符串
     */
    static std::string toUpper(const std::string& str);
    
    /**
     * @brief 转换为小写
     * @param str 输入字符串
     * @return 小写字符串
     */
    static std::string toLower(const std::string& str);
    
    /**
     * @brief 首字母大写
     * @param str 输入字符串
     * @return 首字母大写的字符串
     */
    static std::string capitalize(const std::string& str);
    
    // ==================== 查找与替换 ====================
    
    /**
     * @brief 判断字符串是否以指定前缀开始
     * @param str 输入字符串
     * @param prefix 前缀
     * @return true 以该前缀开始，false 否则
     */
    static bool startsWith(const std::string& str, const std::string& prefix);
    
    /**
     * @brief 判断字符串是否以指定后缀结束
     * @param str 输入字符串
     * @param suffix 后缀
     * @return true 以该后缀结束，false 否则
     */
    static bool endsWith(const std::string& str, const std::string& suffix);
    
    /**
     * @brief 判断字符串是否包含子串
     * @param str 输入字符串
     * @param substr 子串
     * @return true 包含，false 不包含
     */
    static bool contains(const std::string& str, const std::string& substr);
    
    /**
     * @brief 替换所有匹配的子串
     * @param str 输入字符串
     * @param from 要替换的子串
     * @param to 替换为的字符串
     * @return 替换后的字符串
     */
    static std::string replaceAll(const std::string& str, const std::string& from, const std::string& to);
    
    /**
     * @brief 替换第一个匹配的子串
     * @param str 输入字符串
     * @param from 要替换的子串
     * @param to 替换为的字符串
     * @return 替换后的字符串
     */
    static std::string replaceFirst(const std::string& str, const std::string& from, const std::string& to);
    
    // ==================== 判断 ====================
    
    /**
     * @brief 判断字符串是否为空或只包含空白字符
     * @param str 输入字符串
     * @return true 为空或只包含空白，false 否则
     */
    static bool isBlank(const std::string& str);
    
    /**
     * @brief 判断字符串是否为数字
     * @param str 输入字符串
     * @return true 为数字，false 否则
     */
    static bool isNumeric(const std::string& str);
    
    /**
     * @brief 判断字符串是否为整数
     * @param str 输入字符串
     * @return true 为整数，false 否则
     */
    static bool isInteger(const std::string& str);
    
    /**
     * @brief 判断字符串是否为浮点数
     * @param str 输入字符串
     * @return true 为浮点数，false 否则
     */
    static bool isFloat(const std::string& str);
    
    /**
     * @brief 判断字符串是否只包含字母
     * @param str 输入字符串
     * @return true 只包含字母，false 否则
     */
    static bool isAlpha(const std::string& str);
    
    /**
     * @brief 判断字符串是否只包含字母和数字
     * @param str 输入字符串
     * @return true 只包含字母和数字，false 否则
     */
    static bool isAlphaNumeric(const std::string& str);
    
    // ==================== 类型转换 ====================
    
    /**
     * @brief 字符串转整数
     * @param str 输入字符串
     * @param defaultValue 转换失败时的默认值
     * @return 整数值
     */
    static int toInt(const std::string& str, int defaultValue = 0);
    
    /**
     * @brief 字符串转长整数
     * @param str 输入字符串
     * @param defaultValue 转换失败时的默认值
     * @return 长整数值
     */
    static long toLong(const std::string& str, long defaultValue = 0);
    
    /**
     * @brief 字符串转浮点数
     * @param str 输入字符串
     * @param defaultValue 转换失败时的默认值
     * @return 浮点数值
     */
    static double toDouble(const std::string& str, double defaultValue = 0.0);
    
    /**
     * @brief 字符串转布尔值
     * @param str 输入字符串（"true", "1", "yes", "on" 为 true）
     * @param defaultValue 转换失败时的默认值
     * @return 布尔值
     */
    static bool toBool(const std::string& str, bool defaultValue = false);
    
    /**
     * @brief 整数转字符串
     * @param value 整数值
     * @return 字符串
     */
    static std::string toString(int value);
    
    /**
     * @brief 长整数转字符串
     * @param value 长整数值
     * @return 字符串
     */
    static std::string toString(long value);
    
    /**
     * @brief 浮点数转字符串
     * @param value 浮点数值
     * @param precision 精度（小数位数），默认 2
     * @return 字符串
     */
    static std::string toString(double value, int precision = 2);
    
    /**
     * @brief 布尔值转字符串
     * @param value 布尔值
     * @return "true" 或 "false"
     */
    static std::string toString(bool value);
    
    // ==================== 格式化 ====================
    
    /**
     * @brief 填充字符串到指定长度（左侧填充）
     * @param str 输入字符串
     * @param length 目标长度
     * @param fillChar 填充字符，默认空格
     * @return 填充后的字符串
     */
    static std::string padLeft(const std::string& str, size_t length, char fillChar = ' ');
    
    /**
     * @brief 填充字符串到指定长度（右侧填充）
     * @param str 输入字符串
     * @param length 目标长度
     * @param fillChar 填充字符，默认空格
     * @return 填充后的字符串
     */
    static std::string padRight(const std::string& str, size_t length, char fillChar = ' ');
    
    /**
     * @brief 重复字符串
     * @param str 输入字符串
     * @param count 重复次数
     * @return 重复后的字符串
     */
    static std::string repeat(const std::string& str, size_t count);
    
    /**
     * @brief 反转字符串
     * @param str 输入字符串
     * @return 反转后的字符串
     */
    static std::string reverse(const std::string& str);
    
    /**
     * @brief 截断字符串
     * @param str 输入字符串
     * @param maxLength 最大长度
     * @param suffix 超出时的后缀，默认 "..."
     * @return 截断后的字符串
     */
    static std::string truncate(const std::string& str, size_t maxLength, const std::string& suffix = "...");
};

} // namespace utils

