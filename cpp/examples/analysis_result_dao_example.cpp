/**
 * @file AnalysisResultDAO 使用示例
 * 
 * 演示如何使用 AnalysisResultDAO 将策略分析结果保存到数据库
 */

#include <iostream>
#include "Data.h"
#include "Core.h"
#include "Logger.h"

void exampleUsage() {
    // 1. 初始化数据库连接
    auto& conn = data::Connection::getInstance();
    if (!conn.initialize("stock.db")) {
        std::cerr << "数据库初始化失败" << std::endl;
        return;
    }
    
    if (!conn.connect()) {
        std::cerr << "数据库连接失败" << std::endl;
        return;
    }
    
    std::cout << "数据库连接成功" << std::endl;
    
    // 2. 创建 AnalysisResultDAO 实例
    data::AnalysisResultDAO analysisDao;
    
    // 3. 创建分析结果
    core::AnalysisResult result1("000001.SZ", "MA_CROSS", "20240101", "买入");
    core::AnalysisResult result2("000001.SZ", "RSI", "20240101", "持有");
    core::AnalysisResult result3("000002.SZ", "MACD", "20240102", "卖出");
    
    // 4. 插入单条记录
    if (analysisDao.insert(result1)) {
        std::cout << "插入分析结果成功: " << result1.toString() << std::endl;
    }
    
    // 5. 批量插入
    std::vector<core::AnalysisResult> results = {result2, result3};
    int count = analysisDao.batchInsert(results);
    std::cout << "批量插入成功: " << count << " 条记录" << std::endl;
    
    // 6. 查询所有记录
    auto allResults = analysisDao.findAll();
    std::cout << "\n所有分析结果 (" << allResults.size() << " 条):" << std::endl;
    for (const auto& r : allResults) {
        std::cout << "  " << r.toString() << std::endl;
    }
    
    // 7. 根据股票代码查询
    auto stockResults = analysisDao.findByTsCode("000001.SZ");
    std::cout << "\n股票 000001.SZ 的分析结果 (" << stockResults.size() << " 条):" << std::endl;
    for (const auto& r : stockResults) {
        std::cout << "  " << r.toString() << std::endl;
    }
    
    // 8. 根据策略名称查询
    auto strategyResults = analysisDao.findByStrategy("MA_CROSS");
    std::cout << "\nMA_CROSS 策略的分析结果 (" << strategyResults.size() << " 条):" << std::endl;
    for (const auto& r : strategyResults) {
        std::cout << "  " << r.toString() << std::endl;
    }
    
    // 9. 根据标签查询
    auto buyResults = analysisDao.findByLabel("买入");
    std::cout << "\n标签为'买入'的分析结果 (" << buyResults.size() << " 条):" << std::endl;
    for (const auto& r : buyResults) {
        std::cout << "  " << r.toString() << std::endl;
    }
    
    // 10. 根据股票代码和策略查询
    auto specificResults = analysisDao.findByTsCodeAndStrategy("000001.SZ", "MA_CROSS");
    std::cout << "\n股票 000001.SZ 使用 MA_CROSS 策略的结果 (" << specificResults.size() << " 条):" << std::endl;
    for (const auto& r : specificResults) {
        std::cout << "  " << r.toString() << std::endl;
    }
    
    // 11. 统计
    int totalCount = analysisDao.count();
    int stock1Count = analysisDao.countByTsCode("000001.SZ");
    std::cout << "\n统计信息:" << std::endl;
    std::cout << "  总记录数: " << totalCount << std::endl;
    std::cout << "  000001.SZ 记录数: " << stock1Count << std::endl;
    
    // 12. 断开连接
    conn.disconnect();
    std::cout << "\n数据库连接已断开" << std::endl;
}

// 在策略分析中使用的示例
void strategyAnalysisExample() {
    // 初始化数据库
    auto& conn = data::Connection::getInstance();
    conn.initialize("stock.db");
    conn.connect();
    
    // 创建 DAO
    data::AnalysisResultDAO analysisDao;
    
    // 假设我们有股票数据
    std::vector<core::StockData> stockData; // 从数据源获取
    
    // 使用策略分析
    core::EMA17TO25Strategy strategy;
    auto result = strategy.analyze("000001.SZ", stockData);
    
    // 保存分析结果到数据库
    if (analysisDao.insert(result)) {
        std::cout << "分析结果已保存: " << result.toString() << std::endl;
    }
    
    conn.disconnect();
}

int main() {
    // 初始化日志系统
    logger::init();
    
    std::cout << "========================================" << std::endl;
    std::cout << "AnalysisResultDAO 使用示例" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    exampleUsage();
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "示例结束" << std::endl;
    std::cout << "========================================" << std::endl;
    
    return 0;
}

