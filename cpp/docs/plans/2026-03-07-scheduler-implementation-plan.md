# 定时任务调度器实现计划

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development to implement this plan task-by-task.

**Goal:** 为股票分析系统添加定时任务功能，每天 20:00 执行批量分析，仅在交易日（周一到周五）运行。

**Architecture:** 在 main.cpp 中增加调度循环，将程序从单次执行改为常驻服务模式。新增 Scheduler 和 TradeCalendar 模块。

**Tech Stack:** C++17, CMake, spdlog

---

## 阶段 1: 准备文件结构和基础模块

### Task 1: 创建 scheduler 模块目录和头文件

**Files:**
- Create: `scheduler/Scheduler.h`
- Create: `scheduler/Scheduler.cpp`
- Create: `scheduler/TradeCalendar.h`
- Create: `scheduler/TradeCalendar.cpp`

**Step 1: 创建目录和 Scheduler.h**

```cpp
// scheduler/Scheduler.h
#pragma once

#include <string>
#include <memory>
#include <functional>

namespace scheduler {

/**
 * @brief 定时调度器
 * 
 * 管理每日定时执行任务，支持：
 * - 定时执行（每日指定时间）
 * - 交易日判断
 * - 优雅退出
 */
class Scheduler {
public:
    using ExecuteCallback = std::function<void()>;
    
    /**
     * @brief 构造函数
     * @param executeTime 执行时间，格式 HH:MM
     * @param callback 执行回调
     */
    Scheduler(const std::string& executeTime, ExecuteCallback callback);
    
    /**
     * @brief 启动调度器（阻塞）
     */
    void run();
    
    /**
     * @brief 停止调度器
     */
    void stop();
    
    /**
     * @brief 设置是否只运行一次
     */
    void setOnceMode(bool once);
    
private:
    bool shouldRunNow();
    bool hasRunToday();
    void markAsRun();
    void executeAnalysis();
    std::string getCurrentDate() const;
    std::string getCurrentTime() const;
    int getCurrentHour() const;
    int getCurrentMinute() const;
    
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace scheduler
```

**Step 2: 创建 TradeCalendar.h**

```cpp
// scheduler/TradeCalendar.h
#pragma once

#include <string>

namespace scheduler {

/**
 * @brief 交易日历
 * 
 * 判断当前日期是否为交易日
 */
class TradeCalendar {
public:
    /**
     * @brief 判断是否为交易日
     * @param date 日期字符串，格式 YYYY-MM-DD
     * @return true 是交易日，false 非交易日
     */
    static bool isTradingDay(const std::string& date);
    
    /**
     * @brief 判断当前日期是否为交易日
     * @return true 是交易日，false 非交易日
     */
    static bool isTradingDayToday();
    
    /**
     * @brief 获取星期几
     * @param date 日期字符串，格式 YYYY-MM-DD
     * @return 0=周日, 1=周一, ..., 6=周六
     */
    static int getDayOfWeek(const std::string& date);
};

} // namespace scheduler
```

**Step 3: 创建 CMakeLists.txt 片段**

在项目根目录 CMakeLists.txt 中添加：
```cmake
# 添加调度器模块
add_library(scheduler_lib STATIC
    scheduler/Scheduler.cpp
    scheduler/TradeCalendar.cpp
)

target_include_directories(scheduler_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/scheduler
    ${CMAKE_CURRENT_SOURCE_DIR}/log
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
)

target_link_libraries(scheduler_lib PUBLIC
    log_lib
    utils_lib
)
```

---

## 阶段 2: 实现 TradeCalendar 模块

### Task 2: 实现 TradeCalendar.cpp

**Files:**
- Modify: `scheduler/TradeCalendar.cpp`

**Step 1: 写入实现代码**

