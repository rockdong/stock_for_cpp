#ifndef DATA_CONNECTION_H
#define DATA_CONNECTION_H

#include <string>
#include <memory>
#include <mutex>
#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/sqlite3/sqlite3.h>
#include "IConnection.h"

namespace data {

class Connection : public IConnection {
public:
    static Connection& getInstance();

    bool initialize(const std::string& dbPath);

    bool connect() override;
    void disconnect() override;
    bool isConnected() const override;

    sqlpp::sqlite3::connection* getDb();

    bool execute(const std::string& sql) override;
    bool beginTransaction() override;
    bool commit() override;
    bool rollback() override;
    bool createTables() override;

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

