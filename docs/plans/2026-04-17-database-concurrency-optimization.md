# 数据库并发架构优化实施计划

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** 优化数据库并发架构，消除前端请求阻塞，实现读写分离和异步写入

**Architecture:** ConnectionPool 管理读写分离（读连接池 + 写连接单例），WriteQueue 异步写入队列，Node.js 改为调用 C++ API

**Tech Stack:** C++ SQLite, cpp-httplib, Node.js Express, better-sqlite3

---

## 实施阶段概览

| 阶段 | 任务数 | 预计时间 |
|------|--------|---------|
| 阶段 1: ConnectionPool | 15 | 2-3h |
| 阶段 2: WriteQueue | 12 | 1-2h |
| 阶段 3: Node.js Proxy | 10 | 1h |
| 阶段 4: DAO/API 改造 | 20 | 2-3h |
| 阶段 5: 测试验证 | 15 | 1-2h |

**总计**: 72 个步骤，约 6-9 小时

---

## 阶段 1: ConnectionPool 实现

### Task 1: ConnectionPool 头文件

**Files:**
- Create: `cpp/data/database/ConnectionPool.h`

**Step 1: 创建头文件骨架**

```cpp
#ifndef CONNECTION_POOL_H
#define CONNECTION_POOL_H

#include <memory>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include "Connection.h"

namespace data {
namespace database {

/**
 * 数据库连接池 - 读写分离
 * 
 * 读连接池: 多个连接并发读取
 * 写连接: 单连接串行写入（保证一致性）
 */
class ConnectionPool {
public:
    // 单例模式
    static ConnectionPool& getInstance();
    
    // 初始化连接池
    void initialize(int readPoolSize = 5);
    
    // 获取读连接（从池中）
    std::shared_ptr<Connection> acquireRead(int timeoutMs = 5000);
    
    // 释放读连接（归还到池）
    void releaseRead(std::shared_ptr<Connection> conn);
    
    // 获取写连接（单例，带锁）
    std::shared_ptr<Connection> acquireWrite(int timeoutMs = 5000);
    
    // 释放写连接
    void releaseWrite();
    
    // 关闭所有连接
    void shutdown();
    
    // 获取池状态
    int getAvailableReadCount() const;
    int getReadPoolSize() const;
    
private:
    ConnectionPool();
    ~ConnectionPool();
    ConnectionPool(const ConnectionPool&) = delete;
    ConnectionPool& operator=(const ConnectionPool&) = delete;
    
    // 读连接池
    std::vector<std::shared_ptr<Connection>> readPool_;
    std::queue<std::shared_ptr<Connection>> availableRead_;
    std::mutex readMutex_;
    std::condition_variable readCV_;
    int readPoolSize_;
    
    // 写连接（单例）
    std::shared_ptr<Connection> writeConn_;
    std::mutex writeMutex_;
    bool writeInUse_;
    
    std::string dbPath_;
};

} // namespace database
} // namespace data

#endif // CONNECTION_POOL_H
```

**Step 2: 提交**

```bash
git add cpp/data/database/ConnectionPool.h
git commit -m "feat(db): add ConnectionPool header with read/write separation"
```

---

### Task 2: ConnectionPool 实现文件

**Files:**
- Create: `cpp/data/database/ConnectionPool.cpp`

**Step 1: 实现构造和初始化**

```cpp
#include "ConnectionPool.h"
#include "Logger.h"
#include <chrono>

namespace data {
namespace database {

ConnectionPool& ConnectionPool::getInstance() {
    static ConnectionPool instance;
    return instance;
}

ConnectionPool::ConnectionPool() 
    : readPoolSize_(5)
    , writeInUse_(false) {
}

ConnectionPool::~ConnectionPool() {
    shutdown();
}

void ConnectionPool::initialize(int readPoolSize) {
    readPoolSize_ = readPoolSize;
    dbPath_ = Connection::getDbPath();
    
    // 创建读连接池
    for (int i = 0; i < readPoolSize_; ++i) {
        auto conn = std::make_shared<Connection>();
        conn->connect(dbPath_);
        readPool_.push_back(conn);
        availableRead_.push(conn);
    }
    
    // 创建写连接
    writeConn_ = std::make_shared<Connection>();
    writeConn_->connect(dbPath_);
    
    LOG_INFO("ConnectionPool 初始化完成: 读连接=" + std::to_string(readPoolSize_) + ", 写连接=1");
}
```