```cpp
// scheduler/TradeCalendar.cpp
#include "TradeCalendar.h"
#include "TimeUtil.h"
#include <ctime>

namespace scheduler {

bool TradeCalendar::isTradingDay(const std::string& date) {
    int dayOfWeek = getDayOfWeek(date);
    // 0=周日, 6=周六
    // 周一到周五 (1-5) 是交易日
    return dayOfWeek >= 1 && dayOfWeek <= 5;
}

bool TradeCalendar::isTradingDayToday() {
    std::string today = utils::TimeUtil::today();
    return isTradingDay(today);
}

int TradeCalendar::getDayOfWeek(const std::string& date) {
    // 解析日期字符串
    std::tm tm = {};
    if (sscanf(date.c_str(), "%d-%d-%d", &tm.tm_year, &tm.tm_mon, &tm.tm_mday) != 3) {
        return -1;
    }
    tm.tm_year -= 1900;  // 年份从 1900 开始
    tm.tm_mon -= 1;      // 月份从 0 开始
    
    std::mktime(&tm);
    return tm.tm_wday;
}

} // namespace scheduler
```

**Step 2: 添加 TimeUtil 依赖**

检查 utils/TimeUtil.h 中是否有 today() 方法。如果没有，需要添加。

---

## 阶段 3: 实现 Scheduler 模块

### Task 3: 实现 Scheduler.cpp

**Files:**
- Modify: `scheduler/Scheduler.cpp`

**Step 1: 写入实现代码**

```cpp
// scheduler/Scheduler.cpp
#include "Scheduler.h"
#include "TradeCalendar.h"
#include "TimeUtil.h"
#include "Logger.h"
#include <thread>
#include <chrono>
#include <fstream>
#include <atomic>
#include <csignal>

namespace scheduler {

class Scheduler::Impl {
public:
    Impl(const std::string& executeTime, ExecuteCallback callback)
        : executeTime_(executeTime)
        , callback_(callback)
        , running_(false)
        , onceMode_(false) {
        parseExecuteTime();
    }
    
    void run() {
        running_ = true;
        LOG_INFO("调度器启动，每日执行时间: " + executeTime_);
        
        while (running_) {
            if (shouldRunNow()) {
                LOG_INFO("触发定时任务执行...");
                executeAnalysis();
                if (onceMode_) {
                    LOG_INFO("单次模式执行完成，退出调度器");
                    break;
                }
            }
            
            // 每秒检查一次
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        
        LOG_INFO("调度器已停止");
    }
    
    void stop() {
        running_ = false;
    }
    
    void setOnceMode(bool once) {
        onceMode_ = once;
    }
    
private:
    bool shouldRunNow() {
        // 检查时间
        std::string currentTime = getCurrentTime();
        if (currentTime != executeTime_) {
            return false;
        }
        
        // 检查是否已经执行过
        if (hasRunToday()) {
            return false;
        }
        
        // 检查是否为交易日
        if (!TradeCalendar::isTradingDayToday()) {
            LOG_INFO("今日非交易日，跳过执行");
            markAsRun();  // 标记已执行，避免明日重复检查
            return false;
        }
        
        return true;
    }
    
    bool hasRunToday() {
        std::string today = getCurrentDate();
        std::string lastRunFile = "data/.last_run";
        
        std::ifstream file(lastRunFile);
        if (file.is_open()) {
            std::string lastDate;
            file >> lastDate;
            file.close();
            return lastDate == today;
        }
        return false;
    }
    
    void markAsRun() {
        std::string today = getCurrentDate();
        std::string lastRunFile = "data/.last_run";
        
        std::ofstream file(lastRunFile);
        if (file.is_open()) {
            file << today;
            file.close();
            LOG_INFO("已记录执行日期: " + today);
        } else {
            LOG_WARN("无法写入执行记录文件: " + lastRunFile);
        }
    }
    
    void executeAnalysis() {
        try {
            markAsRun();
            callback_();
            LOG_INFO("定时任务执行完成");
        } catch (const std::exception& e) {
            LOG_ERROR("定时任务执行失败: " + std::string(e.what()));
        }
    }
    
    void parseExecuteTime() {
        // 格式: HH:MM
        int hour = 20, minute = 0;
        sscanf(executeTime_.c_str(), "%d:%d", &hour, &minute);
        executeHour_ = hour;
        executeMinute_ = minute;
    }
    
    std::string getCurrentDate() const {
        return utils::TimeUtil::today();
    }
    
    std::string getCurrentTime() const {
        time_t now = time(nullptr);
        struct tm* tm_now = localtime(&now);
        char buf[6];
        snprintf(buf, sizeof(buf), "%02d:%02d", tm_now->tm_hour, tm_now->tm_min);
        return std::string(buf);
    }
    
    int getCurrentHour() const {
        time_t now = time(nullptr);
        struct tm* tm_now = localtime(&now);
        return tm_now->tm_hour;
    }
    
    int getCurrentMinute() const {
        time_t now = time(nullptr);
        struct tm* tm_now = localtime(&now);
        return tm_now->tm_min;
    }
    
    std::string executeTime_;
    int executeHour_;
    int executeMinute_;
    ExecuteCallback callback_;
    std::atomic<bool> running_;
    bool onceMode_;
};

// Scheduler 成员函数实现
Scheduler::Scheduler(const std::string& executeTime, ExecuteCallback callback)
    : pImpl(std::make_unique<Impl>(executeTime, callback)) {}

Scheduler::~Scheduler() = default;

void Scheduler::run() {
    pImpl->run();
}

void Scheduler::stop() {
    pImpl->stop();
}

void Scheduler::setOnceMode(bool once) {
    pImpl->setOnceMode(once);
}

} // namespace scheduler
```

