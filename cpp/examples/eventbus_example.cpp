#include <iostream>
#include "../eventbus/EventBus.h"

using namespace eventbus;

int main() {
    std::cout << "=== EventBus 使用示例 ===" << std::endl;
    
    // 1. 初始化 EventBus
    if (!initialize()) {
        std::cerr << "EventBus 初始化失败" << std::endl;
        return 1;
    }
    
    auto bus = EventBusManager::getInstance().getCoreBus();
    
    // 2. 创建监听器
    dexode::EventBus::Listener listener{bus};
    
    // 3. 订阅不同类型的事件
    
    // 订阅进度事件
    listener.listen([](const events::ProgressUpdated& event) {
        std::cout << "进度更新: " << event.completed << "/" << event.total 
                  << " (" << event.status << ")" << std::endl;
    });
    
    // 订阅分析完成事件
    listener.listen([](const events::StockAnalysisCompleted& event) {
        if (event.success) {
            std::cout << "✅ 分析完成: " << event.tsCode << " (" << event.stockName << ")" << std::endl;
        } else {
            std::cout << "❌ 分析失败: " << event.tsCode << " - " << event.errorMessage << std::endl;
        }
    });
    
    // 订阅应用生命周期事件
    listener.listen([](const events::AppStarted& event) {
        std::cout << "🚀 应用启动: 版本 " << event.version << std::endl;
    });
    
    listener.listen([](const events::AppStopping& event) {
        std::cout << "🛑 应用停止: " << event.reason << std::endl;
    });
    
    // 4. 发布事件
    std::cout << "\n--- 发布事件 ---" << std::endl;
    
    // 发布应用启动事件
    bus->postpone(events::AppStarted{"1.0.0", "2026-04-06"});
    
    // 发布进度更新事件
    bus->postpone(events::ProgressUpdated{100, 25, 0, "running"});
    bus->postpone(events::ProgressUpdated{100, 50, 2, "running"});
    bus->postpone(events::ProgressUpdated{100, 100, 5, "completed"});
    
    // 发布股票分析完成事件
    bus->postpone(events::StockAnalysisCompleted{"000001.SZ", "平安银行", true, ""});
    bus->postpone(events::StockAnalysisCompleted{"600000.SH", "浦发银行", false, "数据不足"});
    
    // 5. 处理事件（在主线程中）
    std::cout << "\n--- 处理事件 ---" << std::endl;
    bus->process();
    
    // 6. 验证事件数据
    std::cout << "\n--- 事件验证 ---" << std::endl;
    
    events::ProgressUpdated validProgress{100, 50, 0, "running"};
    if (EventValidator::validateProgress(validProgress)) {
        std::cout << "✅ 进度事件验证通过" << std::endl;
    }
    
    events::ProgressUpdated invalidProgress{100, 60, 50, "running"};
    if (!EventValidator::validateProgress(invalidProgress)) {
        std::cout << "❌ 无效进度事件（正确拒绝）" << std::endl;
    }
    
    if (EventValidator::validateStockCode("000001.SZ")) {
        std::cout << "✅ 股票代码验证通过" << std::endl;
    }
    
    if (!EventValidator::validateStockCode("invalid")) {
        std::cout << "❌ 无效股票代码（正确拒绝）" << std::endl;
    }
    
    // 7. 发布应用停止事件
    bus->postpone(events::AppStopping{"示例完成"});
    bus->process();
    
    // 8. 清理
    shutdown();
    
    std::cout << "\n=== 示例运行完成 ===" << std::endl;
    
    return 0;
}