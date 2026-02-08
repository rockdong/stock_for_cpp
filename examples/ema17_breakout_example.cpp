/**
 * @file ema17_breakout_example.cpp
 * @brief EMA17 突破策略使用示例
 */

#include <iostream>
#include <vector>
#include <iomanip>
#include "Core.h"

using namespace core;

/**
 * @brief 打印分析结果
 */
void printResult(const std::optional<AnalysisResult>& result) {
    if (result.has_value()) {
        std::cout << "  ✓ 信号: " << result->label << std::endl;
        std::cout << "    股票: " << result->ts_code << std::endl;
        std::cout << "    日期: " << result->trade_date << std::endl;
        std::cout << "    策略: " << result->strategy_name << std::endl;
        std::cout << "    操作: " << result->opt << std::endl;
        std::cout << "    频率: " << result->freq << std::endl;
    } else {
        std::cout << "  ✗ 无信号" << std::endl;
    }
}

/**
 * @brief 示例 1：基本使用
 */
void example1_basic_usage() {
    std::cout << "\n========== 示例 1：基本使用 ==========" << std::endl;
    
    // 创建策略实例（使用默认参数）
    EMA17BreakoutStrategy strategy;
    
    // 模拟股票数据
    std::vector<StockData> data = {
        {"000001.SZ", "20240101", 10.00, 10.20, 9.80, 10.10, 1000000},
        {"000001.SZ", "20240102", 10.10, 10.30, 10.00, 10.20, 1100000},
        {"000001.SZ", "20240103", 10.20, 10.40, 10.10, 10.30, 1200000},
        {"000001.SZ", "20240104", 10.30, 10.50, 10.20, 10.40, 1300000},
        {"000001.SZ", "20240105", 10.40, 10.60, 10.30, 10.50, 1400000},
        {"000001.SZ", "20240108", 10.50, 10.70, 10.40, 10.45, 1350000},  // 在 EMA17 下方
        {"000001.SZ", "20240109", 10.45, 10.65, 10.35, 10.40, 1300000},  // 在 EMA17 下方
        {"000001.SZ", "20240110", 10.40, 10.80, 10.35, 10.75, 1800000},  // 突破 EMA17！
    };
    
    // 分析
    auto result = strategy.analyze("000001.SZ", data);
    
    std::cout << "分析结果:" << std::endl;
    printResult(result);
}

/**
 * @brief 示例 2：自定义参数
 */
void example2_custom_parameters() {
    std::cout << "\n========== 示例 2：自定义参数 ==========" << std::endl;
    
    // 设置自定义参数
    std::map<std::string, double> params;
    params["ema_period"] = 20;           // 使用 EMA20
    params["min_breakout_pct"] = 1.0;    // 要求至少突破 1%
    
    EMA17BreakoutStrategy strategy(params);
    
    std::cout << "策略参数:" << std::endl;
    std::cout << "  EMA 周期: " << static_cast<int>(params["ema_period"]) << std::endl;
    std::cout << "  最小突破幅度: " << params["min_breakout_pct"] << "%" << std::endl;
    
    // 模拟数据（省略...）
    std::vector<StockData> data;
    // ... 填充数据 ...
    
    std::cout << "\n注意：此示例需要实际数据才能运行" << std::endl;
}

/**
 * @brief 示例 3：多频率分析
 */
void example3_multi_frequency() {
    std::cout << "\n========== 示例 3：多频率分析 ==========" << std::endl;
    
    EMA17BreakoutStrategy strategy;
    
    std::vector<std::string> frequencies = {"d", "w", "m"};
    std::vector<std::string> freqNames = {"日线", "周线", "月线"};
    
    std::cout << "对同一只股票进行多频率分析:" << std::endl;
    
    for (size_t i = 0; i < frequencies.size(); ++i) {
        std::cout << "\n" << freqNames[i] << " 分析:" << std::endl;
        
        // 实际使用中，这里应该获取对应频率的数据
        // auto data = dataSource->getQuoteData("000001.SZ", "", "", frequencies[i]);
        // auto result = strategy.analyze("000001.SZ", data);
        // printResult(result);
        
        std::cout << "  (需要实际数据)" << std::endl;
    }
}

/**
 * @brief 示例 4：与其他策略配合
 */
