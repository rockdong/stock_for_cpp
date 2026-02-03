#ifndef CORE_H
#define CORE_H

/**
 * @file Core.h
 * @brief 核心模块统一头文件
 * 
 * 核心模块提供系统的核心数据结构和业务实体
 * 
 * 包含内容：
 * - Stock: 股票基本信息（核心数据结构）
 * - StockEntity: 股票数据库实体
 * - StockData: 股票行情数据
 * 
 * 使用方式：
 * ```cpp
 * #include "Core.h"
 * 
 * core::Stock stock;
 * stock.ts_code = "000001.SZ";
 * stock.name = "平安银行";
 * ```
 */

#include "Stock.h"

#endif // CORE_H

