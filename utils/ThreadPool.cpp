#include "ThreadPool.h"
#include <chrono>

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
