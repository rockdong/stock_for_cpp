/**
 * @file ema_convergence_example.cpp
 * @brief EMA 收敛策略使用示例
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
        std::cout << "  ✗ 无信号（未收敛）" << std::endl;
    }
}

/**
 * @brief 示例 1：基本使用（绝对值模式）
 */
void example1_basic_usage() {
    std::cout << "\n========== 示例 1：基本使用（绝对值模式）==========" << std::endl;
    
    // 创建策略实例（使用默认参数）
    EMAConvergenceStrategy strategy;
    
    std::cout << "默认参数:" << std::endl;
    std::cout << "  EMA 短期周期: 17" << std::endl;
    std::cout << "  EMA 长期周期: 25" << std::endl;
    std::cout << "  收敛阈值: 0.03" << std::endl;
    std::cout << "  模式: 绝对值" << std::endl;
    
    // 模拟股票数据
    std::vector<StockData> data = {
        {"000001.SZ", "20240101", 10.00, 10.20, 9.80, 10.10, 1000000},
        {"000001.SZ", "20240102", 10.10, 10.30, 10.00, 10.20, 1100000},
        {"000001.SZ", "20240103", 10.20, 10.40, 10.10, 10.30, 1200000},
        {"000001.SZ", "20240104", 10.30, 10.50, 10.20, 10.40, 1300000},
        {"000001.SZ", "20240105", 10.40, 10.60, 10.30, 10.50, 1400000},
        // ... 需要更多数据来计算 EMA25
    };
    
    std::cout << "\n注意：此示例需要至少 26 天的数据才能运行" << std::endl;
    std::cout << "实际使用时，请提供完整的历史数据" << std::endl;
}

/**
 * @brief 示例 2：绝对值模式（自定义阈值）
 */
void example2_absolute_mode() {
    std::cout << "\n========== 示例 2：绝对值模式（自定义阈值）==========" << std::endl;
    
    // 设置自定义参数
    std::map<std::string, double> params;
    params["ema_short_period"] = 17;
    params["ema_long_period"] = 25;
    params["convergence_threshold"] = 0.05;  // 阈值 0.05
    params["use_percentage"] = 0.0;          // 使用绝对值
    
    EMAConvergenceStrategy strategy(params);
    
    std::cout << "自定义参数:" << std::endl;
    std::cout << "  收敛阈值: 0.05（绝对值）" << std::endl;
    std::cout << "\n含义：当 |EMA17 - EMA25| <= 0.05 时产生信号" << std::endl;
    
    std::cout << "\n示例场景:" << std::endl;
    std::cout << "  EMA17 = 10.30, EMA25 = 10.32" << std::endl;
    std::cout << "  差值 = |10.30 - 10.32| = 0.02" << std::endl;
    std::cout << "  0.02 <= 0.05 ✓ 产生信号" << std::endl;
}

/**
 * @brief 示例 3：百分比模式
 */
void example3_percentage_mode() {
    std::cout << "\n========== 示例 3：百分比模式 ==========" << std::endl;
    
    // 使用百分比模式
    std::map<std::string, double> params;
    params["ema_short_period"] = 17;
    params["ema_long_period"] = 25;
    params["convergence_threshold"] = 1.0;   // 阈值 1%
    params["use_percentage"] = 1.0;          // 使用百分比
    
    EMAConvergenceStrategy strategy(params);
    
    std::cout << "百分比模式参数:" << std::endl;
    std::cout << "  收敛阈值: 1.0%（百分比）" << std::endl;
    std::cout << "\n含义：当 |EMA17 - EMA25| / EMA25 * 100 <= 1.0 时产生信号" << std::endl;
    
    std::cout << "\n示例场景:" << std::endl;
    std::cout << "  EMA17 = 10.30, EMA25 = 10.40" << std::endl;
    std::cout << "  差值% = |10.30 - 10.40| / 10.40 * 100 = 0.96%" << std::endl;
    std::cout << "  0.96% <= 1.0% ✓ 产生信号" << std::endl;
    
    std::cout << "\n优势：适合不同价格区间的股票" << std::endl;
    std::cout << "  - 低价股（5元）：阈值 = 0.05元" << std::endl;
    std::cout << "  - 高价股（100元）：阈值 = 1.00元" << std::endl;
}

/**
 * @brief 示例 4：趋势判断
 */