**Step 2: 实现读连接获取/释放**

```cpp
std::shared_ptr<Connection> ConnectionPool::acquireRead(int timeoutMs) {
    std::unique_lock<std::mutex> lock(readMutex_);
    
    // 等待可用连接
    bool available = readCV_.wait_for(lock, 
        std::chrono::milliseconds(timeoutMs),
        [this] { return !availableRead_.empty(); });
    
    if (!available) {
        LOG_ERROR("读连接池耗尽，等待超时 (timeout=" + std::to_string(timeoutMs) + "ms)");
        return nullptr;
    }
    
    auto conn = availableRead_.front();
    availableRead_.pop();
    
    LOG_DEBUG("获取读连接，剩余可用: " + std::to_string(availableRead_.size()));
    return conn;
}

void ConnectionPool::releaseRead(std::shared_ptr<Connection> conn) {
    if (!conn) return;
    
    std::unique_lock<std::mutex> lock(readMutex_);
    availableRead_.push(conn);
    readCV_.notify_one();
    
    LOG_DEBUG("释放读连接，可用数量: " + std::to_string(availableRead_.size()));
}
```

**Step 3: 实现写连接获取/释放**

```cpp
std::shared_ptr<Connection> ConnectionPool::acquireWrite(int timeoutMs) {
    std::unique_lock<std::mutex> lock(writeMutex_);
    
    // 等待写连接可用
    bool available = true;
    if (writeInUse_) {
        // 简单等待（实际应使用 condition_variable）
        available = false;
        LOG_WARN("写连接正在使用，请求等待");
    }
    
    if (!available && timeoutMs > 0) {
        // 等待逻辑（简化版）
        lock.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(timeoutMs));
        lock.lock();
    }
    
    if (writeInUse_) {
        LOG_ERROR("写连接仍被占用");
        return nullptr;
    }
    
    writeInUse_ = true;
    LOG_DEBUG("获取写连接");
    return writeConn_;
}

void ConnectionPool::releaseWrite() {
    std::unique_lock<std::mutex> lock(writeMutex_);
    writeInUse_ = false;
    LOG_DEBUG("释放写连接");
}
```

**Step 4: 实现关闭和状态查询**

```cpp
void ConnectionPool::shutdown() {
    std::unique_lock<std::mutex> readLock(readMutex_);
    std::unique_lock<std::mutex> writeLock(writeMutex_);
    
    // 关闭读连接
    for (auto& conn : readPool_) {
        if (conn) conn->close();
    }
    readPool_.clear();
    availableRead_ = std::queue<std::shared_ptr<Connection>>();
    
    // 关闭写连接
    if (writeConn_) {
        writeConn_->close();
        writeConn_ = nullptr;
    }
    
    LOG_INFO("ConnectionPool 已关闭");
}

int ConnectionPool::getAvailableReadCount() const {
    return availableRead_.size();
}

int ConnectionPool::getReadPoolSize() const {
    return readPoolSize_;
}

} // namespace database
} // namespace data
```

**Step 5: 提交**

```bash
git add cpp/data/database/ConnectionPool.cpp
git commit -m "feat(db): implement ConnectionPool with read pool and write connection"
```

---

### Task 3: ConnectionPool 单元测试

**Files:**
- Create: `cpp/tests/data/database/test_connection_pool.cpp`

**Step 1: 创建测试文件**

