#ifndef DATA_ICONNECTION_H
#define DATA_ICONNECTION_H

#include <string>

namespace data {

/**
 * @brief 数据库连接抽象接口
 * 定义 SQLite 和 MySQL 连接的共同操作
 * 注意：getDb() 方法不在接口中，因为返回类型不同
 *       DAO 层应根据数据库类型直接使用具体的 Connection 或 MySQLConnection 类
 */
class IConnection {
public:
    virtual ~IConnection() = default;

    /**
     * @brief 连接到数据库
     * @return 是否成功
     */
    virtual bool connect() = 0;

    /**
     * @brief 断开数据库连接
     */
    virtual void disconnect() = 0;

    /**
     * @brief 检查是否已连接
     * @return 是否已连接
     */
    virtual bool isConnected() const = 0;

    /**
     * @brief 执行 SQL 语句
     * @param sql SQL 语句
     * @return 是否成功
     */
    virtual bool execute(const std::string& sql) = 0;

    /**
     * @brief 开始事务
     * @return 是否成功
     */
    virtual bool beginTransaction() = 0;

    /**
     * @brief 提交事务
     * @return 是否成功
     */
    virtual bool commit() = 0;

    /**
     * @brief 回滚事务
     * @return 是否成功
     */
    virtual bool rollback() = 0;

    virtual bool createTables() = 0;

    virtual bool ensureAnalysisProgressRecord() = 0;
};

} // namespace data

#endif // DATA_ICONNECTION_H