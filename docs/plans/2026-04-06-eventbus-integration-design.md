# EventBus 集成设计文档

**版本**: 1.0  
**日期**: 2026-04-06  
**作者**: Development Team  
**状态**: Draft

---

## 📋 目录

1. [概述](#概述)
2. [目标与动机](#目标与动机)
3. [方案分析](#方案分析)
4. [详细设计](#详细设计)
5. [实施计划](#实施计划)
6. [测试策略](#测试策略)
7. [性能考虑](#性能考虑)
8. [风险评估](#风险评估)

---

## 概述

### 背景

股票分析系统（stock_for_cpp）当前架构：
- ✅ C++17 实现，高性能分析引擎
- ✅ 多种技术策略（EMA、MACD、RSI、布林带等）
- ✅ 使用 ThreadPool 进行并发处理
- ✅ Scheduler 定时调度器
- ✅ 完善的日志、配置、网络、数据、分析模块

### 当前问题

1. **模块间通信依赖直接调用** - 缺乏统一的事件通知机制
2. **回调函数耦合度高** - ThreadPool 和 Scheduler 使用回调函数，耦合度高
3. **状态通知分散** - 进度更新、错误处理等分散在各处
4. **扩展困难** - 添加新的事件处理需要修改多处代码

### EventBus 库

**gelldur/EventBus** - C++17 轻量级事件总线

**特点：**
- ✅ 高性能（比 CCNotificationCenter 快 40倍+）
- ✅ 强类型事件
- ✅ 支持 lambda/std::bind 订阅
- ✅ postpone 模式（延迟处理）
- ✅ 线程安全
- ✅ Apache 2.0 许可证

**GitHub**: https://github.com/gelldur/EventBus

---

## 目标与动机

### 主要目标

1. **解耦模块间依赖** - 通过事件机制降低模块间耦合度
2. **统一事件通知** - 提供标准化的事件发布/订阅模式
3. **异步事件处理** - 支持 postpone 模式，批量处理事件
4. **提高扩展性** - 新增事件处理无需修改现有代码

### 成功标准

- ✅ 模块间通信通过 EventBus 完成
- ✅ ThreadPool 任务完成通过事件通知
- ✅ Scheduler 状态变更通过事件通知
- ✅ 进度更新统一通过 EventBus
- ✅ 系统性能无明显下降（< 5%）
- ✅ 单元测试覆盖率 > 80%

---

## 方案分析

### 方案对比

我们分析了三种集成方案：

| 方案 | 描述 | 优点 | 缺点 | 适用性 |
|------|------|------|------|--------|
| **方案1** | 全局 EventBus 单例 | 实现简单、易于调试 | 全局依赖、测试困难 | ⭐⭐ |
| **方案2** | 局部 EventBus 模块化 | 高度模块化、易于测试 | 实现复杂、资源占用多 | ⭐⭐ |
| **方案3** | 混合模式 | 平衡灵活性和统一性 | 需要设计事件分类规则 | ⭐⭐⭐⭐⭐ |

### 推荐方案：方案3（混合模式）

**架构图：**

```
┌─────────────────────────────────────────────┐
│          核心 EventBus (全局单例)             │
│      EventBusManager::getInstance()         │
│  - 系统级事件                                │
│  - 跨模块通信                                │
│  - 进度通知                                  │
└─────────────────────────────────────────────┘
                     ↑
                     │ 订阅/发布
    ┌────────────────┼────────────────┐
    │                │                │
┌───▼──────┐    ┌───▼──────┐    ┌───▼──────┐
│Scheduler │    │ThreadPool│    │ Core层   │
│          │    │          │    │          │
│ 可选内部  │    │ 内部     │    │ 可选内部 │
│ EventBus │    │ EventBus │    │ EventBus │
│(任务调度) │    │(任务完成) │    │(策略事件) │
└──────────┘    └──────────┘    └──────────┘
```

**推荐理由：**

1. 符合现有架构风格（单例模式）
2. 满足实际需求（跨模块通知）
3. 易于实施和测试
4. 渐进式改造（先核心 EventBus，再模块级）

---

## 详细设计

### 核心 EventBus 管理

#### EventBusManager.h

```cpp
#pragma once

#include <memory>
#include "EventBus.hpp"  // gelldur/EventBus

namespace eventbus {

/**
 * @brief EventBus 管理器（单例模式）
 * 
 * 提供全局 EventBus 实例，用于跨模块事件通信。
 * 符合现有架构风格（Config::getInstance, Connection::getInstance）
 */
class EventBusManager {
public:
    /**
     * @brief 获取单例实例
     */
    static EventBusManager& getInstance();
    
    /**
     * @brief 获取核心 EventBus
     * @return EventBus 实例（shared_ptr）
     */
    std::shared_ptr<dexode::EventBus> getCoreBus();
    
    /**
     * @brief 处理事件队列中的所有事件
     * 
     * 调用 postpone() 发布的事件在此处理
     * 应在主线程定期调用（如每秒一次）
     */
    void process();
    
private:
    EventBusManager();
    ~EventBusManager();
    
    // 禁止拷贝和赋值
    EventBusManager(const EventBusManager&) = delete;
    EventBusManager& operator=(const EventBusManager&) = delete;
    
    std::shared_ptr<dexode::EventBus> coreBus_;
};

} // namespace eventbus
```

#### EventBusManager.cpp

```cpp
#include "EventBusManager.h"
#include "Logger.h"
#include <stdexcept>

namespace eventbus {

EventBusManager& EventBusManager::getInstance() {
    static EventBusManager instance;
    return instance;
}

EventBusManager::EventBusManager() {
    try {
        coreBus_ = std::make_shared<dexode::EventBus>();
        LOG_INFO("EventBus 初始化成功");
    } catch (const std::exception& e) {
        LOG_ERROR("EventBus 初始化失败: " + std::string(e.what()));
        throw std::runtime_error("EventBus initialization failed");
    }
}

EventBusManager::~EventBusManager() {
    LOG_INFO("EventBus 销毁");
}

std::shared_ptr<dexode::EventBus> EventBusManager::getCoreBus() {
    return coreBus_;
}

void EventBusManager::process() {
    try {
        coreBus_->process();
    } catch (const std::exception& e) {
        LOG_ERROR("EventBus 处理失败: " + std::string(e.what()));
        // 不抛出异常，继续运行
    }
}

} // namespace eventbus
```

---

### 事件定义

#### SystemEvents.h

```cpp
#pragma once

#include <string>

namespace eventbus::events {

/**
 * @brief 系统生命周期事件
 */
struct AppStarted {
    std::string version;
    std::string buildDate;
};

struct AppStopping {
    std::string reason;  // "User requested", "Signal received", "Error"
};

/**
 * @brief 进度更新事件
 */
struct ProgressUpdated {
    int total;           // 总任务数
    int completed;       // 已完成数
    int failed;          // 失败数
    std::string status;  // "running", "completed", "failed"
};

/**
 * @brief 股票分析事件
 */
struct StockAnalysisStarted {
    std::string tsCode;
    std::string stockName;
};

struct StockAnalysisCompleted {
    std::string tsCode;
    std::string stockName;
    bool success;
    std::string errorMessage;  // 仅当 success=false 时有效
};

struct StockAnalysisFailed {
    std::string tsCode;
    std::string stockName;
    std::string error;
};

/**
 * @brief 策略信号事件
 */
struct StrategySignalGenerated {
    std::string tsCode;
    std::string strategyName;
    std::string signal;      // "buy", "sell", "hold"
    double confidence;       // 置信度 0.0-1.0
    std::string freq;        // "d", "w", "m"
};

/**
 * @brief 数据获取事件
 */
struct DataFetchCompleted {
    std::string tsCode;
    std::string freq;
    int dataCount;
};

struct DataFetchFailed {
    std::string tsCode;
    std::string freq;
    std::string error;
};

} // namespace eventbus::events
```

---

### 模块集成

#### ThreadPool 集成

**修改点：** `utils/ThreadPool.cpp`

```cpp
#include "../eventbus/EventBusManager.h"
#include "../eventbus/events/SystemEvents.h"

// 在任务执行完成后发布事件
void ThreadPool::enqueue(/* ... */) {
    // ... 执行任务
    
    if (success) {
        auto bus = eventbus::EventBusManager::getInstance().getCoreBus();
        bus->postpone(eventbus::events::StockAnalysisCompleted{
            stock.ts_code, stock.name, true, ""
        });
    } else {
        bus->postpone(eventbus::events::StockAnalysisFailed{
            stock.ts_code, stock.name, error
        });
    }
}
```

**关键改动：**
- 使用 `postpone()` 延迟发布事件
- 不在 ThreadPool 线程中直接处理事件
- 主线程调用 `process()` 批量处理

---

#### Scheduler 集成

**修改点：** `scheduler/Scheduler.cpp`

```cpp
#include "../eventbus/EventBusManager.h"
#include "../eventbus/events/SystemEvents.h"

void Scheduler::executeAnalysis() {
    auto bus = eventbus::EventBusManager::getInstance().getCoreBus();
    
    // 发布启动事件
    bus->postpone(eventbus::events::AppStarted{
        VERSION_STRING, BUILD_DATE
    });
    
    // 执行分析回调
    callback_();
    
    // 处理事件
    bus->process();
}

void Scheduler::stop() {
    auto bus = eventbus::EventBusManager::getInstance().getCoreBus();
    bus->postpone(eventbus::events::AppStopping{"User requested stop"});
}
```

---

#### main.cpp 集成

**修改点：** `main.cpp`

```cpp
#include "eventbus/EventBusManager.h"
#include "eventbus/events/SystemEvents.h"

int main() {
    // 初始化 EventBus
    auto& eventBusManager = eventbus::EventBusManager::getInstance();
    auto bus = eventBusManager.getCoreBus();
    
    // 创建监听器
    dexode::EventBus::Listener listener{bus};
    
    // 订阅进度事件
    listener.listen([](const eventbus::events::ProgressUpdated& event) {
        LOG_INFO("进度更新: " + std::to_string(event.completed) + 
                 "/" + std::to_string(event.total) + 
                 " (" + event.status + ")");
    });
    
    // 订阅分析完成事件
    listener.listen([](const eventbus::events::StockAnalysisCompleted& event) {
        if (event.success) {
            LOG_INFO("分析完成: " + event.tsCode + " (" + event.stockName + ")");
        }
    });
    
    // 订阅失败事件
    listener.listen([](const eventbus::events::StockAnalysisFailed& event) {
        LOG_ERROR("分析失败: " + event.tsCode + " - " + event.error);
    });
    
    // 执行分析...
    
    // 定期处理事件（在 ThreadPool.wait() 之后）
    eventBusManager.process();
    
    return 0;
}
```

---

### 数据流设计

#### 事件流程

```
1. 初始化阶段:
   main.cpp → EventBusManager::getInstance() → 初始化 EventBus

2. 订阅阶段:
   main.cpp → Listener.listen() → 注册事件处理器

3. 发布阶段:
   ThreadPool/Scheduler → bus->postpone(event) → 事件入队

4. 处理阶段:
   main.cpp → bus->process() → 批量处理事件 → Listener 处理器执行
```

#### 线程安全

- ✅ EventBus 本身线程安全
- ✅ `postpone()` 可在任意线程调用
- ✅ `process()` 必须在主线程调用（避免竞态）
- ✅ Listener 注册可在任意线程，但建议主线程

---

## 实施计划

### 阶段划分

#### 第一阶段：基础设施（Day 1-2）

**任务：**
1. 添加 EventBus 库到 thirdparty/
2. 创建 eventbus/ 模块结构
3. 实现 EventBusManager
4. 定义系统事件类型
5. 编写单元测试

**验收：**
- ✅ EventBus 编译成功
- ✅ EventBusManager 单元测试通过
- ✅ 事件发布/订阅测试通过

---

#### 第二阶段：ThreadPool 集成（Day 3）

**任务：**
1. 修改 ThreadPool 任务完成逻辑
2. 发布 StockAnalysisCompleted/Failed 事件
3. 测试并发场景

**验收：**
- ✅ ThreadPool 任务完成事件正常发布
- ✅ 多线程环境下无竞态问题

---

#### 第三阶段：Scheduler 集成（Day 4）

**任务：**
1. 修改 Scheduler 启动/停止逻辑
2. 发布 AppStarted/AppStopping 事件
3. 测试定时场景

**验收：**
- ✅ Scheduler 状态变更事件正常发布
- ✅ 定时任务流程无异常

---

#### 第四阶段：main.cpp 集成（Day 5）

**任务：**
1. 在 main.cpp 初始化 EventBus
2. 订阅所有系统事件
3. 集成进度更新逻辑
4. 测试完整流程

**验收：**
- ✅ 系统启动流程正常
- ✅ 进度更新通过 EventBus 实现
- ✅ 日志记录正常

---

#### 第五阶段：测试与优化（Day 6-7）

**任务：**
1. 编写完整测试套件
2. 性能基准测试
3. 文档编写
4. 代码审查

**验收：**
- ✅ 单元测试覆盖率 > 80%
- ✅ 性能无明显下降（< 5%）
- ✅ 文档完整清晰

---

### 文件结构

```
cpp/
├── eventbus/                    # 新增模块
│   ├── EventBusManager.h/cpp    # 核心 EventBus 管理
│   ├── EventValidator.h/cpp     # 事件验证
│   ├── events/                  # 事件定义
│   │   ├── SystemEvents.h       # 系统事件
│   │   ├── AnalysisEvents.h     # 分析事件（可选）
│   │   └ DataEvents.h           # 数据事件（可选）
│   ├── EventBus.h               # 统一头文件
│   └── README.md                # 模块文档
├── thirdparty/
│   └ EventBus/                  # gelldur/EventBus 库
│       ├── include/
│       │   └ EventBus.hpp
│       └ lib/
│       └ ...
├── tests/
│   └ eventbus/                  # EventBus 测试
│       ├── TestEventBus.cpp
│       └ TestEvents.cpp
│       └ TestIntegration.cpp
└── CMakeLists.txt               # 修改：添加 EventBus
```

---

## 测试策略

### 单元测试

#### EventBusManager 测试

```cpp
TEST(EventBusManagerTest, TestSingleton) {
    auto& instance1 = eventbus::EventBusManager::getInstance();
    auto& instance2 = eventbus::EventBusManager::getInstance();
    ASSERT_EQ(&instance1, &instance2);
}

TEST(EventBusManagerTest, TestGetCoreBus) {
    auto bus = eventbus::EventBusManager::getInstance().getCoreBus();
    ASSERT_NE(bus, nullptr);
}
```

---

#### 事件发布/订阅测试

```cpp
TEST(EventBusTest, TestPostAndProcess) {
    auto bus = eventbus::EventBusManager::getInstance().getCoreBus();
    
    bool received = false;
    dexode::EventBus::Listener listener{bus};
    listener.listen([&](const eventbus::events::ProgressUpdated& event) {
        received = true;
        ASSERT_EQ(event.total, 100);
        ASSERT_EQ(event.completed, 50);
    });
    
    bus->postpone(eventbus::events::ProgressUpdated{100, 50, 0, "running"});
    bus->process();
    
    ASSERT_TRUE(received);
}
```

---

#### 多监听器测试

```cpp
TEST(EventBusTest, TestMultipleListeners) {
    auto bus = eventbus::EventBusManager::getInstance().getCoreBus();
    
    int count = 0;
    dexode::EventBus::Listener listener1{bus};
    dexode::EventBus::Listener listener2{bus};
    
    listener1.listen([&](const eventbus::events::StockAnalysisCompleted&) {
        count++;
    });
    
    listener2.listen([&](const eventbus::events::StockAnalysisCompleted&) {
        count++;
    });
    
    bus->postpone(eventbus::events::StockAnalysisCompleted{
        "000001.SZ", "平安银行", true, ""
    });
    bus->process();
    
    ASSERT_EQ(count, 2);
}
```

---

#### 线程安全测试

```cpp
TEST(EventBusTest, TestThreadSafety) {
    auto bus = eventbus::EventBusManager::getInstance().getCoreBus();
    
    std::atomic<int> count{0};
    dexode::EventBus::Listener listener{bus};
    listener.listen([&](const eventbus::events::ProgressUpdated&) {
        count++;
    });
    
    // 多线程发布事件
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&]() {
            bus->postpone(eventbus::events::ProgressUpdated{100, i, 0, "running"});
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    bus->process();
    
    ASSERT_EQ(count.load(), 10);
}
```

---

### 集成测试

#### ThreadPool 集成测试

```cpp
TEST(ThreadPoolIntegrationTest, TestEventPublish) {
    utils::ThreadPool pool(4);
    
    auto bus = eventbus::EventBusManager::getInstance().getCoreBus();
    std::atomic<int> completedCount{0};
    
    dexode::EventBus::Listener listener{bus};
    listener.listen([&](const eventbus::events::StockAnalysisCompleted&) {
        completedCount++;
    });
    
    // 添加任务
    for (int i = 0; i < 10; ++i) {
        pool.enqueue([&]() {
            bus->postpone(eventbus::events::StockAnalysisCompleted{
                "TEST", "Test", true, ""
            });
        });
    }
    
    pool.wait();
    bus->process();
    
    ASSERT_EQ(completedCount.load(), 10);
}
```

---

### 性能基准测试

```cpp
TEST(EventBusPerformanceTest, BenchmarkPostAndProcess) {
    auto bus = eventbus::EventBusManager::getInstance().getCoreBus();
    
    dexode::EventBus::Listener listener{bus};
    listener.listen([](const eventbus::events::ProgressUpdated&) {});
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 10000; ++i) {
        bus->postpone(eventbus::events::ProgressUpdated{10000, i, 0, "running"});
    }
    
    bus->process();
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "10000 events processed in " << duration.count() << " ms" << std::endl;
    
    // 性能要求：< 100ms
    ASSERT_LT(duration.count(), 100);
}
```

---

## 性能考虑

### postpone 模式优势

**EventBus postpone 模式：**

```cpp
// 在 ThreadPool 线程中
bus->postpone(event);  // 事件入队，不立即处理

// 在主线程中
bus->process();        // 批量处理所有事件
```

**优势：**
1. **避免线程竞态** - 事件处理统一在主线程
2. **批量处理** - 提高性能，减少上下文切换
3. **可控时机** - 主线程决定何时处理
4. **符合设计理念** - EventBus 推荐模式

---

### 事件频率控制

**高频事件（如 ProgressUpdated）：**

```cpp
// 建议：每秒最多发布一次
class EventRateLimiter {
    std::chrono::steady_clock::time_point lastPublish_;
    
public:
    bool shouldPublish() {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastPublish_);
        
        if (elapsed.count() >= 1) {
            lastPublish_ = now;
            return true;
        }
        return false;
    }
};

// 使用
if (rateLimiter.shouldPublish()) {
    bus->postpone(eventbus::events::ProgressUpdated{...});
}
```

---

### 性能影响评估

**预期影响：**

| 指标 | 当前 | 预期 | 影响 |
|------|------|------|------|
| 任务完成通知 | 直接回调 | EventBus 事件 | < 2% |
| 进度更新 | 直接调用 | EventBus 事件 | < 1% |
| 日志记录 | 直接调用 | EventBus + 日志 | < 1% |
| **总体影响** | - | - | **< 5%** |

**优化措施：**
- 使用 postpone 模式批量处理
- 高频事件频率控制
- 主线程定期处理（每秒一次）

---

## 风险评估

### 技术风险

| 风险 | 影响 | 概率 | 缓解措施 |
|------|------|------|----------|
| EventBus 库兼容性 | 中 | 低 | 充分测试，查看 GitHub Issues |
| 线程安全问题 | 高 | 低 | 使用 postpone 模式，主线程处理 |
| 性能下降 | 中 | 低 | postpone 模式，性能基准测试 |
| 内存泄漏 | 低 | 低 | 使用 shared_ptr，RAII 管理 |

---

### 业务风险

| 雎险 | 影响 | 概率 | 缓解措施 |
|------|------|------|----------|
| 系统不稳定 | 高 | 低 | 充分测试，渐进式改造 |
| 日志丢失 | 中 | 低 | 事件验证，异常捕获 |
| 进度显示异常 | 中 | 低 | 事件频率控制，UI 测试 |

---

### 依赖风险

| 风险 | 影响 | 概率 | 缓解措施 |
|------|------|------|----------|
| EventBus 库停止维护 | 低 | 低 | Apache 2.0 许可，可自行维护 |
| C++17 兼容性 | 低 | 低 | 项目已使用 C++17 |
| 编译问题 | 低 | 中 | CMake 配置，CI 测试 |

---

## 附录

### 参考资料

- **EventBus GitHub**: https://github.com/gelldur/EventBus
- **EventBus 文档**: https://gelldur.github.io/EventBus/
- **设计模式**: 观察者模式、单例模式
- **SOLID 原则**: 依赖倒置原则（DIP）

---

### 相关文档

- [系统设计文档](../cpp/DESIGN.md)
- [ThreadPool 文档](../cpp/utils/README.md)
- [Scheduler 文档](../cpp/scheduler/Scheduler.h)
- [日志系统文档](../cpp/log/README.md)

---

### 变更记录

| 版本 | 日期 | 变更内容 | 作者 |
|------|------|----------|------|
| 1.0 | 2026-04-06 | 初始设计文档 | Development Team |

---

**文档状态**: Draft  
**下一步**: 调用 writing-plans skill 创建详细实施计划