#include "Core.h"
#include <iostream>

/**
 * @brief Core 模块使用示例
 * 
 * 演示如何使用统一的核心数据结构
 */

int main() {
    std::cout << "=== Core 模块使用示例 ===" << std::endl;
    
    // 1. 创建股票对象
    core::Stock stock;
    stock.ts_code = "000001.SZ";
    stock.symbol = "000001";
    stock.name = "平安银行";
    stock.area = "深圳";
    stock.industry = "银行";
    stock.fullname = "平安银行股份有限公司";
    stock.market = "主板";
    stock.exchange = "SZSE";
    stock.list_status = "L";
    stock.list_date = "19910403";
    
    std::cout << "\n1. 股票基本信息：" << std::endl;
    std::cout << "   代码: " << stock.ts_code << std::endl;
    std::cout << "   名称: " << stock.name << std::endl;
    std::cout << "   行业: " << stock.industry << std::endl;
    std::cout << "   市场: " << stock.market << std::endl;
    
    // 2. 创建数据库实体
    core::StockEntity entity;
    entity.id = 1;
    entity.stock = stock;  // 组合核心结构
    entity.created_at = "2024-01-01 10:00:00";
    entity.updated_at = "2024-01-01 10:00:00";
    
    std::cout << "\n2. 数据库实体：" << std::endl;
    std::cout << "   ID: " << entity.id << std::endl;
    std::cout << "   股票: " << entity.stock.name << std::endl;
    std::cout << "   创建时间: " << entity.created_at << std::endl;
    
    // 3. 创建行情数据
    core::StockData data;
    data.ts_code = "000001.SZ";
    data.trade_date = "20240101";
    data.open = 10.50;
    data.high = 10.80;
    data.low = 10.30;
    data.close = 10.65;
    data.pre_close = 10.50;
    data.change = 0.15;
    data.pct_chg = 1.43;
    data.volume = 1000000;
    data.amount = 10650000;
    
    std::cout << "\n3. 行情数据：" << std::endl;
    std::cout << "   日期: " << data.trade_date << std::endl;
    std::cout << "   开盘: " << data.open << std::endl;
    std::cout << "   收盘: " << data.close << std::endl;
    std::cout << "   涨跌幅: " << data.pct_chg << "%" << std::endl;
    std::cout << "   成交量: " << data.volume << std::endl;
    
    // 4. 演示跨模块使用（模拟）
    std::cout << "\n4. 跨模块使用：" << std::endl;
    
    // 模拟 Network 模块返回
    auto networkStock = stock;  // network::Stock = core::Stock
    std::cout << "   Network 模块返回: " << networkStock.name << std::endl;
    
    // 模拟 Data 模块接收
    auto dataStock = networkStock;  // data::Stock = core::Stock
    std::cout << "   Data 模块接收: " << dataStock.name << std::endl;
    std::cout << "   ✓ 无需类型转换！" << std::endl;
    
    // 5. 演示数据一致性
    std::cout << "\n5. 数据一致性验证：" << std::endl;
    std::cout << "   原始股票: " << stock.ts_code << std::endl;
    std::cout << "   Network: " << networkStock.ts_code << std::endl;
    std::cout << "   Data: " << dataStock.ts_code << std::endl;
    std::cout << "   Entity: " << entity.stock.ts_code << std::endl;
    std::cout << "   ✓ 所有数据一致！" << std::endl;
    
    std::cout << "\n=== 示例完成 ===" << std::endl;
    
    return 0;
}

