#ifndef DATA_DATABASE_SQLITE_ADAPTER_H
#define DATA_DATABASE_SQLITE_ADAPTER_H

#include "IDatabaseAdapter.h"
#include "Logger.h"
#include <sqlpp11/sqlite3/sqlite3.h>
#include <memory>
#include <mutex>

namespace data {

class SQLiteAdapter : public IDatabaseAdapter {
public:
    SQLiteAdapter() = default;
    ~SQLiteAdapter();
    
    bool initialize(const std::string& dbPath);
    
    bool connect() override;
    void disconnect() override;
    bool isConnected() const override;
    std::string getDbType() const override;
    
    bool beginTransaction() override;
    bool commit() override;
    bool rollback() override;
    
    bool execute(const std::string& sql) override;
    
    bool createTable(const TableDefinition& table) override;
    bool tableExists(const std::string& tableName) override;
    bool dropTable(const std::string& tableName) override;
    
    bool columnExists(const std::string& tableName,
                      const std::string& columnName) override;
    bool addColumn(const std::string& tableName,
                   const ColumnDefinition& column,
                   bool ifNotExists = true) override;
    
    bool createIndex(const IndexDefinition& index,
                     bool ifNotExists = true) override;
    bool indexExists(const std::string& indexName) override;
    bool dropIndex(const std::string& indexName) override;
    
    bool insertOrIgnore(const std::string& tableName,
                        const std::map<std::string, std::string>& values) override;
    
    bool createTables() override;
    bool configurePerformance() override;
    
    int getSchemaVersion() override;
    bool setSchemaVersion(int version) override;
    bool runMigrations() override;
    
    sqlpp::sqlite3::connection* getConnection();
    
private:
    
    bool executeInternal(const std::string& sql);
    
    std::string generateCreateTableSQL(const TableDefinition& table);
    std::string generateColumnSQL(const ColumnDefinition& column);
    std::string generateCreateIndexSQL(const IndexDefinition& index, bool ifNotExists);
    
    std::unique_ptr<sqlpp::sqlite3::connection> connection_;
    std::string dbPath_;
    bool connected_ = false;
    mutable std::mutex mutex_;
};

} // namespace data

#endif // DATA_DATABASE_SQLITE_ADAPTER_H