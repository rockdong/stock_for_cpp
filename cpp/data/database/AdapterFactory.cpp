#include "AdapterFactory.h"
#include "Logger.h"

namespace data {

std::unique_ptr<IDatabaseAdapter> AdapterFactory::create(const AdapterConfig& config) {
    if (config.dbType == "mysql") {
#ifdef HAS_MYSQL
        auto adapter = std::make_unique<MySQLAdapter>();
        MySQLAdapter& mysqlAdapter = dynamic_cast<MySQLAdapter&>(*adapter);
        mysqlAdapter.initialize(config.host, config.port, 
                                config.database, config.user, 
                                config.password, config.charset);
        LOG_INFO("创建 MySQL Adapter");
        return adapter;
#else
        LOG_WARN("MySQL 支持未编译，降级使用 SQLite");
        auto adapter = std::make_unique<SQLiteAdapter>();
        SQLiteAdapter& sqliteAdapter = dynamic_cast<SQLiteAdapter&>(*adapter);
        sqliteAdapter.initialize(config.dbPath.empty() ? "stock_db.db" : config.dbPath);
        return adapter;
#endif
    }
    
    auto adapter = std::make_unique<SQLiteAdapter>();
    SQLiteAdapter& sqliteAdapter = dynamic_cast<SQLiteAdapter&>(*adapter);
    sqliteAdapter.initialize(config.dbPath.empty() ? "stock_db.db" : config.dbPath);
    LOG_INFO("创建 SQLite Adapter");
    return adapter;
}

std::unique_ptr<IDatabaseAdapter> AdapterFactory::createFromConfig() {
    auto& config = config::Config::getInstance();
    
    AdapterConfig adapterConfig;
    adapterConfig.dbType = config.getDbType();
    adapterConfig.dbPath = config.getDbName() + ".db";
    adapterConfig.host = config.getDbHost();
    adapterConfig.port = config.getDbPort();
    adapterConfig.database = config.getDbName();
    adapterConfig.user = config.getDbUser();
    adapterConfig.password = config.getDbPassword();
    
    return create(adapterConfig);
}

std::unique_ptr<IDatabaseAdapter> AdapterFactory::createDefault() {
    AdapterConfig config;
    config.dbType = "sqlite";
    config.dbPath = "stock_db.db";
    return create(config);
}

} // namespace data