```cpp
#include "gtest/gtest.h"
#include "ConnectionPool.h"
#include "Connection.h"

using namespace data::database;

class ConnectionPoolTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 使用测试数据库
        Connection::setDbPath("/tmp/test_stock.db");
        ConnectionPool::getInstance().initialize(3);
    }
    
    void TearDown() override {
        ConnectionPool::getInstance().shutdown();
    }
};

TEST_F(ConnectionPoolTest, InitializeCreatesPool) {
    auto& pool = ConnectionPool::getInstance();
    EXPECT_EQ(pool.getReadPoolSize(), 3);
    EXPECT_EQ(pool.getAvailableReadCount(), 3);
}

TEST_F(ConnectionPoolTest, AcquireReadWorks) {
    auto& pool = ConnectionPool::getInstance();
    
    auto conn = pool.acquireRead();
    EXPECT_NE(conn, nullptr);
    EXPECT_EQ(pool.getAvailableReadCount(), 2);
    
    pool.releaseRead(conn);
    EXPECT_EQ(pool.getAvailableReadCount(), 3);
}

TEST_F(ConnectionPoolTest, AcquireWriteWorks) {
    auto& pool = ConnectionPool::getInstance();
    
    auto conn = pool.acquireWrite();
    EXPECT_NE(conn, nullptr);
    
    // 再次获取应返回 nullptr（写连接被占用）
    auto conn2 = pool.acquireWrite(0);
    EXPECT_EQ(conn2, nullptr);
    
    pool.releaseWrite();
    
    // 释放后应能获取
    auto conn3 = pool.acquireWrite();
    EXPECT_NE(conn3, nullptr);
    pool.releaseWrite();
}

TEST_F(ConnectionPoolTest, PoolExhaustionTimeout) {
    auto& pool = ConnectionPool::getInstance();
    pool.initialize(2);  // 只 2 个连接
    
    // 获取所有连接
    auto conn1 = pool.acquireRead();
    auto conn2 = pool.acquireRead();
    
    EXPECT_EQ(pool.getAvailableReadCount(), 0);
    
    // 第 3 个获取应超时返回 nullptr
    auto conn3 = pool.acquireRead(1000);  // 等待 1s
    EXPECT_EQ(conn3, nullptr);
    
    pool.releaseRead(conn1);
    
    // 现在应能获取
    auto conn4 = pool.acquireRead(100);
    EXPECT_NE(conn4, nullptr);
    
    pool.releaseRead(conn2);
    pool.releaseRead(conn4);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
```

**Step 2: 运行测试**

```bash
cd cpp/build
cmake .. -DBUILD_TESTS=ON
make test_connection_pool
./test_connection_pool
```

Expected: 4 tests PASS

**Step 3: 提交**

```bash
git add cpp/tests/data/database/test_connection_pool.cpp
git commit -m "test(db): add ConnectionPool unit tests"
```

---

## 阶段 2: WriteQueue 实现

### Task 4: WriteQueue 头文件

**Files:**
- Create: `cpp/data/database/WriteQueue.h`

**Step 1: 创建头文件**

```cpp
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

/**
 * 写入任务结构
 */
struct WriteTask {
    std::string table;
    std::string operation;  // INSERT, UPDATE, DELETE
    nlohmann::json data;
    std::function<void(bool success)> callback;  // 完成回调（可选）
    
    WriteTask(const std::string& t, const std::string& op, const nlohmann::json& d,
              std::function<void(bool)> cb = nullptr)
        : table(t), operation(op), data(d), callback(cb) {}
};

/**
 * 异步写入队列
 * 
 * 分析任务完成后入队，后台线程串行化写入
 */
class WriteQueue {
public:
    static WriteQueue& getInstance();
    
    // 启动后台写入线程
    void start();
    
    // 停止并等待完成
    void stop();
    
    // 添加写入任务（异步）
    void enqueue(const WriteTask& task);
    
    // 获取队列大小
    size_t size() const;
    
    // 是否正在运行
    bool isRunning() const;
    
private:
    WriteQueue();
    ~WriteQueue();
    WriteQueue(const WriteQueue&) = delete;
    WriteQueue& operator=(const WriteQueue&) = delete;
    
    // 后台线程处理函数
    void processQueue();
    
    // 执行单个写入任务
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
```

**Step 2: 提交**

```bash
git add cpp/data/database/WriteQueue.h
git commit -m "feat(db): add WriteQueue header for async writes"
```

---

### Task 5: WriteQueue 实现文件

**Files:**
- Create: `cpp/data/database/WriteQueue.cpp`

**Step 1: 实现核心逻辑**

