#include "ConnectionManager.h"
#include "Logger.h"

namespace data {

ConnectionManager& ConnectionManager::getInstance() {
    static ConnectionManager instance;
    return instance;
}

void ConnectionManager::registerConnection(IConnection& connection) {
    std::lock_guard<std::mutex> lock(mutex_);
    connection_ = &connection;
    
#ifdef HAS_MYSQL
    mysqlConnection_ = dynamic_cast<MySQLConnection*>(&connection);
#else
    sqliteConnection_ = dynamic_cast<Connection*>(&connection);
#endif
    
    LOG_INFO("ConnectionManager: 已注册数据库连接");
}

IConnection& ConnectionManager::getConnection() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!connection_) {
        throw std::runtime_error("ConnectionManager: 未注册数据库连接");
    }
    return *connection_;
}

#ifdef HAS_MYSQL
MySQLConnection& ConnectionManager::getConnectionForDb() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!mysqlConnection_) {
        throw std::runtime_error("ConnectionManager: MySQL 连接未注册");
    }
    return *mysqlConnection_;
}

sqlpp::mysql::connection* ConnectionManager::getDb() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!mysqlConnection_) {
        throw std::runtime_error("ConnectionManager: MySQL 连接未注册");
    }
    return mysqlConnection_->getDb();
}
#else
Connection& ConnectionManager::getConnectionForDb() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!sqliteConnection_) {
        throw std::runtime_error("ConnectionManager: SQLite 连接未注册");
    }
    return *sqliteConnection_;
}

sqlpp::sqlite3::connection* ConnectionManager::getDb() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!sqliteConnection_) {
        throw std::runtime_error("ConnectionManager: SQLite 连接未注册");
    }
    return sqliteConnection_->getDb();
}
#endif

bool ConnectionManager::isConnected() const {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!connection_) {
        return false;
    }
    return connection_->isConnected();
}

} // namespace data