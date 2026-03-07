# 多线程股票分析优化实现计划

> **For Claude:** 使用 superpowers:executing-plans 逐任务实施

**目标:** 将串行股票分析改为多线程并发处理，提高处理速度

**架构:** 采用线程池 + 任务队列模式，主线程提交任务，多 worker 并发执行

**技术栈:** C++17, std::thread, std::mutex, std::condition_variable

---

## Task 1: 创建 ThreadPool 头文件

**Files:**
- Create: `utils/ThreadPool.h`

**Step 1: 编写 ThreadPool.h**

```cpp
#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <functional>
#include <atomic>

class ThreadPool {
public:
    explicit ThreadPool(size_t numThreads);
    ~ThreadPool();
    
    template<typename F>
    void enqueue(F&& task);
    
    void wait();
    
    size_t getThreadCount() const { return threads_.size(); }

private:
    void workerThread();
    
    std::vector<std::thread> threads_;
    std::queue<std::function<void()>> tasks_;
    std::mutex queueMutex_;
    std::condition_variable condition_;
    std::atomic<bool> stop_;
    std::atomic<int> activeTasks_;
};

template<typename F>
void ThreadPool::enqueue(F&& task) {
    {
        std::unique_lock<std::mutex> lock(queueMutex_);
        tasks_.push(std::forward<F>(task));
    }
    condition_.notify_one();
}

#endif
```

**Step 2: 提交代码**

```bash
git add utils/ThreadPool.h
git commit -m "feat: 添加 ThreadPool 头文件"
```

---

## Task 2: 创建 ThreadPool 实现文件

**Files:**
- Create: `utils/ThreadPool.cpp`

**Step 1: 编写 ThreadPool.cpp**

```cpp
#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t numThreads) : stop_(false), activeTasks_(0) {
    for (size_t i = 0; i < numThreads; ++i) {
        threads_.emplace_back(&ThreadPool::workerThread, this);
    }
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queueMutex_);
        stop_.store(true);
    }
    condition_.notify_all();
    
    for (auto& thread : threads_) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

void ThreadPool::workerThread() {
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            condition_.wait(lock, [this] {
                return stop_.load() || !tasks_.empty();
            });
            
            if (stop_.load() && tasks_.empty()) {
                return;
            }
            
            task = std::move(tasks_.front());
            tasks_.pop();
        }
        
        if (task) {
            activeTasks_++;
            task();
            activeTasks_--;
        }
    }
}

void ThreadPool::wait() {
    while (true) {
        std::unique_lock<std::mutex> lock(queueMutex_);
        if (tasks_.empty() && activeTasks_.load() == 0) {
            break;
        }
        lock.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
```

**Step 2: 添加到 CMakeLists.txt**

在 utils_lib 中添加 ThreadPool.cpp

**Step 3: 编译验证**

```bash
cmake -S . -B build && cmake --build build -j 16
```

预期: 编译成功

**Step 4: 提交代码**

```bash
git add utils/ThreadPool.cpp CMakeLists.txt
git commit -m "feat: 添加 ThreadPool 实现"
```

---

## Task 3: 修改 main.cpp 实现多线程分析

**Files:**
- Modify: `main.cpp:216-244`

**Step 1: 添加头文件**

```cpp
#include "ThreadPool.h"
#include <thread>
```

**Step 2: 修改 performBatchAnalysis 函数**

```cpp
void performBatchAnalysis(
    const std::vector<core::Stock>& stockList,
    std::shared_ptr<network::IDataSource> dataSource,
    core::StrategyManager& strategyManager,
    data::AnalysisResultDAO& analysisResultDao
) {
    LOG_INFO("========================================");
    LOG_INFO("开始批量分析，共 " + std::to_string(stockList.size()) + " 只股票");
    LOG_INFO("========================================");
    
    // 获取 CPU 核心数
    unsigned int threadCount = std::thread::hardware_concurrency();
    if (threadCount == 0) threadCount = 4;
    LOG_INFO("使用 " + std::to_string(threadCount) + " 个线程并发处理");
    
    // 创建线程池
    ThreadPool pool(threadCount);
    
    // 数据库写入锁
    std::mutex dbMutex;
    
    // 统计
    std::atomic<int> successCount(0);
    std::atomic<int> failCount(0);
    
    // 为每只股票创建任务
    for (const auto& stock : stockList) {
        pool.enqueue([&]() {
            try {
                // 每线程独立创建数据源
                auto localDataSource = network::DataSourceFactory::createFromConfig();
                
                // 执行分析
                analyzeStock(stock, localDataSource, strategyManager, analysisResultDao);
                successCount++;
            } catch (const std::exception& e) {
                LOG_ERROR("分析失败: " + stock.ts_code + " - " + std::string(e.what()));
                failCount++;
            }
        });
    }
    
    // 等待所有任务完成
    pool.wait();
    
    LOG_INFO("========================================");
    LOG_INFO("批量分析完成");
    LOG_INFO("  成功: " + std::to_string(successCount.load()) + " 只");
    LOG_INFO("  失败: " + std::to_string(failCount.load()) + " 只");
    LOG_INFO("========================================");
}
```

**Step 3: 编译验证**

```bash
cmake --build build -j 16
```

预期: 编译成功，无错误

**Step 4: 测试运行**

```bash
./build/stock_for_cpp
```

预期: 程序正常运行，使用多线程处理

**Step 5: 提交代码**

```bash
git add main.cpp
git commit -m "feat: 使用多线程并发处理股票分析"
```

---

## Task 4: 更新文档

**Files:**
- Modify: `docs/plans/2026-03-07-multithreading-analysis-design.md`

**Step 1: 添加实现记录**

在设计文档中添加 "Implementation" 部分，记录实现细节。

**Step 2: 提交文档**

```bash
git add docs/plans/2026-03-07-multithreading-analysis-design.md
git commit -m "docs: 更新多线程设计文档"
```

---

## 执行方式

**计划完成，保存到 `docs/plans/2026-03-07-multithreading-analysis-design.md`**

两种执行方式：

1. **Subagent-Driven (当前会话)** - 我逐任务分发子任务，任务间审查，快速迭代
2. **Parallel Session (单独)** - 新开 session 使用 executing-plans，带检查点的批量执行

选择哪种方式？