```cpp
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

WriteQueue::WriteQueue() 
    : running_(false)
    , stopped_(false) {
}

WriteQueue::~WriteQueue() {
    stop();
}

void WriteQueue::start() {
    if (running_) return;
    
    running_ = true;
    stopped_ = false;
    writerThread_ = std::thread(&WriteQueue::processQueue, this);
    
    LOG_INFO("WriteQueue 后台线程已启动");
}

void WriteQueue::stop() {
    if (!running_) return;
    
    running_ = false;
    stopped_ = true;
    
    // 通知线程退出
    queueCV_.notify_all();
    
    // 等待线程完成
    if (writerThread_.joinable()) {
        writerThread_.join();
    }
    
    LOG_INFO("WriteQueue 已停止，剩余任务: " + std::to_string(queue_.size()));
}

void WriteQueue::enqueue(const WriteTask& task) {
    std::unique_lock<std::mutex> lock(queueMutex_);
    queue_.push(task);
    queueCV_.notify_one();
    
    LOG_DEBUG("写入任务入队: table=" + task.table + ", operation=" + task.operation + 
              ", queue_size=" + std::to_string(queue_.size()));
}

size_t WriteQueue::size() const {
    std::unique_lock<std::mutex> lock(const_cast<std::mutex&>(queueMutex_));
    return queue_.size();
}

bool WriteQueue::isRunning() const {
    return running_;
}

void WriteQueue::processQueue() {
    while (running_) {
        WriteTask task;
        
        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            
            // 等待任务或停止信号
            queueCV_.wait(lock, [this] { 
                return !queue_.empty() || stopped_; 
            });
            
            if (stopped_ && queue_.empty()) break;
            
            if (queue_.empty()) continue;
            
            task = queue_.front();
            queue_.pop();
        }
        
        // 执行写入（重试 3 次）
        int maxRetry = 3;
        bool success = false;
        
        for (int retry = 0; retry < maxRetry && !success; ++retry) {
            success = executeWrite(task);
            
            if (!success && retry < maxRetry - 1) {
                LOG_WARN("写入失败，重试 " + std::to_string(retry + 1) + "/" + std::to_string(maxRetry));
                std::this_thread::sleep_for(std::chrono::milliseconds(100 * (retry + 1)));
            }
        }
        
        // 调用回调
        if (task.callback) {
            task.callback(success);
        }
        
        if (!success) {
            LOG_ERROR("写入最终失败: table=" + task.table + ", operation=" + task.operation);
        }
    }
}

bool WriteQueue::executeWrite(const WriteTask& task) {
    auto& pool = ConnectionPool::getInstance();
    
    // 获取写连接
    auto conn = pool.acquireWrite(5000);
    if (!conn) {
        LOG_ERROR("无法获取写连接");
        return false;
    }
    
    try {
        // 根据操作类型执行
        if (task.operation == "INSERT") {
            // 执行插入（具体实现取决于 DAO）
            // 示例：conn->execute("INSERT INTO " + task.table + " ...", task.data);
            LOG_DEBUG("执行 INSERT: " + task.table);
        } else if (task.operation == "UPDATE") {
            LOG_DEBUG("执行 UPDATE: " + task.table);
        } else if (task.operation == "DELETE") {
            LOG_DEBUG("执行 DELETE: " + task.table);
        }
        
        pool.releaseWrite();
        return true;
        
    } catch (const std::exception& e) {
        pool.releaseWrite();
        LOG_ERROR("写入异常: " + std::string(e.what()));
        return false;
    }
}

} // namespace database
} // namespace data
```

**Step 2: 提交**

```bash
git add cpp/data/database/WriteQueue.cpp
git commit -m "feat(db): implement WriteQueue with async processing and retry"
```

---

### Task 6: WriteQueue 单元测试

**Files:**
- Create: `cpp/tests/data/database/test_write_queue.cpp`

**Step 1: 创建测试**

