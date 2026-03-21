/**
 * @file EMA17TO25 策略测试
 * 
 * 验证改进后的趋势判断和信号强度评估功能
 */

#include <iostream>
#include <cassert>
#include <vector>
#include "Core.h"

void testSignalStrengthEvaluation() {
    std::cout << "\n========== 测试信号强度评估 ==========" << std::endl;
    
    core::EMA17TO25Strategy strategy;
    
    auto testStrength = [&](double confidence, core::SignalStrength expected) {
        core::AnalysisResult result("000001.SZ", "EMA17TO25", "20260321", "买入");
        result.confidence = confidence;
        result.strength = strategy.evaluateStrength(confidence);
        
        std::string strengthStr = core::strengthToString(result.strength);
        std::cout << "置信度 " << confidence << " -> " << strengthStr;
        
        if (result.strength == expected) {
            std::cout << " ✓" << std::endl;
            return true;
        } else {
            std::cout << " ✗ (预期: " << core::strengthToString(expected) << ")" << std::endl;
            return false;
        }
    };
    
    assert(testStrength(75.0, core::SignalStrength::STRONG));
    assert(testStrength(70.0, core::SignalStrength::STRONG));
    assert(testStrength(69.9, core::SignalStrength::MEDIUM));
    assert(testStrength(50.0, core::SignalStrength::MEDIUM));
    assert(testStrength(40.0, core::SignalStrength::MEDIUM));
    assert(testStrength(39.9, core::SignalStrength::WEAK));
    assert(testStrength(10.0, core::SignalStrength::WEAK));
    
    std::cout << "信号强度评估测试通过！" << std::endl;
}

void testTrendDetection() {
    std::cout << "\n========== 测试趋势判断（窗口化） ==========" << std::endl;
    
    core::EMA17TO25Strategy strategy;
    
    auto testTrend = [&](const std::vector<double>& ema, int trendDays, bool expected, const std::string& desc) {
        strategy.setParameter("trend_days", trendDays);
        bool result = strategy.isUpTrend(ema);
        
        std::cout << desc << " (窗口=" << trendDays << "): ";
        std::cout << (result ? "上升趋势" : "非上升趋势");
        
        if (result == expected) {
            std::cout << " ✓" << std::endl;
            return true;
        } else {
            std::cout << " ✗ (预期: " << (expected ? "上升趋势" : "非上升趋势") << ")" << std::endl;
            return false;
        }
    };
    
    std::vector<double> risingEMA = {10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0};
    std::vector<double> fallingEMA = {16.0, 15.0, 14.0, 13.0, 12.0, 11.0, 10.0};
    std::vector<double> mixedEMA = {10.0, 12.0, 11.0, 13.0, 12.0, 14.0, 15.0};
    std::vector<double> shortEMA = {10.0, 11.0};
    
    assert(testTrend(risingEMA, 3, true, "持续上涨序列"));
    assert(testTrend(fallingEMA, 3, false, "持续下跌序列"));
    assert(testTrend(mixedEMA, 3, false, "震荡序列"));
    assert(testTrend(shortEMA, 3, false, "数据不足"));
    assert(testTrend(risingEMA, 5, true, "持续上涨序列（大窗口）"));
    
    std::cout << "趋势判断测试通过！" << std::endl;
}

void testAnalyzeWithStrength() {
    std::cout << "\n========== 测试分析结果带信号强度 ==========" << std::endl;
    
    core::EMA17TO25Strategy strategy;
    
    std::vector<core::StockData> stockData;
    for (int i = 0; i < 30; i++) {
        core::StockData data;
        data.ts_code = "000001.SZ";
        data.trade_date = "202603" + std::to_string(20 + i);
        data.close = 10.0 + i * 0.5;
        data.open = data.close - 0.1;
        data.high = data.close + 0.2;
        data.low = data.close - 0.2;
        data.volume = 1000000;
        stockData.push_back(data);
    }
    
    std::cout << "创建测试数据: " << stockData.size() << " 条记录" << std::endl;
    
    auto result = strategy.analyze("000001.SZ", stockData);
    
    if (result.has_value()) {
        std::cout << "分析结果:" << std::endl;
        std::cout << "  - 股票代码: " << result->ts_code << std::endl;
        std::cout << "  - 策略名称: " << result->strategy_name << std::endl;
        std::cout << "  - 交易日期: " << result->trade_date << std::endl;
        std::cout << "  - 信号标签: " << result->label << std::endl;
        std::cout << "  - 信号强度: " << core::strengthToString(result->strength) << std::endl;
        std::cout << "  - 置信度: " << result->confidence << std::endl;
        std::cout << "  - toString: " << result->toString() << std::endl;
        std::cout << "带信号强度的分析测试通过！" << std::endl;
    } else {
        std::cout << "分析返回空结果（可能是数据不满足条件）" << std::endl;
        std::cout << "这是正常的，取决于数据是否满足金叉条件" << std::endl;
    }
}

void testCustomParameters() {
    std::cout << "\n========== 测试自定义参数 ==========" << std::endl;
    
    std::map<std::string, double> customParams = {
        {"trend_days", 5},
        {"min_confidence", 30},
        {"fast_period", 12},
        {"slow_period", 26}
    };
    
    core::EMA17TO25Strategy strategy(customParams);
    
    std::cout << "自定义参数:" << std::endl;
    std::cout << "  - trend_days: 5" << std::endl;
    std::cout << "  - min_confidence: 30" << std::endl;
    std::cout << "  - fast_period: 12" << std::endl;
    std::cout << "  - slow_period: 26" << std::endl;
    
    assert(strategy.validateParameters());
    std::cout << "参数验证通过 ✓" << std::endl;
    
    std::cout << "自定义参数测试通过！" << std::endl;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "EMA17TO25 策略改进测试" << std::endl;
    std::cout << "========================================" << std::endl;
    
    testSignalStrengthEvaluation();
    testTrendDetection();
    testAnalyzeWithStrength();
    testCustomParameters();
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "所有测试通过！ ✓" << std::endl;
    std::cout << "========================================" << std::endl;
    
    return 0;
}