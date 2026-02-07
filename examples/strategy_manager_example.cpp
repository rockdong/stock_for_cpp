/**
 * @file StrategyManager 使用示例
 * 
 * 演示如何使用 StrategyManager 从配置文件中加载和管理策略
 */

#include <iostream>
#include "Core.h"
#include "Config.h"
#include "Logger.h"
#include "Data.h"

void basicUsageExample() {
    std::cout << "\n========== 基本使用示例 ==========" << std::endl;
    
    // 1. 从配置字符串初始化
    auto& manager = core::StrategyManager::getInstance();
    
    if (manager.initialize("EMA17TO25;MACD;RSI;BOLL")) {
        std::cout << "策略管理器初始化成功" << std::endl;
    }
    
    // 2. 查看已注册的策略
    std::cout << "\n已注册的策略 (" << manager.count() << " 个):" << std::endl;
    for (const auto& name : manager.getStrategyNames()) {
        std::cout << "  - " << name << std::endl;
    }
    
    // 3. 获取单个策略
    auto emaStrategy = manager.getStrategy("EMA17TO25");
    if (emaStrategy) {
        std::cout << "\n获取策略: " << emaStrategy->getName() 
                  << " - " << emaStrategy->getDescription() << std::endl;
    }
    
    // 4. 检查策略是否存在
    if (manager.hasStrategy("MACD")) {
        std::cout << "MACD 策略已注册" << std::endl;
    }
}

void envConfigExample() {
    std::cout << "\n========== 从环境变量加载示例 ==========" << std::endl;
    
    // 1. 初始化配置系统
    auto& config = config::Config::getInstance();
    config.initialize(".env");
    
    // 2. 从环境变量加载策略
    auto& manager = core::StrategyManager::getInstance();
    manager.clear(); // 清除之前的策略
    
    if (manager.initializeFromEnv("STRATEGIES")) {
        std::cout << "从环境变量加载策略成功" << std::endl;
    }
    
    // 3. 显示配置的策略
    std::cout << "\n配置的策略: " << config.getStrategies() << std::endl;
    std::cout << "已注册策略数量: " << manager.count() << std::endl;
}

void analyzeAllExample() {
    std::cout << "\n========== 批量分析示例 ==========" << std::endl;
    
    // 初始化
    auto& config = config::Config::getInstance();
    config.initialize(".env");
    
    auto& manager = core::StrategyManager::getInstance();
    manager.clear();
    manager.initialize("EMA17TO25;MACD;RSI");
    
    // 模拟股票数据（实际应从数据源获取）
    std::vector<core::StockData> stockData;
    // ... 填充数据 ...
    
    if (stockData.empty()) {
        std::cout << "注意：需要实际的股票数据才能进行分析" << std::endl;
        return;
    }
    
    // 使用所有策略分析
    auto results = manager.analyzeAll("000001.SZ", stockData);
    
    std::cout << "\n分析结果:" << std::endl;
    for (const auto& pair : results) {
        const auto& strategyName = pair.first;
        const auto& result = pair.second;
        
        if (result.has_value()) {
            std::cout << "  " << strategyName << ": " << result->label << std::endl;
        } else {
            std::cout << "  " << strategyName << ": 数据不足" << std::endl;
        }
    }
}

void databaseIntegrationExample() {
    std::cout << "\n========== 数据库集成示例 ==========" << std::endl;
    
    // 1. 初始化系统
    logger::init();
    
    auto& config = config::Config::getInstance();
    config.initialize(".env");
    
    auto& conn = data::Connection::getInstance();
    conn.initialize("stock.db");
    conn.connect();
    
    // 2. 初始化策略管理器
    auto& manager = core::StrategyManager::getInstance();
    manager.clear();
    manager.initializeFromEnv("STRATEGIES");
    
    // 3. 创建 DAO
    data::AnalysisResultDAO analysisDao;
    data::StockDAO stockDao;
    
    // 4. 获取股票列表
    auto stocks = stockDao.findAll();
    
    if (stocks.empty()) {
        std::cout << "数据库中没有股票数据" << std::endl;
        conn.disconnect();
        return;
    }
    
    std::cout << "开始分析 " << stocks.size() << " 只股票..." << std::endl;
    
    // 5. 对每只股票使用所有策略进行分析
    int totalAnalysis = 0;
    int successfulAnalysis = 0;
    
    for (const auto& stock : stocks) {
        // 获取股票数据（这里需要实际实现）
        std::vector<core::StockData> data;
        // data = getStockData(stock.ts_code);
        
        if (data.empty()) {
            continue;
        }
        
        // 使用所有策略分析
        auto results = manager.analyzeAll(stock.ts_code, data);
        
        // 保存所有有效结果
        for (const auto& pair : results) {
            totalAnalysis++;
            const auto& result = pair.second;
            
            if (result.has_value()) {
                if (analysisDao.insert(*result)) {
                    successfulAnalysis++;
                }
            }
        }
        
        std::cout << "完成股票分析: " << stock.ts_code << " (" << stock.name << ")" << std::endl;
    }
    
    std::cout << "\n分析完成: " << successfulAnalysis << "/" << totalAnalysis << " 条结果已保存" << std::endl;
    
    // 6. 查询买入信号
    auto buySignals = analysisDao.findByLabel("买入");
    std::cout << "发现 " << buySignals.size() << " 个买入信号" << std::endl;
    
    // 7. 断开连接
    conn.disconnect();
}

void manualRegistrationExample() {
    std::cout << "\n========== 手动注册策略示例 ==========" << std::endl;
    
    auto& manager = core::StrategyManager::getInstance();
    manager.clear();
    
    // 手动创建和注册策略
    auto emaStrategy = std::make_shared<core::EMA17TO25Strategy>();
    auto macdStrategy = std::make_shared<core::MACDStrategy>();
    
    manager.registerStrategy(emaStrategy);
    manager.registerStrategy(macdStrategy);
    
    std::cout << "手动注册了 " << manager.count() << " 个策略" << std::endl;
    
    for (const auto& name : manager.getStrategyNames()) {
        std::cout << "  - " << name << std::endl;
    }
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "StrategyManager 使用示例" << std::endl;
    std::cout << "========================================" << std::endl;
    
    // 示例 1: 基本使用
    basicUsageExample();
    
    // 示例 2: 从环境变量加载
    envConfigExample();
    
    // 示例 3: 批量分析
    analyzeAllExample();
    
    // 示例 4: 数据库集成
    // databaseIntegrationExample();  // 需要实际的数据库和股票数据
    
    // 示例 5: 手动注册
    manualRegistrationExample();
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "示例结束" << std::endl;
    std::cout << "========================================" << std::endl;
    
    return 0;
}