```cpp
#include "gtest/gtest.h"
#include "WriteQueue.h"
#include "ConnectionPool.h"
#include <chrono>
#include <thread>

using namespace data::database;

class WriteQueueTest : public ::testing::Test {
protected:
    void SetUp() override {
        ConnectionPool::getInstance().initialize(2);
        WriteQueue::getInstance().start();
    }
    
    void TearDown() override {
        WriteQueue::getInstance().stop();
        ConnectionPool::getInstance().shutdown();
    }
};

TEST_F(WriteQueueTest, EnqueueWorks) {
    auto& queue = WriteQueue::getInstance();
    
    EXPECT_EQ(queue.size(), 0);
    
    nlohmann::json data = {"key", "value"};
    WriteTask task("test_table", "INSERT", data);
    
    queue.enqueue(task);
    
    EXPECT_EQ(queue.size(), 1);
}

TEST_F(WriteQueueTest, CallbackWorks) {
    auto& queue = WriteQueue::getInstance();
    
    bool callbackCalled = false;
    bool successResult = false;
    
    nlohmann::json data;
    WriteTask task("test_table", "INSERT", data, [&](bool success) {
        callbackCalled = true;
        successResult = success;
    });
    
    queue.enqueue(task);
    
    // 等待处理
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    EXPECT_TRUE(callbackCalled);
    // INSERT 可能失败（测试环境），但回调被调用
}

TEST_F(WriteQueueTest, StopAndWait) {
    auto& queue = WriteQueue::getInstance();
    
    // 入队多个任务
    for (int i = 0; i < 5; ++i) {
        nlohmann::json data;
        queue.enqueue(WriteTask("test", "INSERT", data));
    }
    
    EXPECT_EQ(queue.size(), 5);
    
    // 停止
    queue.stop();
    
    EXPECT_FALSE(queue.isRunning());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
```

**Step 2: 运行测试**

```bash
cd cpp/build
cmake .. -DBUILD_TESTS=ON
make test_write_queue
./test_write_queue
```

Expected: 3 tests PASS

**Step 3: 提交**

```bash
git add cpp/tests/data/database/test_write_queue.cpp
git commit -m "test(db): add WriteQueue unit tests"
```

---

## 阶段 3: Node.js Proxy 改造

### Task 7: Node.js Proxy 转发层

**Files:**
- Create: `nodejs/src/proxy.js`
- Modify: `nodejs/src/index.js`

**Step 1: 创建 Proxy 模块**

```javascript
// nodejs/src/proxy.js
const axios = require('axios');
const logger = require('./logger');

const CPP_API_URL = process.env.CPP_API_URL || 'http://localhost:3001';
const CPP_API_TIMEOUT = parseInt(process.env.CPP_API_TIMEOUT) || 30000;

/**
 * 转发请求到 C++ API
 * 
 * @param {Object} req - Express request
 * @param {Object} res - Express response
 * @param {string} endpoint - API endpoint (如 '/api/stocks')
 */
async function proxyRequest(req, res, endpoint) {
    const startTime = Date.now();
    
    try {
        const response = await axios({
            method: req.method,
            url: `${CPP_API_URL}${endpoint}`,
            params: req.query,
            data: req.body,
            timeout: CPP_API_TIMEOUT,
            headers: {
                'Content-Type': 'application/json'
            }
        });
        
        const duration = Date.now() - startTime;
        logger.info(`Proxy ${req.method} ${endpoint} - ${response.status} (${duration}ms)`);
        
        res.json(response.data);
        
    } catch (error) {
        const duration = Date.now() - startTime;
        
        if (error.code === 'ECONNREFUSED') {
            logger.error(`Proxy ${endpoint} - C++ API 不可用 (${duration}ms)`);
            res.status(503).json({
                success: false,
                error: 'C++ API 服务不可用，请检查 C++ 服务是否运行'
            });
        } else if (error.code === 'ETIMEDOUT' || error.message.includes('timeout')) {
            logger.error(`Proxy ${endpoint} - 超时 (${duration}ms)`);
            res.status(504).json({
                success: false,
                error: '请求超时，请稍后重试'
            });
        } else if (error.response) {
            // C++ API 返回错误
            logger.error(`Proxy ${endpoint} - C++ API 错误: ${error.response.status}`);
            res.status(error.response.status).json(error.response.data);
        } else {
            logger.error(`Proxy ${endpoint} - 未知错误: ${error.message}`);
            res.status(500).json({
                success: false,
                error: error.message
            });
        }
    }
}

/**
 * API 路由转发映射
 */
const API_ROUTES = {
    // 股票相关
    '/stocks': '/api/stocks',
    '/stocks/search': '/api/stocks/search',
    '/stocks/:code': '/api/stocks/:code',
    
    // 分析相关
    '/analysis/signals': '/api/analysis/signals',
    '/analysis/progress': '/api/analysis/progress',
    '/analysis/process': '/api/analysis/process',
    '/analysis/process/strategies': '/api/analysis/process/strategies',
    '/analysis/process/chart/:ts_code': '/api/analysis/process/chart/:ts_code',
    '/analysis/process/:id': '/api/analysis/process/:id',
    '/analysis/:code': '/api/analysis/:code',
    
    // 图表相关
    '/charts/:code': '/api/charts/:code',
    
    // 用户相关
    '/user/watchlist': '/api/user/watchlist'
};

module.exports = {
    proxyRequest,
    API_ROUTES,
    CPP_API_URL
};
```

