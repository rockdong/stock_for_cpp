#ifndef NETWORK_H
#define NETWORK_H

/**
 * @file Network.h
 * @brief 网络层统一头文件
 * 
 * 包含所有网络层相关的头文件，提供便捷的导入方式
 */

// HTTP 客户端
#include "http/HttpClient.h"
#include "http/TushareClient.h"

// 数据源
#include "IDataSource.h"
#include "TushareDataSource.h"
#include "DataSourceFactory.h"

/**
 * @namespace network
 * @brief 网络层命名空间
 * 
 * 包含所有网络相关的类和函数：
 * - HTTP 客户端封装
 * - Tushare API 客户端
 * - 数据源接口和实现
 * - 工厂类
 */

#endif // NETWORK_H