void example4_trend_detection() {
    std::cout << "\n========== 示例 4：趋势判断 ==========" << std::endl;
    
    EMAConvergenceStrategy strategy;
    
    std::cout << "收敛时的趋势判断:" << std::endl;
    
    std::cout << "\n场景 A：EMA17 > EMA25（看涨）" << std::endl;
    std::cout << "  EMA17 = 10.32, EMA25 = 10.30" << std::endl;
    std::cout << "  差值 = 0.02 <= 0.03 ✓" << std::endl;
    std::cout << "  信号: 买入（短期均线在上方，多头排列）" << std::endl;
    
    std::cout << "\n场景 B：EMA17 < EMA25（看跌）" << std::endl;
    std::cout << "  EMA17 = 10.28, EMA25 = 10.30" << std::endl;
    std::cout << "  差值 = 0.02 <= 0.03 ✓" << std::endl;
    std::cout << "  信号: 观望（短期均线在下方，空头排列）" << std::endl;
    
    std::cout << "\n场景 C：EMA17 = EMA25（交叉点）" << std::endl;
    std::cout << "  EMA17 = 10.30, EMA25 = 10.30" << std::endl;
    std::cout << "  差值 = 0.00 <= 0.03 ✓" << std::endl;
    std::cout << "  信号: 观望（等待方向明确）" << std::endl;
}

/**
 * @brief 示例 5：与交叉策略配合
 */
void example5_combine_with_cross() {
    std::cout << "\n========== 示例 5：与交叉策略配合 ==========" << std::endl;
    
    EMAConvergenceStrategy convergenceStrategy;
    EMA17TO25Strategy crossStrategy;
    
    std::cout << "组合使用逻辑:" << std::endl;
    std::cout << "\n第一步：收敛策略预警" << std::endl;
    std::cout << "  - 检测到 EMA17 和 EMA25 接近" << std::endl;
    std::cout << "  - 产生预警信号" << std::endl;
    std::cout << "  - 准备入场" << std::endl;
    
    std::cout << "\n第二步：交叉策略确认" << std::endl;
    std::cout << "  - 检测到金叉发生" << std::endl;
    std::cout << "  - 确认买入信号" << std::endl;
    std::cout << "  - 执行买入" << std::endl;
    
    std::cout << "\n代码示例:" << std::endl;
    std::cout << "  auto convergence = convergenceStrategy.analyze(tsCode, data);" << std::endl;
    std::cout << "  if (convergence && convergence->label == \"买入\") {" << std::endl;
    std::cout << "      std::cout << \"预警：均线即将金叉\" << std::endl;" << std::endl;
    std::cout << "  }" << std::endl;
    std::cout << "  " << std::endl;
    std::cout << "  auto cross = crossStrategy.analyze(tsCode, data);" << std::endl;
    std::cout << "  if (cross && cross->label == \"买入\") {" << std::endl;
    std::cout << "      std::cout << \"确认：金叉发生，买入！\" << std::endl;" << std::endl;
    std::cout << "  }" << std::endl;
}

/**
 * @brief 示例 6：参数优化
 */
void example6_parameter_optimization() {
    std::cout << "\n========== 示例 6：参数优化 ==========" << std::endl;
    
    std::cout << "优化 1：测试不同的阈值（绝对值模式）" << std::endl;
    std::vector<double> thresholds = {0.01, 0.03, 0.05, 0.10};
    
    for (double threshold : thresholds) {
        std::map<std::string, double> params;
        params["convergence_threshold"] = threshold;
        params["use_percentage"] = 0.0;
        
        EMAConvergenceStrategy strategy(params);
        
        std::cout << "  阈值 " << threshold << ": ";
        std::cout << "(需要回测数据来评估)" << std::endl;
    }
    
    std::cout << "\n优化 2：测试不同的阈值（百分比模式）" << std::endl;
    std::vector<double> pctThresholds = {0.5, 1.0, 2.0, 3.0};
    
    for (double threshold : pctThresholds) {
        std::map<std::string, double> params;
        params["convergence_threshold"] = threshold;
        params["use_percentage"] = 1.0;
        
        EMAConvergenceStrategy strategy(params);
        
        std::cout << "  阈值 " << threshold << "%: ";
        std::cout << "(需要回测数据来评估)" << std::endl;
    }
    
    std::cout << "\n优化 3：测试不同的 EMA 周期组合" << std::endl;
    std::vector<std::pair<int, int>> periods = {
        {10, 20}, {12, 26}, {17, 25}, {20, 30}
    };
    
    for (const auto& [short_period, long_period] : periods) {
        std::map<std::string, double> params;
        params["ema_short_period"] = short_period;
        params["ema_long_period"] = long_period;
        
        EMAConvergenceStrategy strategy(params);
        
        std::cout << "  EMA" << short_period << "/EMA" << long_period << ": ";
        std::cout << "(需要回测数据来评估)" << std::endl;
    }
}