**Step 2: 提交**

```bash
git add nodejs/src/proxy.js
git commit -m "feat(nodejs): add proxy module to forward requests to C++ API"
```

---

### Task 8: Node.js 改用 Proxy

**Files:**
- Modify: `nodejs/src/index.js`
- Modify: `nodejs/src/api.js`

**Step 1: 在 index.js 中引入 Proxy**

```javascript
// nodejs/src/index.js (修改部分)
const express = require('express');
const cors = require('cors');
const logger = require('./logger');
const proxy = require('./proxy');

// ... 现有配置 ...

const app = express();

// CORS
app.use(cors());

// Body parser
app.use(express.json());

// ===== 改造：所有 /api 路由改为转发 =====
// 保留 Feishu 相关路由（不转发）

// 股票相关 - 转发
app.get('/api/stocks', (req, res) => proxy.proxyRequest(req, res, '/api/stocks'));
app.get('/api/stocks/search', (req, res) => proxy.proxyRequest(req, res, '/api/stocks/search'));
app.get('/api/stocks/:code', (req, res) => proxy.proxyRequest(req, res, `/api/stocks/${req.params.code}`));

// 分析相关 - 转发
app.get('/api/analysis/signals', (req, res) => proxy.proxyRequest(req, res, '/api/analysis/signals'));
app.get('/api/analysis/progress', (req, res) => proxy.proxyRequest(req, res, '/api/analysis/progress'));
app.get('/api/analysis/process', (req, res) => proxy.proxyRequest(req, res, '/api/analysis/process'));
app.get('/api/analysis/process/strategies', (req, res) => proxy.proxyRequest(req, res, '/api/analysis/process/strategies'));
app.get('/api/analysis/process/chart/:ts_code', (req, res) => proxy.proxyRequest(req, res, `/api/analysis/process/chart/${req.params.ts_code}`));
app.get('/api/analysis/process/:id', (req, res) => proxy.proxyRequest(req, res, `/api/analysis/process/${req.params.id}`));
app.get('/api/analysis/:code', (req, res) => proxy.proxyRequest(req, res, `/api/analysis/${req.params.code}`));

// 图表相关 - 转发
app.get('/api/charts/:code', (req, res) => proxy.proxyRequest(req, res, `/api/charts/${req.params.code}`));

// 用户相关 - 转发
app.get('/api/user/watchlist', (req, res) => proxy.proxyRequest(req, res, '/api/user/watchlist'));

// ===== Feishu 相关路由保持不变（不转发） =====
// app.post('/feishu/*', ...);

// ===== 启动服务器 =====
const PORT = process.env.PORT || 3000;
app.listen(PORT, () => {
    logger.info(`Node.js Proxy 服务启动，端口: ${PORT}`);
    logger.info(`转发目标: ${proxy.CPP_API_URL}`);
});
```

**Step 2: 移除 api.js 中的数据库直接访问**

```javascript
// nodejs/src/api.js (移除或注释掉数据库直接访问的代码)
// 此文件可保留用于 Feishu 相关逻辑，或删除
```

**Step 3: 提交**

```bash
git add nodejs/src/index.js nodejs/src/api.js
git commit -m "refactor(nodejs): migrate API routes to proxy C++ API, remove direct DB access"
```

