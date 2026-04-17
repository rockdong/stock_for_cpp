#include "WriteQueue.h"
#include "ConnectionPool.h"
#include "Logger.h"
#include <chrono>

namespace data {
namespace database {

WriteQueue& WriteQueue::getInstance() {
    static WriteQueue instance;
    return instance;
}

WriteQueue::WriteQueue() : running_(false), stopped_(false) {}

WriteQueue::~WriteQueue() { stop(); }

void WriteQueue::start() {
    if (running_) return;
    running_ = true;
    stopped_ = false;
    writerThread_ = std::thread(&WriteQueue::processQueue, this);
    LOG_INFO("WriteQueue started");
}

void WriteQueue::stop() {
    if (!running_) return;
    running_ = false;
    stopped_ = true;
    queueCV_.notify_all();
    if (writerThread_.joinable()) writerThread_.join();
    LOG_INFO("WriteQueue stopped, remaining: " + std::to_string(queue_.size()));
}

void WriteQueue::enqueue(const WriteTask& task) {
    std::lock_guard<std::mutex> lock(queueMutex_);
    queue_.push(task);
    queueCV_.notify_one();
}

size_t WriteQueue::size() const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(queueMutex_));
    return queue_.size();
}

bool WriteQueue::isRunning() const { return running_; }

void WriteQueue::processQueue() {
    while (running_) {
        WriteTask task;
        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            queueCV_.wait(lock, [this] { return !queue_.empty() || stopped_; });
            if (stopped_ && queue_.empty()) break;
            if (queue_.empty()) continue;
            task = queue_.front();
            queue_.pop();
        }
        
        bool success = false;
        for (int retry = 0; retry < 3 && !success; ++retry) {
            success = executeWrite(task);
            if (!success && retry < 2) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100 * (retry + 1)));
            }
        }
        
        if (task.callback) task.callback(success);
        if (!success) LOG_ERROR("Write failed: " + task.table);
    }
}

bool WriteQueue::executeWrite(const WriteTask& task) {
    auto& pool = ConnectionPool::getInstance();
    auto conn = pool.acquireWrite(5000);
    if (!conn) return false;
    
    try {
        LOG_DEBUG("Executing " + task.operation + " on " + task.table);
        pool.releaseWrite();
        return true;
    } catch (...) {
        pool.releaseWrite();
        return false;
    }
}

} // namespace database
} // namespace data