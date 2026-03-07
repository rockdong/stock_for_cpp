#ifndef DATA_CONNECTION_H
#define DATA_CONNECTION_H

#include <string>
#include <memory>
#include <mutex>
#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/sqlite3/sqlite3.h>

namespace data {

/**
 * @brief 数据库连接管理类
 * 单例模式：全局唯一的数据库连接管理器
 * 职责：管理数据库连接的生命周期
 * 使用 sqlpp11 提供类型安全的 SQL 查询构建
 */
class Connection {
public:
    /**
     * @brief 获取单例实例
     * @return Connection 引用
     */
    static Connection& getInstance();

    /**
     * @brief 初始化数据库连接
     * @param dbPath 数据库文件路径
     * @return 是否成功
     */
    bool initialize(const std::string& dbPath);

    /**
     * @brief 连接到数据库
     * @return 是否成功
     */
    bool connect();

    /**
     * @brief 断开数据库连接
     */
    void disconnect();

    /**
     * @brief 检查是否已连接
     * @return 是否已连接
     */
    bool isConnected() const;

    /**
     * @brief 获取 sqlpp11 数据库连接
     * @return sqlpp11::sqlite3::connection 指针
     */
    sqlpp::sqlite3::connection* getDb();

    /**
     * @brief 执行 SQL 语句
     * @param sql SQL 语句
     * @return 是否成功
     */
    bool execute(const std::string& sql);

    /**
     * @brief 开始事务
     * @return 是否成功
     */
    bool beginTransaction();

    /**
     * @brief 提交事务
     * @return 是否成功
     */
    bool commit();

    /**
     * @brief 回滚事务
     * @return 是否成功
     */
    bool rollback();

    /**
     * @brief 创建数据库表
     * @return 是否成功
     */
    bool createTables();

    // 禁用拷贝和赋值
    Connection(const Connection&) = delete;
    Connection& operator=(const Connection&) = delete;

private:
    Connection() = default;
    ~Connection();

    /**
     * @brief 内部执行 SQL 语句（不加锁，假设调用者已持有锁）
     * @param sql SQL 语句
     * @return 是否成功
     */
    bool executeInternal(const std::string& sql);

    std::unique_ptr<sqlpp::sqlite3::connection> db_;
    std::string dbPath_;
    bool connected_ = false;
    mutable std::mutex mutex_;
};

} // namespace data

#endif // DATA_CONNECTION_H