---

## 阶段 4: 修改 main.cpp

### Task 4: 添加命令行参数解析

**Files:**
- Modify: `main.cpp`

**Step 1: 添加命令行解析代码**

在文件开头添加：
```cpp
#include <cstring>
#include <csignal>

// 全局调度器指针（用于信号处理）
scheduler::Scheduler* g_scheduler = nullptr;

// 信号处理函数
void signalHandler(int signal) {
    LOG_INFO("收到退出信号，正在优雅关闭...");
    if (g_scheduler) {
        g_scheduler->stop();
    }
}
```

**Step 2: 添加参数解析函数**

```cpp
struct ProgramOptions {
    bool onceMode = false;
    std::string executeTime = "20:00";
    bool help = false;
};

ProgramOptions parseArgs(int argc, char* argv[]) {
    ProgramOptions options;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--once" || arg == "-o") {
            options.onceMode = true;
        } else if (arg == "--help" || arg == "-h") {
            options.help = true;
        } else if (arg == "--time" || arg == "-t") {
            if (i + 1 < argc) {
                options.executeTime = argv[++i];
            }
        }
    }
    
    return options;
}
```

**Step 3: 添加帮助信息函数**

```cpp
void printHelp(const char* programName) {
    std::cout << "用法: " << programName << " [选项]" << std::endl;
    std::cout << std::endl;
    std::cout << "选项:" << std::endl;
    std::cout << "  --once, -o       单次执行模式（默认：定时模式）" << std::endl;
    std::cout << "  --time, -t TIME  设置执行时间，格式 HH:MM（默认：20:00）" << std::endl;
    std::cout << "  --help, -h       显示帮助信息" << std::endl;
    std::cout << std::endl;
    std::cout << "示例:" << std::endl;
    std::cout << "  " << programName << "           # 定时模式，每天 20:00 执行" << std::endl;
    std::cout << "  " << programName << " --once   # 单次执行模式" << std::endl;
    std::cout << "  " << programName << " -t 09:30 # 每天 09:30 执行" << std::endl;
}
```

**Step 4: 修改 main 函数**

将 main 函数修改为：

