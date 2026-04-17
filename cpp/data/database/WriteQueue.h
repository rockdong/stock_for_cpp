#ifndef WRITE_QUEUE_H
#define WRITE_QUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <functional>
#include <string>
#include "json.hpp"

namespace data {
namespace database {

struct WriteTask {
    std::string table;
    std::string operation;
    nlohmann::json data;
    std::function<void(bool)> callback;
    
    WriteTask(const std::string& t, const std::string& op, 
              const nlohmann::json& d, std::function<void(bool)> cb = nullptr)
        : table(t), operation(op), data(d), callback(cb) {}
};

class WriteQueue {
public:
    static WriteQueue& getInstance();
    
    void start();
    void stop();
    void enqueue(const WriteTask& task);
    size_t size() const;
    bool isRunning() const;
    
private:
    WriteQueue();
    ~WriteQueue();
    WriteQueue(const WriteQueue&) = delete;
    WriteQueue& operator=(const WriteQueue&) = delete;
    
    void processQueue();
    bool executeWrite(const WriteTask& task);
    
    std::queue<WriteTask> queue_;
    std::mutex queueMutex_;
    std::condition_variable queueCV_;
    std::thread writerThread_;
    bool running_;
    bool stopped_;
};

} // namespace database
} // namespace data

#endif // WRITE_QUEUE_H