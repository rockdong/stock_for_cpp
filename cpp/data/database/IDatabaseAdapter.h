#ifndef DATA_DATABASE_IDATABASE_ADAPTER_H
#define DATA_DATABASE_IDATABASE_ADAPTER_H

#include "DatabaseTypes.h"
#include <memory>
#include <functional>

namespace data {

class IDatabaseAdapter {
public:
    virtual ~IDatabaseAdapter() = default;
    
    virtual bool connect() = 0;
    virtual void disconnect() = 0;
    virtual bool isConnected() const = 0;
    virtual std::string getDbType() const = 0;
    
    virtual bool beginTransaction() = 0;
    virtual bool commit() = 0;
    virtual bool rollback() = 0;
    
    virtual bool execute(const std::string& sql) = 0;
    
    virtual bool createTable(const TableDefinition& table) = 0;
    virtual bool tableExists(const std::string& tableName) = 0;
    virtual bool dropTable(const std::string& tableName) = 0;
    
    virtual bool columnExists(const std::string& tableName, 
                              const std::string& columnName) = 0;
    virtual bool addColumn(const std::string& tableName,
                           const ColumnDefinition& column,
                           bool ifNotExists = true) = 0;
    
    virtual bool createIndex(const IndexDefinition& index,
                             bool ifNotExists = true) = 0;
    virtual bool indexExists(const std::string& indexName) = 0;
    virtual bool dropIndex(const std::string& indexName) = 0;
    
    virtual bool insertOrIgnore(const std::string& tableName,
                                const std::map<std::string, std::string>& values) = 0;
    
    virtual bool createTables() = 0;
    virtual bool configurePerformance() = 0;
    
    virtual int getSchemaVersion() = 0;
    virtual bool setSchemaVersion(int version) = 0;
    virtual bool runMigrations() = 0;
    
    // 确保 analysis_progress 表有初始化记录（id=1）
    virtual bool ensureAnalysisProgressRecord() = 0;
};

} // namespace data

#endif // DATA_DATABASE_IDATABASE_ADAPTER_H