#include "ConnectionFactory.h"
#include "Connection.h"
#include "Config.h"
#include "Logger.h"

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
        LOG_WARN("MySQL 支持未编译，降级使用 SQLite");
        LOG_WARN("请安装 MySQL 库并重新编译以启用 MySQL 支持");
    }
#endif
    
    std::string dbPath = config.getDbName() + ".db";
    LOG_INFO("使用 SQLite 连接: " + dbPath);
    auto& conn = Connection::getInstance();
    conn.initialize(dbPath);
    return conn;
}

} // namespace data