#ifndef DATA_MYSQL_CONNECTION_H
#define DATA_MYSQL_CONNECTION_H

#include <string>
#include <memory>
#include <mutex>
#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/mysql/mysql.h>

namespace data {

class MySQLConnection {
public:
    static MySQLConnection& getInstance();

    bool initialize(const std::string& host, int port, const std::string& database,
                    const std::string& user, const std::string& password,
                    const std::string& charset = "utf8mb4");

    bool connect();
    void disconnect();
    bool isConnected() const;

    sqlpp::mysql::connection* getDb();

    bool execute(const std::string& sql);
    bool beginTransaction();
    bool commit();
    bool rollback();
    bool createTables();

    MySQLConnection(const MySQLConnection&) = delete;
    MySQLConnection& operator=(const MySQLConnection&) = delete;

private:
    MySQLConnection() = default;
    ~MySQLConnection();

    bool executeInternal(const std::string& sql);

    std::unique_ptr<sqlpp::mysql::connection> db_;
    std::unique_ptr<sqlpp::mysql::connection_config> config_;
    bool connected_ = false;
    mutable std::mutex mutex_;
};

} // namespace data

#endif // DATA_MYSQL_CONNECTION_H