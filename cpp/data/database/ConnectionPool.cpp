#include "ConnectionPool.h"
#include "Logger.h"
#include "Connection.h"
#include <chrono>

namespace data {
namespace database {

std::string ConnectionPool::defaultDbPath_;

ConnectionPool& ConnectionPool::getInstance() {
    static ConnectionPool instance;
    return instance;
}

ConnectionPool::ConnectionPool() 
    : readPoolSize_(5)
    , writeInUse_(false)
    , initialized_(false) {
}

ConnectionPool::~ConnectionPool() {
    shutdown();
}

void ConnectionPool::setDefaultDbPath(const std::string& path) {
    defaultDbPath_ = path;
}

std::string ConnectionPool::getDefaultDbPath() {
    if (defaultDbPath_.empty()) {
        defaultDbPath_ = "/app/stock.db";
    }
    return defaultDbPath_;
}

void ConnectionPool::initialize(int readPoolSize, const std::string& dbPath) {
    if (initialized_) {
        LOG_WARN("ConnectionPool 已初始化，跳过重复初始化");
        return;
    }
    
    readPoolSize_ = readPoolSize;
    dbPath_ = dbPath.empty() ? getDefaultDbPath() : dbPath;
    
    LOG_INFO("初始化 ConnectionPool: 读连接=" + std::to_string(readPoolSize_) + 
             ", 写连接=1, 路径=" + dbPath_);
    
    try {
        for (int i = 0; i < readPoolSize_; ++i) {
            auto conn = std::make_shared<Connection>();
            conn->connect(dbPath_);
            readPool_.push_back(conn);
            availableRead_.push(conn);
        }
        
        writeConn_ = std::make_shared<Connection>();
        writeConn_->connect(dbPath_);
        
        initialized_ = true;
        LOG_INFO("ConnectionPool 初始化完成");
        
    } catch (const std::exception& e) {
        LOG_ERROR("ConnectionPool 初始化失败: " + std::string(e.what()));
        shutdown();
        throw;
    }
}

std::shared_ptr<Connection> ConnectionPool::acquireRead(int timeoutMs) {
    if (!initialized_) {
        LOG_ERROR("ConnectionPool 未初始化");
        return nullptr;
    }
    
    std::unique_lock<std::mutex> lock(readMutex_);
    
    bool available = readCV_.wait_for(lock, 
        std::chrono::milliseconds(timeoutMs),
        [this] { return !availableRead_.empty(); });
    
    if (!available) {
        LOG_ERROR("读连接池耗尽，等待超时 (timeout=" + std::to_string(timeoutMs) + "ms)");
        return nullptr;
    }
    
    auto conn = availableRead_.front();
    availableRead_.pop();
    
    LOG_DEBUG("获取读连接，剩余: " + std::to_string(availableRead_.size()));
    return conn;
}

void ConnectionPool::releaseRead(std::shared_ptr<Connection> conn) {
    if (!conn) return;
    
    std::unique_lock<std::mutex> lock(readMutex_);
    availableRead_.push(conn);
    readCV_.notify_one();
    
    LOG_DEBUG("释放读连接，可用: " + std::to_string(availableRead_.size()));
}

std::shared_ptr<Connection> ConnectionPool::acquireWrite(int timeoutMs) {
    if (!initialized_) {
        LOG_ERROR("ConnectionPool 未初始化");
        return nullptr;
    }
    
    std::unique_lock<std::mutex> lock(writeMutex_);
    
    bool available = writeCV_.wait_for(lock, 
        std::chrono::milliseconds(timeoutMs),
        [this] { return !writeInUse_; });
    
    if (!available) {
        LOG_ERROR("写连接被占用，等待超时 (timeout=" + std::to_string(timeoutMs) + "ms)");
        return nullptr;
    }
    
    writeInUse_ = true;
    LOG_DEBUG("获取写连接");
    return writeConn_;
}

void ConnectionPool::releaseWrite() {
    std::unique_lock<std::mutex> lock(writeMutex_);
    writeInUse_ = false;
    writeCV_.notify_one();
    LOG_DEBUG("释放写连接");
}

void ConnectionPool::shutdown() {
    if (!initialized_) return;
    
    std::unique_lock<std::mutex> readLock(readMutex_);
    std::unique_lock<std::mutex> writeLock(writeMutex_);
    
    for (auto& conn : readPool_) {
        if (conn) conn->close();
    }
    readPool_.clear();
    
    while (!availableRead_.empty()) {
        availableRead_.pop();
    }
    
    if (writeConn_) {
        writeConn_->close();
        writeConn_ = nullptr;
    }
    
    initialized_ = false;
    LOG_INFO("ConnectionPool 已关闭");
}

int ConnectionPool::getAvailableReadCount() const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(readMutex_));
    return static_cast<int>(availableRead_.size());
}

int ConnectionPool::getReadPoolSize() const {
    return readPoolSize_;
}

} // namespace database
} // namespace data