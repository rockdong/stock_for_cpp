# 多线程股票分析优化设计

## 背景

当前项目在 `main.cpp` 的 `performBatchAnalysis` 函数中串行处理股票列表，每只股票依次调用 `analyzeStock` 进行分析。当股票数量较多时（如 5000 只），串行处理效率低下。

## 目标

使用多线程并发处理股票分析，提高整体处理速度。

## 需求

- 线程数：动态（CPU 核数）
- 限速：不需要
- 失败处理：继续处理其他股票

## 设计

### 架构

采用 **线程池 + 任务队列** 模式：

```
主线程
  ├── 加载股票列表
  ├── 创建线程池
  ├── 提交任务到队列
  └── 等待完成

线程池 (N 个 worker)
  ├── Worker 1: 取任务 → 分析 → 写入结果
  ├── Worker 2: 取任务 → 分析 → 写入结果
  └── Worker N: 取任务 → 分析 → 写入结果

数据层
  ├── 数据源：线程独立创建
  └── 数据库：互斥锁保护写入
```

### 关键组件

1. **ThreadPool 类**
   - 管理线程生命周期
   - 任务队列（线程安全）
   - `enqueue()` 提交任务
   - `wait()` 等待完成

2. **数据源隔离**
   - 每个线程创建独立的 `TushareClient`
   - 避免线程安全问题

3. **数据库写入保护**
   - `std::mutex` 保护 `AnalysisResultDAO`

### 线程数选择

```cpp
unsigned int threadCount = std::thread::hardware_concurrency();
if (threadCount == 0) threadCount = 4;
```

## 影响范围

- `main.cpp`: 修改 `performBatchAnalysis` 函数
- 新增 `utils/ThreadPool.h/cpp`: 线程池实现

## 测试

- 验证多线程正确性
- 性能对比（单线程 vs 多线程）