/**
 * @brief 示例 7：使用 StrategyManager
 */
void example7_strategy_manager() {
    std::cout << "\n========== 示例 7：使用 StrategyManager ==========" << std::endl;
    
    try {
        // 获取策略管理器实例
        auto& manager = StrategyManager::getInstance();
        
        // 初始化策略（包括 EMA_CONVERGENCE）
        manager.initialize("EMA_CONVERGENCE;EMA17TO25;MACD");
        
        std::cout << "已注册的策略:" << std::endl;
        auto strategies = manager.getRegisteredStrategies();
        for (const auto& name : strategies) {
            std::cout << "  - " << name << std::endl;
        }
        
        std::cout << "\n批量分析示例:" << std::endl;
        std::cout << "  auto results = manager.analyzeAll(\"000001.SZ\", stockData);" << std::endl;
        std::cout << "  " << std::endl;
        std::cout << "  for (const auto& [name, result] : results) {" << std::endl;
        std::cout << "      if (result.has_value()) {" << std::endl;
        std::cout << "          std::cout << name << \": \" << result->label << std::endl;" << std::endl;
        std::cout << "      }" << std::endl;
        std::cout << "  }" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
    }
}

/**
 * @brief 示例 8：实战场景
 */
void example8_real_world_scenarios() {
    std::cout << "\n========== 示例 8：实战场景 ==========" << std::endl;
    
    std::cout << "场景 1：预测金叉" << std::endl;
    std::cout << "  时间线:" << std::endl;
    std::cout << "    T-2: EMA17 < EMA25，差值 0.10（未收敛）" << std::endl;
    std::cout << "    T-1: EMA17 < EMA25，差值 0.05（未收敛）" << std::endl;
    std::cout << "    T0:  EMA17 < EMA25，差值 0.02（收敛）✓ 产生观望信号" << std::endl;
    std::cout << "    T+1: EMA17 > EMA25（金叉发生）" << std::endl;
    std::cout << "  策略价值：在 T0 提前预警，为入场做准备" << std::endl;
    
    std::cout << "\n场景 2：趋势确认" << std::endl;
    std::cout << "  当前状态:" << std::endl;
    std::cout << "    - EMA17 > EMA25（多头排列）" << std::endl;
    std::cout << "    - 差值 0.02（收敛）" << std::endl;
    std::cout << "  信号：买入" << std::endl;
    std::cout << "  含义：多头趋势中的短暂调整，可能是加仓机会" << std::endl;
    
    std::cout << "\n场景 3：避免假信号" << std::endl;
    std::cout << "  震荡市场:" << std::endl;
    std::cout << "    - 均线频繁收敛和发散" << std::endl;
    std::cout << "    - 建议：提高阈值或结合其他指标" << std::endl;
    std::cout << "    - 例如：使用 RSI 过滤超买超卖" << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  EMA 收敛策略使用示例" << std::endl;
    std::cout << "========================================" << std::endl;
    
    try {
        example1_basic_usage();
        example2_absolute_mode();
        example3_percentage_mode();
        example4_trend_detection();
        example5_combine_with_cross();
        example6_parameter_optimization();
        example7_strategy_manager();
        example8_real_world_scenarios();
        
        std::cout << "\n========================================" << std::endl;
        std::cout << "  所有示例运行完成" << std::endl;
        std::cout << "========================================" << std::endl;
        
        std::cout << "\n关键要点:" << std::endl;
        std::cout << "  1. 收敛策略用于预警，不作为唯一依据" << std::endl;
        std::cout << "  2. 建议结合交叉策略确认信号" << std::endl;
        std::cout << "  3. 根据股票特性选择绝对值或百分比模式" << std::endl;
        std::cout << "  4. 通过回测优化阈值参数" << std::endl;
        std::cout << "  5. 注意市场环境，震荡市场需谨慎" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

