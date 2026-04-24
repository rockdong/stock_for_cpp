#include "SQLiteAdapter.h"
#include "SchemaDefinitions.h"
#include "Logger.h"
#include <sstream>
#include <iomanip>

namespace data {

bool SQLiteAdapter::initialize(const std::string& dbPath) {
    std::lock_guard<std::mutex> lock(mutex_);
    dbPath_ = dbPath;
    LOG_INFO("SQLite 初始化: " + dbPath_);
    return true;
}

bool SQLiteAdapter::connect() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (connected_) {
        LOG_WARN("SQLite 已连接");
        return true;
    }
    
    if (dbPath_.empty()) {
        LOG_ERROR("数据库路径未设置");
        return false;
    }
    
    try {
        sqlpp::sqlite3::connection_config config;
        config.path_to_database = dbPath_;
        config.flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
        config.debug = false;
        
        connection_ = std::make_unique<sqlpp::sqlite3::connection>(config);
        connected_ = true;
        
        LOG_INFO("SQLite 连接成功: " + dbPath_);
        
        configurePerformance();
        
        if (!createTables()) {
            LOG_ERROR("创建 SQLite 表失败");
            disconnect();
            return false;
        }
        
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("SQLite 连接失败: " + std::string(e.what()));
        connection_.reset();
        return false;
    }
}

void SQLiteAdapter::disconnect() {
    if (connection_) {
        connection_.reset();
        connected_ = false;
        LOG_INFO("SQLite 连接已断开");
    }
}

bool SQLiteAdapter::isConnected() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return connected_;
}

std::string SQLiteAdapter::getDbType() const {
    return "sqlite";
}

bool SQLiteAdapter::beginTransaction() {
    return execute("BEGIN TRANSACTION");
}

bool SQLiteAdapter::commit() {
    return execute("COMMIT");
}

bool SQLiteAdapter::rollback() {
    return execute("ROLLBACK");
}

bool SQLiteAdapter::execute(const std::string& sql) {
    std::lock_guard<std::mutex> lock(mutex_);
    return executeInternal(sql);
}

bool SQLiteAdapter::executeInternal(const std::string& sql) {
    if (!connected_ || !connection_) {
        LOG_ERROR("SQLite 未连接");
        return false;
    }
    
    try {
        connection_->execute(sql);
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("SQLite SQL 执行失败: " + std::string(e.what()) + ", SQL: " + sql);
        return false;
    }
}

std::string SQLiteAdapter::generateColumnSQL(const ColumnDefinition& column) {
    std::ostringstream sql;
    sql << column.name << " ";
    
    switch (column.type) {
        case DataType::TEXT:
            sql << "TEXT";
            break;
        case DataType::INTEGER:
            if (column.primaryKey && column.autoIncrement) {
                sql << "INTEGER PRIMARY KEY AUTOINCREMENT";
                return sql.str();
            }
            sql << "INTEGER";
            break;
        case DataType::DECIMAL:
        case DataType::REAL:
            sql << "REAL";
            break;
        case DataType::BIGINT:
            sql << "INTEGER";
            break;
        case DataType::DATETIME:
        case DataType::TIMESTAMP:
            sql << "DATETIME";
            break;
        case DataType::DATE:
            sql << "TEXT";
            break;
        case DataType::JSON:
            sql << "TEXT";
            break;
        case DataType::BOOLEAN:
            sql << "INTEGER";
            break;
        default:
            sql << "TEXT";
    }
    
    if (column.primaryKey) {
        sql << " PRIMARY KEY";
    }
    
    if (!column.nullable && !column.primaryKey) {
        sql << " NOT NULL";
    }
    
    if (column.unique && !column.primaryKey) {
        sql << " UNIQUE";
    }
    
    if (!column.defaultValue.empty()) {
        sql << " DEFAULT " << column.defaultValue;
    }
    
    if (!column.checkConstraint.empty()) {
        sql << " CHECK (" << column.checkConstraint << ")";
    }
    
    return sql.str();
}

std::string SQLiteAdapter::generateCreateTableSQL(const TableDefinition& table) {
    std::ostringstream sql;
    sql << "CREATE TABLE IF NOT EXISTS " << table.name << " (\n";
    
    bool first = true;
    for (const auto& column : table.columns) {
        if (!first) sql << ",\n";
        sql << "    " << generateColumnSQL(column);
        first = false;
    }
    
    bool hasUniqueConstraint = false;
    for (const auto& column : table.columns) {
        if (column.unique && !column.primaryKey) {
            hasUniqueConstraint = true;
            break;
        }
    }
    
    if (!hasUniqueConstraint) {
        bool hasFk = false;
        for (const auto& column : table.columns) {
            if (!column.foreignKeyTable.empty()) {
                if (!first) sql << ",\n";
                sql << "    FOREIGN KEY (" << column.name << ") REFERENCES " 
                    << column.foreignKeyTable << "(" << column.foreignKeyColumn << ")";
                first = false;
                hasFk = true;
            }
        }
        
        if (table.name == "prices" || table.name == "analysis_results" || 
            table.name == "chart_data" || table.name == "analysis_process_records") {
            std::vector<std::string> uniqueCols;
            if (table.name == "prices") uniqueCols = {"ts_code", "trade_date"};
            else if (table.name == "analysis_results") uniqueCols = {"ts_code", "strategy_name", "trade_date", "freq"};
            else if (table.name == "chart_data") uniqueCols = {"ts_code", "freq", "analysis_date"};
            else if (table.name == "analysis_process_records") uniqueCols = {"ts_code", "trade_date"};
            
            if (!uniqueCols.empty()) {
                if (!first) sql << ",\n";
                sql << "    UNIQUE(";
                for (size_t i = 0; i < uniqueCols.size(); i++) {
                    if (i > 0) sql << ", ";
                    sql << uniqueCols[i];
                }
                sql << ")";
            }
        }
    }
    
    sql << "\n)";
    return sql.str();
}

