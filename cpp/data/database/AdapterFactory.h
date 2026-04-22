#ifndef DATA_DATABASE_ADAPTER_FACTORY_H
#define DATA_DATABASE_ADAPTER_FACTORY_H

#include "IDatabaseAdapter.h"
#include "SQLiteAdapter.h"
#include "Config.h"
#include <memory>

#ifdef HAS_MYSQL
#include "MySQLAdapter.h"
#endif

namespace data {

struct AdapterConfig {
    std::string dbType;
    std::string dbPath;
    std::string host;
    int port = 3306;
    std::string database;
    std::string user;
    std::string password;
    std::string charset = "utf8mb4";
};

class AdapterFactory {
public:
    static std::unique_ptr<IDatabaseAdapter> create(const AdapterConfig& config);
    static std::unique_ptr<IDatabaseAdapter> createFromConfig();
    static std::unique_ptr<IDatabaseAdapter> createDefault();
};

} // namespace data

#endif // DATA_DATABASE_ADAPTER_FACTORY_H