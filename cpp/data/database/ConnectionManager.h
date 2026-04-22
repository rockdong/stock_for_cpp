#ifndef DATA_CONNECTIONMANAGER_H
#define DATA_CONNECTIONMANAGER_H

#include <mutex>
#include "IConnection.h"

#ifdef HAS_MYSQL
#include "MySQLConnection.h"
#else
#include "Connection.h"
#endif

namespace data {

class ConnectionManager {
public:
    static ConnectionManager& getInstance();

    void registerConnection(IConnection& connection);

    IConnection& getConnection();

#ifdef HAS_MYSQL
    MySQLConnection& getConnectionForDb();
    sqlpp::mysql::connection* getDb();
#else
    Connection& getConnectionForDb();
    sqlpp::sqlite3::connection* getDb();
#endif

    bool isConnected() const;

    ConnectionManager(const ConnectionManager&) = delete;
    ConnectionManager& operator=(const ConnectionManager&) = delete;

private:
    ConnectionManager() = default;
    ~ConnectionManager() = default;

#ifdef HAS_MYSQL
    MySQLConnection* mysqlConnection_ = nullptr;
#else
    Connection* sqliteConnection_ = nullptr;
#endif

    IConnection* connection_ = nullptr;
    mutable std::mutex mutex_;
};

} // namespace data

#endif // DATA_CONNECTIONMANAGER_H