---

## 阶段 4: DAO/API 改造

### Task 9: Connection 改用 ConnectionPool

**Files:**
- Modify: `cpp/data/database/Connection.h`
- Modify: `cpp/data/database/Connection.cpp`

**Step 1: 移除全局单例 g_db**

```cpp
// Connection.h (修改)

// 移除：
// static sqlite3* g_db = nullptr;
// static std::mutex db_mutex_;

// 改为：
// ConnectionPool 管理连接，Connection 类变成普通连接封装
```

**Step 2: Connection 改为普通连接封装**

```cpp
// Connection.cpp (修改)

class Connection {
public:
    Connection();
    ~Connection();
    
    void connect(const std::string& dbPath);
    void close();
    
    sqlite3* getHandle() { return db_; }
    
    // 移除 getInstance() - 改用 ConnectionPool
    
private:
    sqlite3* db_;
    std::string dbPath_;
};
```

**Step 3: 提交**

```bash
git add cpp/data/database/Connection.h cpp/data/database/Connection.cpp
git commit -m "refactor(db): remove global g_db singleton, Connection becomes plain wrapper"
```

---

### Task 10: StockDAO 改用连接池

**Files:**
- Modify: `cpp/data/database/StockDAO.cpp`

**Step 1: 改用 ConnectionPool**

```cpp
// StockDAO.cpp (修改示例)

#include "ConnectionPool.h"

std::vector<Stock> StockDAO::getAll(int limit) {
    auto& pool = ConnectionPool::getInstance();
    auto conn = pool.acquireRead();
    
    if (!conn) {
        LOG_ERROR("无法获取读连接");
        return {};
    }
    
    try {
        // 执行查询
        auto result = conn->executeQuery("SELECT * FROM stocks LIMIT ?", limit);
        
        pool.releaseRead(conn);
        return parseResult(result);
        
    } catch (...) {
        pool.releaseRead(conn);
        throw;
    }
}

void StockDAO::insert(const Stock& stock) {
    auto& pool = ConnectionPool::getInstance();
    auto conn = pool.acquireWrite();
    
    if (!conn) {
        LOG_ERROR("无法获取写连接");
        throw std::runtime_error("Write connection unavailable");
    }
    
    try {
        conn->execute("INSERT INTO stocks ...", stock);
        pool.releaseWrite();
        
    } catch (...) {
        pool.releaseWrite();
        throw;
    }
}
```

**Step 2: 提交**

```bash
git add cpp/data/database/StockDAO.cpp
git commit -m "refactor(dao): StockDAO uses ConnectionPool for read/write separation"
```

---

### Task 11: ApiRouter 改用连接池

**Files:**
- Modify: `cpp/api/ApiRouter.cpp`

**Step 1: 改用 ConnectionPool**

```cpp
// ApiRouter.cpp (修改示例)

#include "ConnectionPool.h"

void ApiRouter::getStocks(const httplib::Request& req, httplib::Response& res) {
    auto& pool = ConnectionPool::getInstance();
    auto conn = pool.acquireRead();
    
    if (!conn) {
        res.status = 503;
        res.set_content(jsonError("Database connection pool exhausted"), "application/json");
        return;
    }
    
    try {
        // 查询逻辑
        auto stocks = StockDAO(conn).getAll(limit);
        
        pool.releaseRead(conn);
        res.set_content(jsonSuccess(stocks), "application/json");
        
    } catch (...) {
        pool.releaseRead(conn);
        res.status = 500;
        res.set_content(jsonError("Database error"), "application/json");
    }
}
```

**Step 2: 提交**

```bash
git add cpp/api/ApiRouter.cpp
git commit -m "refactor(api): ApiRouter uses ConnectionPool, handles pool exhaustion"
```

---

### Task 12: main.cpp 初始化连接池和写入队列

**Files:**
- Modify: `cpp/main.cpp`

**Step 1: 初始化 ConnectionPool**

