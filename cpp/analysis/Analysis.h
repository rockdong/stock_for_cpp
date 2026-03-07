#ifndef ANALYSIS_H
#define ANALYSIS_H

/**
 * @file Analysis.h
 * @brief 技术分析模块统一头文件
 * 
 * 包含所有技术指标和工厂类
 */

// 基础接口
#include "IIndicator.h"
#include "IndicatorBase.h"

// 技术指标
#include "indicators/MA.h"
#include "indicators/EMA.h"
#include "indicators/MACD.h"
#include "indicators/RSI.h"
#include "indicators/KDJ.h"
#include "indicators/BOLL.h"
#include "indicators/ATR.h"

// 工厂类
#include "IndicatorFactory.h"

/**
 * @namespace analysis
 * @brief 技术分析命名空间
 * 
 * 包含所有技术指标的实现，使用 TA-Lib 库
 */

#endif // ANALYSIS_H

