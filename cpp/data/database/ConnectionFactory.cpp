#include "ConnectionFactory.h"
#include "Connection.h"
#include "Config.h"
#include "Logger.h"
#include <stdexcept>

#ifdef HAS_MYSQL
#include "MySQLConnection.h"
#endif

namespace data {

IConnection& ConnectionFactory::createConnection() {
    auto& config = config::Config::getInstance();
    std::string dbType = config.getDbType();
    
    LOG_INFO("数据库类型: " + dbType);
    
#ifdef HAS_MYSQL
    if (dbType == "mysql") {
        LOG_INFO("使用 MySQL 连接: " + config.getDbHost() + ":" + 
                 std::to_string(config.getDbPort()) + "/" + config.getDbName());
        auto& conn = MySQLConnection::getInstance();
        conn.initialize(
            config.getDbHost(),
            config.getDbPort(),
            config.getDbName(),
            config.getDbUser(),
            config.getDbPassword(),
            config.getDbCharset()
        );
        return conn;
    }
#else
    if (dbType == "mysql") {
        LOG_ERROR("DB_TYPE=mysql 但 MySQL 支持未编译");
        LOG_ERROR("请安装 MySQL 开发库：");
        LOG_ERROR("  - Ubuntu: sudo apt-get install libmysqlclient-dev");
        LOG_ERROR("  - macOS: brew install mysql-client");
        LOG_ERROR("然后重新编译：cmake -S cpp -B cpp/build && cmake --build cpp/build");
        throw std::runtime_error("MySQL support not compiled. Cannot connect to MySQL database.");
    }
#endif
    
    std::string dbPath = config.getDbName() + ".db";
    LOG_INFO("使用 SQLite 连接: " + dbPath);
    auto& conn = Connection::getInstance();
    conn.initialize(dbPath);
    return conn;
}

} // namespace data