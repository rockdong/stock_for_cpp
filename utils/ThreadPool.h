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