```cpp
int main(int argc, char* argv[]) {
    try {
        // 0. 解析命令行参数
        ProgramOptions options = parseArgs(argc, argv);
        
        if (options.help) {
            printHelp(argv[0]);
            return 0;
        }
        
        // 注册信号处理
        std::signal(SIGINT, signalHandler);
        std::signal(SIGTERM, signalHandler);
        
        // 0. 打印版本信息
        printVersion();
        
        // 1. 初始化系统
        if (!initializeSystem()) {
            return 1;
        }
        
        // 2. 打印配置信息
        printConfiguration();
        
        // 3. 初始化数据库
        if (!initializeDatabase()) {
            return 1;
        }
        
        // 4. 初始化策略管理器
        if (!initializeStrategies()) {
            return 1;
        }
        
        // 5. 创建 DAO 和数据源
        data::StockDAO stockDao;
        data::AnalysisResultDAO analysisResultDao;
        auto dataSource = network::DataSourceFactory::createFromConfig();
        auto& strategyManager = core::StrategyManager::getInstance();
        
        // 6. 加载股票列表
        auto stockList = loadStockList(dataSource);
        
        // 7. 根据模式执行
        if (options.onceMode) {
            // 单次执行模式
            LOG_INFO("执行单次分析...");
            performBatchAnalysis(stockList, dataSource, strategyManager, analysisResultDao);
        } else {
            // 定时执行模式
            LOG_INFO("启动定时调度模式，执行时间: " + options.executeTime);
            
            scheduler::Scheduler sched(options.executeTime, [&]() {
                performBatchAnalysis(stockList, dataSource, strategyManager, analysisResultDao);
            });
            
            g_scheduler = &sched;
            sched.run();
        }
        
        // 8. 清理资源
        cleanup();
        
        std::cout << "应用程序正常退出" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "程序异常: " << e.what() << std::endl;
        LOG_ERROR("程序异常: " + std::string(e.what()));
        cleanup();
        return 1;
    }
}
```

---

## 阶段 5: 更新 CMakeLists.txt

### Task 5: 添加 scheduler 模块到构建系统

**Files:**
- Modify: `CMakeLists.txt`

**Step 1: 添加 scheduler 库**

在 `add_library(core_lib ...)` 之前添加：

```cmake
# 添加调度器模块
add_library(scheduler_lib STATIC
    scheduler/Scheduler.cpp
    scheduler/TradeCalendar.cpp
)

target_include_directories(scheduler_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/scheduler
    ${CMAKE_CURRENT_SOURCE_DIR}/log
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/core
)

target_link_libraries(scheduler_lib PUBLIC
    log_lib
    utils_lib
    core_lib
)
```

**Step 2: 链接到主程序**

在 `target_link_libraries(stock_for_cpp PRIVATE ...)` 中添加：

```cmake
scheduler_lib
```

---

## 阶段 6: 添加配置文件支持

### Task 6: 在 .env 中添加调度配置

**Files:**
- Modify: `.env`
- Modify: `env.example`

**Step 1: 添加配置项**

在 `.env` 和 `env.example` 中添加：

```env
# ========== 调度配置 ==========
# 执行时间（24小时制，格式：HH:MM）
SCHEDULER_EXECUTE_TIME=20:00
```

---

## 阶段 7: 编译测试

### Task 7: 编译项目

**Step 1: 清理并重新编译**

```bash
rm -rf build
mkdir build
cd build
cmake ..
make -j$(nproc)
```

**Step 2: 测试单次执行模式**

```bash
./stock_for_cpp --once
```

**Step 3: 测试帮助信息**

```bash
./stock_for_cpp --help
```

**Step 4: 测试指定时间**

```bash
./stock_for_cpp -t 09:30
# 程序应该立即执行（因为 09:30 是当前时间）
```

---

## 阶段 8: 验证功能

### Task 8: 验证调度逻辑

**Step 1: 验证交易日判断**

在代码中添加临时测试：
```cpp
// 测试代码
std::cout << "2026-03-07 是交易日: " << scheduler::TradeCalendar::isTradingDay("2026-03-07") << std::endl;  // 周六
std::cout << "2026-03-09 是交易日: " << scheduler::TradeCalendar::isTradingDay("2026-03-09") << std::endl;  // 周一
```

**Step 2: 验证时间检查**

确保 20:00 触发执行。

---

## 实现顺序

1. Task 1: 创建 scheduler 模块头文件
2. Task 2: 实现 TradeCalendar.cpp
3. Task 3: 实现 Scheduler.cpp
4. Task 4: 修改 main.cpp 添加参数解析和调度逻辑
5. Task 5: 更新 CMakeLists.txt
6. Task 6: 添加 .env 配置
7. Task 7: 编译测试
8. Task 8: 验证功能
