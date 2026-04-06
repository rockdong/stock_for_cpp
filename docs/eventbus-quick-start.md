# EventBus 快速开始指南

## 🚀 5 分钟上手 EventBus

### 步骤 1: 包含头文件

```cpp
#include "EventBus.h"
using namespace eventbus;
```

### 步骤 2: 初始化 EventBus

```cpp
// 在程序启动时初始化
if (!eventbus::initialize()) {
    std::cerr << "EventBus 初始化失败" << std::endl;
    return -1;
}
```

### 步骤 3: 订阅事件

```cpp
auto bus = EventBusManager::getInstance().getCoreBus();
dexode::EventBus::Listener listener{bus};

// 订阅进度更新事件
listener.listen([](const events::ProgressUpdated& e) {
    std::cout << "进度: " << e.completed << "/" << e.total << std::endl;
});
```

### 步骤 4: 发布事件

```cpp
// 使用 postpone 延迟处理
bus->postpone(events::ProgressUpdated{100, 50, 0, "running"});

// 在主线程中处理事件
bus->process();
```

### 步骤 5: 清理资源

```cpp
// 在程序退出时清理
eventbus::shutdown();
```

---

## 📋 完整示例

```cpp
#include <iostream>
#include "EventBus.h"

using namespace eventbus;

int main() {
    // 1. 初始化
    eventbus::initialize();
    
    // 2. 获取 EventBus
    auto bus = EventBusManager::getInstance().getCoreBus();
    
    // 3. 创建监听器并订阅事件
    dexode::EventBus::Listener listener{bus};
    
    listener.listen([](const events::ProgressUpdated& e) {
        std::cout << "进度: " << e.completed << "/" << e.total 
                  << " - " << e.status << std::endl;
    });
    
    listener.listen([](const events::StockAnalysisCompleted& e) {
        if (e.success) {
            std::cout << "✅ " << e.tsCode << " 分析完成" << std::endl;
        }
    });
    
    // 4. 发布事件
    bus->postpone(events::ProgressUpdated{100, 0, 0, "starting"});
    bus->postpone(events::ProgressUpdated{100, 50, 0, "running"});
    bus->postpone(events::StockAnalysisCompleted{"000001.SZ", "平安银行", true, ""});
    bus->postpone(events::ProgressUpdated{100, 100, 0, "completed"});
    
    // 5. 处理事件
    bus->process();
    
    // 6. 清理
    eventbus::shutdown();
    
    return 0;
}
```

---

## 🎯 常用事件类型

### 系统事件

| 事件 | 用途 | 示例 |
|------|------|------|
| `AppStarted` | 应用启动 | `AppStarted{"1.0", "2026-04-06"}` |
| `AppStopping` | 应用停止 | `AppStopping{"用户请求"}` |

### 业务事件

| 事件 | 用途 | 示例 |
|------|------|------|
| `ProgressUpdated` | 进度更新 | `ProgressUpdated{100, 50, 0, "running"}` |
| `StockAnalysisCompleted` | 分析完成 | `StockAnalysisCompleted{"000001.SZ", "平安银行", true, ""}` |
| `StrategySignalGenerated` | 策略信号 | `StrategySignalGenerated{"000001.SZ", "EMA", "buy", 0.85, "d"}` |

---

## 💡 最佳实践

### 1. 使用 postpone 模式

```cpp
// ✅ 推荐：postpone + process
bus->postpone(event);  // 在任意线程发布
bus->process();        // 在主线程批量处理

// ❌ 不推荐：fire（可能导致竞态）
bus->fire(event);      // 立即处理
```

### 2. 主线程处理事件

```cpp
// 在主线程定期调用
void mainLoop() {
    while (running) {
        // 处理事件
        eventbus::EventBusManager::getInstance().process();
        
        // 其他任务...
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
```

### 3. 验证事件数据

```cpp
events::ProgressUpdated event{100, 50, 0, "running"};

if (EventValidator::validateProgress(event)) {
    bus->postpone(event);
} else {
    std::cerr << "无效的进度事件" << std::endl;
}
```

### 4. 使用命名空间

```cpp
// 简化代码
using namespace eventbus;
using namespace eventbus::events;

// 直接使用
bus->postpone(ProgressUpdated{100, 50, 0, "running"});
```

---

## 🔧 编译配置

### CMakeLists.txt

```cmake
# 添加 eventbus_lib
target_link_libraries(your_target
    eventbus_lib
    EventBus
)
```

### 编译命令

```bash
cd build
cmake ..
make
```

---

## 📖 进阶用法

### 多个监听器

```cpp
// 多个监听器订阅同一事件
dexode::EventBus::Listener listener1{bus};
dexode::EventBus::Listener listener2{bus};

listener1.listen([](const ProgressUpdated& e) {
    std::cout << "监听器 1: " << e.completed << std::endl;
});

listener2.listen([](const ProgressUpdated& e) {
    std::cout << "监听器 2: " << e.completed << std::endl;
});

// 两个监听器都会收到事件
bus->postpone(ProgressUpdated{100, 50, 0, "running"});
bus->process();
```

### Lambda 捕获

```cpp
std::string stockCode = "000001.SZ";

// 捕获外部变量
listener.listen([stockCode](const StockAnalysisCompleted& e) {
    if (e.tsCode == stockCode) {
        std::cout << "匹配股票: " << e.tsCode << std::endl;
    }
});
```

### 成员函数订阅

```cpp
class AnalysisManager {
public:
    void onProgress(const ProgressUpdated& e) {
        // 处理进度
    }
    
    void subscribe(std::shared_ptr<dexode::EventBus> bus) {
        listener_ = std::make_unique<dexode::EventBus::Listener>(bus);
        listener_->listen([this](const ProgressUpdated& e) {
            this->onProgress(e);
        });
    }
    
private:
    std::unique_ptr<dexode::EventBus::Listener> listener_;
};
```

---

## ⚠️ 注意事项

### 线程安全

- ✅ `postpone()` 可在任意线程调用
- ✅ `process()` 必须在主线程调用
- ❌ 不要在 Listener 回调中调用 `postpone()`

### 内存管理

- ✅ EventBus 使用 `shared_ptr` 管理
- ✅ Listener 使用 `weak_ptr` 避免循环引用
- ❌ 不要在事件中传递大对象（使用智能指针）

### 性能考虑

- ✅ 使用 `postpone()` 批量处理
- ✅ 控制事件发布频率
- ❌ 避免高频事件（每秒 > 1000 次）

---

## 🐛 常见问题

### Q1: 事件没有触发？

**检查清单：**
- [ ] 是否调用了 `process()`？
- [ ] Listener 是否正确创建？
- [ ] 事件类型是否匹配？

### Q2: 编译错误？

**常见原因：**
- 未包含 `EventBus.h`
- 未链接 `eventbus_lib`
- C++ 标准版本 < 17

### Q3: 运行时崩溃？

**调试步骤：**
1. 检查 EventBus 是否初始化
2. 检查事件数据是否有效
3. 使用 EventValidator 验证

---

## 📚 更多资源

- **完整文档：** `docs/plans/2026-04-06-eventbus-integration-summary.md`
- **示例代码：** `cpp/examples/eventbus_example.cpp`
- **API 参考：** `cpp/eventbus/README.md`

---

**快速开始完成！开始使用 EventBus 构建事件驱动的应用程序吧！** 🎉