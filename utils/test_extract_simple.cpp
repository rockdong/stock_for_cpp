#include "MathUtil.h"
#include <iostream>
#include <cassert>

// 模拟 network::StockData 结构
struct StockData {
    std::string ts_code;
    std::string trade_date;
    double open;
    double high;
    double low;
    double close;
    double pre_close;
    double change;
    double pct_chg;
    long volume;
    double amount;
};

int main() {
    std::cout << "=== 测试 extractClose 函数 ===" << std::endl;
    
    // 创建测试数据
    std::vector<StockData> stockData = {
        {"000001.SZ", "20240101", 10.0, 10.5, 9.8, 10.2, 10.0, 0.2, 2.0, 1000000, 10200000},
        {"000001.SZ", "20240102", 10.2, 10.8, 10.1, 10.5, 10.2, 0.3, 2.94, 1100000, 11550000},
        {"000001.SZ", "20240103", 10.5, 11.0, 10.3, 10.8, 10.5, 0.3, 2.86, 1200000, 12960000},
        {"000001.SZ", "20240104", 10.8, 11.2, 10.6, 11.0, 10.8, 0.2, 1.85, 1300000, 14300000},
        {"000001.SZ", "20240105", 11.0, 11.5, 10.9, 11.3, 11.0, 0.3, 2.73, 1400000, 15820000}
    };
    
    std::cout << "原始数据：" << std::endl;
    for (const auto& data : stockData) {
        std::cout << "  日期: " << data.trade_date 
                  << ", 收盘价: " << data.close << std::endl;
    }
    
    // 提取收盘价
    auto closes = utils::MathUtil::extractClose(stockData);
    
    std::cout << "\n提取的收盘价：" << std::endl;
    for (size_t i = 0; i < closes.size(); ++i) {
        std::cout << "  [" << i << "] " << closes[i] << std::endl;
    }
    
    // 验证结果
    assert(closes.size() == 5);
    assert(closes[0] == 10.2);
    assert(closes[1] == 10.5);
    assert(closes[2] == 10.8);
    assert(closes[3] == 11.0);
    assert(closes[4] == 11.3);
    
    std::cout << "\n✓ 所有测试通过！" << std::endl;
    
    // 演示与其他函数结合使用
    std::cout << "\n=== 与统计函数结合使用 ===" << std::endl;
    
    double avgPrice = utils::MathUtil::mean(closes);
    double minPrice = utils::MathUtil::min(closes);
    double maxPrice = utils::MathUtil::max(closes);
    double stdPrice = utils::MathUtil::stddev(closes);
    
    std::cout << "平均收盘价: " << avgPrice << std::endl;
    std::cout << "最低收盘价: " << minPrice << std::endl;
    std::cout << "最高收盘价: " << maxPrice << std::endl;
    std::cout << "标准差: " << stdPrice << std::endl;
    
    // 计算收益率
    auto returns = utils::MathUtil::pctChange(closes);
    std::cout << "\n收益率序列：" << std::endl;
    for (size_t i = 0; i < returns.size(); ++i) {
        std::cout << "  [" << i << "] " << returns[i] << "%" << std::endl;
    }
    
    std::cout << "\n✓ 完整测试通过！" << std::endl;
    
    return 0;
}