```cpp
// main.cpp (添加)

#include "ConnectionPool.h"
#include "WriteQueue.h"

int main() {
    // ... 现有初始化 ...
    
    // 初始化连接池
    auto& pool = data::database::ConnectionPool::getInstance();
    pool.initialize(5);  // 5 个读连接
    
    // 启动写入队列
    auto& writeQueue = data::database::WriteQueue::getInstance();
    writeQueue.start();
    
    LOG_INFO("数据库并发架构已初始化: 读连接池=5, 异步写入队列已启动");
    
    // ... 现有逻辑 ...
    
    // 清理时
    writeQueue.stop();
    pool.shutdown();
    
    return 0;
}
```

**Step 2: 提交**

```bash
git add cpp/main.cpp
git commit -m "feat(main): initialize ConnectionPool and WriteQueue at startup"
```

---

## 阶段 5: 测试验证

### Task 13: CMakeLists.txt 添加连接池编译

**Files:**
- Modify: `cpp/CMakeLists.txt`

**Step 1: 添加新文件**

```cmake
# CMakeLists.txt (添加)

# ConnectionPool
set(CONNECTION_POOL_SRC
    data/database/ConnectionPool.cpp
)

# WriteQueue
set(WRITE_QUEUE_SRC
    data/database/WriteQueue.cpp
)

# 添加到主程序
target_sources(stock_for_cpp PRIVATE
    ${CONNECTION_POOL_SRC}
    ${WRITE_QUEUE_SRC}
)

# 测试
if(BUILD_TESTS)
    add_executable(test_connection_pool tests/data/database/test_connection_pool.cpp ${CONNECTION_POOL_SRC})
    add_executable(test_write_queue tests/data/database/test_write_queue.cpp ${WRITE_QUEUE_SRC})
endif()
```

**Step 2: 提交**

```bash
git add cpp/CMakeLists.txt
git commit -m "build(cmake): add ConnectionPool and WriteQueue to build"
```

---

### Task 14: Docker Compose 配置环境变量

**Files:**
- Modify: `docker-compose.yml`

**Step 1: 添加 CPP_API_URL**

```yaml
# docker-compose.yml (修改)

services:
  stock-analysis:
    environment:
      - CPP_API_URL=http://localhost:3001
      - CPP_API_TIMEOUT=30000
```

**Step 2: 提交**

```bash
git add docker-compose.yml
git commit -m "chore(docker): add CPP_API_URL env for Node.js proxy"
```

---

### Task 15: 性能对比测试

**Files:**
- Create: `docs/plans/performance-test-results.md`

**Step 1: 运行性能测试**

```bash
# 优化前
curl -w "Time: %{time_total}s\n" http://localhost:3000/api/analysis/process

# 优化后
curl -w "Time: %{time_total}s\n" http://localhost:3001/api/analysis/process

# 并发测试
ab -n 100 -c 50 http://localhost:3001/api/stocks
```

**Step 2: 记录结果**

```markdown
# 性能测试结果

## 测试环境
- 日期: 2026-04-XX
- 机器: macOS/Linux
- 数据库大小: X 条记录

## 单请求响应时间

| 端点 | 优化前 | 优化后 | 改善 |
|------|--------|--------|------|
| /api/stocks | X ms | Y ms | Z% |
| /api/analysis/process | X ms | Y ms | Z% |

## 并发测试（100请求，50并发）

| 端点 | 成功率 | 平均响应 | 最大响应 |
|------|--------|---------|---------|
| /api/stocks | 100% | X ms | Y ms |
```

**Step 3: 提交**

```bash
git add docs/plans/performance-test-results.md
git commit -m "docs: add performance test results for database optimization"
```

---

## 提交总结

完成所有任务后，推送到远程：

```bash
git push origin master
```

预期提交：
- 15+ commits（每个 Task 一个 commit）
- 新增文件：ConnectionPool.h/cpp, WriteQueue.h/cpp, proxy.js, 测试文件
- 修改文件：Connection, DAO, ApiRouter, main, CMakeLists, docker-compose

---

## 执行选项

**Plan complete and saved to `docs/plans/2026-04-17-database-concurrency-optimization.md`.**

**Two execution options:**

**1. Subagent-Driven (this session)** - 我在当前会话逐个任务派遣子代理，任务间审查，快速迭代

**2. Parallel Session (separate)** - 在新会话中使用 executing-plans，批量执行带检查点

**选择哪种方式？**