#ifndef CORE_CORE_H
#define CORE_CORE_H

/**
 * @file Core.h
 * @brief 核心业务模块统一头文件
 * 
 * 包含所有核心业务相关的头文件
 */

// 数据结构
#include "Stock.h"

// 策略系统
#include "Strategy.h"
#include "StrategyFactory.h"

// 具体策略
#include "strategies/MACrossStrategy.h"
#include "strategies/MACDStrategy.h"
#include "strategies/RSIStrategy.h"
#include "strategies/BOLLStrategy.h"
#include "strategies/GridStrategy.h"

#endif // CORE_CORE_H