std::string SQLiteAdapter::generateCreateIndexSQL(const IndexDefinition& index, bool ifNotExists) {
    std::ostringstream sql;
    sql << "CREATE ";
    if (index.unique) sql << "UNIQUE ";
    sql << "INDEX ";
    if (ifNotExists) sql << "IF NOT EXISTS ";
    sql << index.name << " ON " << index.tableName << "(";
    for (size_t i = 0; i < index.columns.size(); i++) {
        if (i > 0) sql << ", ";
        sql << index.columns[i];
    }
    sql << ")";
    return sql.str();
}

bool SQLiteAdapter::createTable(const TableDefinition& table) {
    std::string sql = generateCreateTableSQL(table);
    LOG_INFO("创建表: " + table.name);
    return executeInternal(sql);
}

bool SQLiteAdapter::tableExists(const std::string& tableName) {
    std::ostringstream sql;
    sql << "SELECT name FROM sqlite_master WHERE type='table' AND name='" << tableName << "'";
    
    try {
        auto result = connection_->execute(sql.str());
        return true;
    } catch (...) {
        return false;
    }
}

bool SQLiteAdapter::dropTable(const std::string& tableName) {
    return executeInternal("DROP TABLE IF EXISTS " + tableName);
}

bool SQLiteAdapter::columnExists(const std::string& tableName, const std::string& columnName) {
    std::ostringstream sql;
    sql << "PRAGMA table_info(" << tableName << ")";
    
    try {
        connection_->execute(sql.str());
        return true;
    } catch (...) {
        return false;
    }
}

bool SQLiteAdapter::addColumn(const std::string& tableName, 
                               const ColumnDefinition& column,
                               bool ifNotExists) {
    if (ifNotExists && columnExists(tableName, column.name)) {
        LOG_INFO("字段已存在: " + tableName + "." + column.name);
        return true;
    }
    
    std::ostringstream sql;
    sql << "ALTER TABLE " << tableName << " ADD COLUMN " << generateColumnSQL(column);
    return executeInternal(sql.str());
}

bool SQLiteAdapter::createIndex(const IndexDefinition& index, bool ifNotExists) {
    std::string sql = generateCreateIndexSQL(index, ifNotExists);
    LOG_INFO("创建索引: " + index.name);
    return executeInternal(sql);
}

bool SQLiteAdapter::indexExists(const std::string& indexName) {
    std::ostringstream sql;
    sql << "SELECT name FROM sqlite_master WHERE type='index' AND name='" << indexName << "'";
    
    try {
        connection_->execute(sql.str());
        return true;
    } catch (...) {
        return false;
    }
}

bool SQLiteAdapter::dropIndex(const std::string& indexName) {
    return executeInternal("DROP INDEX IF EXISTS " + indexName);
}

bool SQLiteAdapter::insertOrIgnore(const std::string& tableName,
                                    const std::map<std::string, std::string>& values) {
    std::ostringstream sql;
    sql << "INSERT OR IGNORE INTO " << tableName << " (";
    
    bool first = true;
    for (const auto& [key, val] : values) {
        if (!first) sql << ", ";
        sql << key;
        first = false;
    }
    
    sql << ") VALUES (";
    first = true;
    for (const auto& [key, val] : values) {
        if (!first) sql << ", ";
        sql << val;
        first = false;
    }
    sql << ")";
    
    return executeInternal(sql.str());
}

bool SQLiteAdapter::createTables() {
    LOG_INFO("创建所有 SQLite 表");
    
    for (const auto& table : schema::ALL_TABLES) {
        if (!createTable(table)) {
            LOG_ERROR("创建表失败: " + table.name);
            return false;
        }
        
        for (const auto& index : table.indexes) {
            if (!createIndex(index, true)) {
                LOG_WARN("创建索引失败: " + index.name);
            }
        }
    }
    
    ensureAnalysisProgressRecord();
    
    LOG_INFO("SQLite 表创建成功");
    return true;
}

bool SQLiteAdapter::configurePerformance() {
    LOG_INFO("配置 SQLite 性能参数");
    
    executeInternal("PRAGMA busy_timeout = 5000");
    executeInternal("PRAGMA journal_mode = WAL");
    executeInternal("PRAGMA synchronous = NORMAL");
    
    return true;
}

int SQLiteAdapter::getSchemaVersion() {
    executeInternal("CREATE TABLE IF NOT EXISTS schema_version (version INTEGER PRIMARY KEY)");
    
    try {
        return 0;
    } catch (...) {
        return 0;
    }
}

bool SQLiteAdapter::setSchemaVersion(int version) {
    std::ostringstream sql;
    sql << "INSERT OR REPLACE INTO schema_version (version) VALUES (" << version << ")";
    return executeInternal(sql.str());
}

bool SQLiteAdapter::runMigrations() {
    return true;
}

bool SQLiteAdapter::ensureAnalysisProgressRecord() {
    return executeInternal(
        "INSERT OR REPLACE INTO analysis_progress (id, phase1_status, phase2_status) "
        "VALUES (1, 'idle', 'idle')"
    );
}

sqlpp::sqlite3::connection* SQLiteAdapter::getConnection() {
    std::lock_guard<std::mutex> lock(mutex_);
    return connection_.get();
}

SQLiteAdapter::~SQLiteAdapter() = default;

} // namespace data