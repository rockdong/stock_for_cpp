#pragma once

/**
 * @file Utils.h
 * @brief 工具类模块统一头文件
 * 
 * 包含所有工具类的头文件，提供便捷的导入方式
 */

#include "TimeUtil.h"
#include "StringUtil.h"
#include "MathUtil.h"

/**
 * @namespace utils
 * @brief 工具类命名空间
 * 
 * 包含时间、字符串、数学等常用工具类
 * 
 * 使用示例：
 * @code
 * #include "Utils.h"
 * 
 * // 时间工具
 * std::string today = utils::TimeUtil::today();
 * int days = utils::TimeUtil::daysBetween("2024-01-01", "2024-12-31");
 * 
 * // 字符串工具
 * std::string trimmed = utils::StringUtil::trim("  hello  ");
 * auto parts = utils::StringUtil::split("a,b,c", ",");
 * 
 * // 数学工具
 * double avg = utils::MathUtil::mean({1.0, 2.0, 3.0, 4.0, 5.0});
 * double std = utils::MathUtil::stddev({1.0, 2.0, 3.0, 4.0, 5.0});
 * @endcode
 */

namespace utils {
    // 所有工具类都在此命名空间下
    // - TimeUtil: 时间工具类
    // - StringUtil: 字符串工具类
    // - MathUtil: 数学工具类
}

