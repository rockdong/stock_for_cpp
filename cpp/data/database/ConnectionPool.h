#ifndef CONNECTION_POOL_H
#define CONNECTION_POOL_H

#include <memory>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <string>

namespace data {
namespace database {

// 前向声明
class Connection;

/**
 * 数据库连接池 - 读写分离
 * 
 * 读连接池: 多个连接并发读取 (支持 WAL 模式并发读)
 * 写连接: 单连接串行写入 (保证数据一致性)
 * 
 * 解决现有全局单例 g_db 的并发阻塞问题
 */
class ConnectionPool {
public:
    // 单例模式
    static ConnectionPool& getInstance();
    
    // 初始化连接池
    // @param readPoolSize 读连接池大小，默认 5
    // @param dbPath 数据库路径，默认从环境变量获取
    void initialize(int readPoolSize = 5, const std::string& dbPath = "");
    
    // 获取读连接（从池中）
    // @param timeoutMs 超时时间（毫秒），默认 5000
    // @return 连接指针，超时返回 nullptr
    std::shared_ptr<Connection> acquireRead(int timeoutMs = 5000);
    
    // 释放读连接（归还到池）
    void releaseRead(std::shared_ptr<Connection> conn);
    
    // 获取写连接（单例，带锁）
    // @param timeoutMs 超时时间（毫秒），默认 5000
    // @return 连接指针，超时返回 nullptr
    std::shared_ptr<Connection> acquireWrite(int timeoutMs = 5000);
    
    // 释放写连接
    void releaseWrite();
    
    // 关闭所有连接
    void shutdown();
    
    // 获取池状态
    int getAvailableReadCount() const;
    int getReadPoolSize() const;
    
    // 设置数据库路径（用于初始化）
    static void setDefaultDbPath(const std::string& path);
    static std::string getDefaultDbPath();

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
    std::condition_variable writeCV_;
    bool writeInUse_;
    
    std::string dbPath_;
    bool initialized_;
    
    static std::string defaultDbPath_;
};

} // namespace database
} // namespace data

#endif // CONNECTION_POOL_H