void example4_combine_strategies() {
    std::cout << "\n========== 示例 4：与其他策略配合 ==========" << std::endl;
    
    // 创建多个策略
    EMA17BreakoutStrategy breakoutStrategy;
    EMA17TO25Strategy crossStrategy;
    RSIStrategy rsiStrategy;
    
    std::cout << "组合策略分析:" << std::endl;
    std::cout << "  1. EMA17 突破策略" << std::endl;
    std::cout << "  2. EMA17/25 交叉策略" << std::endl;
    std::cout << "  3. RSI 策略" << std::endl;
    
    // 实际使用中的逻辑
    std::cout << "\n分析逻辑:" << std::endl;
    std::cout << "  - 如果 EMA17 突破 + EMA17/25 金叉 → 强烈买入信号" << std::endl;
    std::cout << "  - 如果 EMA17 突破 + RSI 未超买 → 可以买入" << std::endl;
    std::cout << "  - 如果 EMA17 突破 + RSI 超买 → 谨慎观望" << std::endl;
}

/**
 * @brief 示例 5：使用 StrategyManager
 */
void example5_strategy_manager() {
    std::cout << "\n========== 示例 5：使用 StrategyManager ==========" << std::endl;
    
    try {
        // 获取策略管理器实例
        auto& manager = StrategyManager::getInstance();
        
        // 初始化策略（包括 EMA17_BREAKOUT）
        manager.initialize("EMA17_BREAKOUT;MACD;RSI");
        
        std::cout << "已注册的策略:" << std::endl;
        auto strategies = manager.getRegisteredStrategies();
        for (const auto& name : strategies) {
            std::cout << "  - " << name << std::endl;
        }
        
        // 批量分析
        std::cout << "\n批量分析示例:" << std::endl;
        std::cout << "  (需要实际数据)" << std::endl;
        
        // 实际使用：
        // auto results = manager.analyzeAll("000001.SZ", stockData);
        // for (const auto& [strategyName, result] : results) {
        //     if (result.has_value()) {
        //         std::cout << strategyName << ": " << result->label << std::endl;
        //     }
        // }
        
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
    }
}

/**
 * @brief 示例 6：参数优化
 */
void example6_parameter_optimization() {
    std::cout << "\n========== 示例 6：参数优化 ==========" << std::endl;
    
    std::cout << "测试不同的 EMA 周期:" << std::endl;
    
    std::vector<int> periods = {10, 17, 20, 30};
    
    for (int period : periods) {
        std::map<std::string, double> params;
        params["ema_period"] = period;
        
        EMA17BreakoutStrategy strategy(params);
        
        std::cout << "\n  EMA" << period << " 策略:" << std::endl;
        std::cout << "    (需要回测数据来评估性能)" << std::endl;
        
        // 实际使用中：
        // - 对历史数据进行回测
        // - 计算胜率、收益率等指标
        // - 选择最优参数
    }
    
    std::cout << "\n测试不同的最小突破幅度:" << std::endl;
    
    std::vector<double> breakoutPcts = {0.0, 0.5, 1.0, 2.0};
    
    for (double pct : breakoutPcts) {
        std::map<std::string, double> params;
        params["min_breakout_pct"] = pct;
        
        EMA17BreakoutStrategy strategy(params);
        
        std::cout << "\n  最小突破幅度 " << pct << "%:" << std::endl;
        std::cout << "    (需要回测数据来评估性能)" << std::endl;
    }
}

/**
 * @brief 示例 7：数据库集成
 */
void example7_database_integration() {
    std::cout << "\n========== 示例 7：数据库集成 ==========" << std::endl;
    
    std::cout << "数据库操作示例:" << std::endl;
    std::cout << "\n1. 保存分析结果:" << std::endl;
    std::cout << "   auto dao = data::AnalysisResultDAO(connection);" << std::endl;
    std::cout << "   dao.insert(result);" << std::endl;
    
    std::cout << "\n2. 查询突破信号:" << std::endl;
    std::cout << "   auto results = dao.findByStrategy(\"EMA17_BREAKOUT\");" << std::endl;
    
    std::cout << "\n3. 查询特定股票:" << std::endl;
    std::cout << "   auto results = dao.findByTsCodeAndStrategy(\"000001.SZ\", \"EMA17_BREAKOUT\");" << std::endl;
    
    std::cout << "\n4. 批量保存:" << std::endl;
    std::cout << "   dao.batchInsert(results);" << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  EMA17 突破策略使用示例" << std::endl;
    std::cout << "========================================" << std::endl;
    
    try {
        example1_basic_usage();
        example2_custom_parameters();
        example3_multi_frequency();
        example4_combine_strategies();
        example5_strategy_manager();
        example6_parameter_optimization();
        example7_database_integration();
        
        std::cout << "\n========================================" << std::endl;
        std::cout << "  所有示例运行完成" << std::endl;
        std::cout << "========================================" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

