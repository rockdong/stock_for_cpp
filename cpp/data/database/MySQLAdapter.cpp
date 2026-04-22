#include "MySQLAdapter.h"
#include "SchemaDefinitions.h"
#include "Logger.h"
#include <sstream>
#include <iomanip>

#ifdef HAS_MYSQL

namespace data {

bool MySQLAdapter::initialize(const std::string& host, int port,
                               const std::string& database,
                               const std::string& user,
                               const std::string& password,
                               const std::string& charset) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    config_ = std::make_unique<sqlpp::mysql::connection_config>();
    config_->host = host;
    config_->port = port;
    config_->database = database;
    config_->user = user;
    config_->password = password;
    config_->charset = charset;
    config_->debug = false;
    
    LOG_INFO("MySQL 配置: host=" + host + ", port=" + std::to_string(port) +
             ", database=" + database + ", user=" + user);
    
    return true;
}

bool MySQLAdapter::connect() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (connected_) {
        LOG_WARN("MySQL 已连接");
        return true;
    }
    
    if (!config_) {
        LOG_ERROR("MySQL 配置未设置");
        return false;
    }
    
    try {
        connection_ = std::make_unique<sqlpp::mysql::connection>(*config_);
        connected_ = true;
        
        LOG_INFO("MySQL 连接成功: " + config_->host + ":" + std::to_string(config_->port));
        
        if (!createTables()) {
            LOG_ERROR("创建 MySQL 表失败");
            disconnect();
            return false;
        }
        
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("MySQL 连接失败: " + std::string(e.what()));
        connection_.reset();
        return false;
    }
}

void MySQLAdapter::disconnect() {
    if (connection_) {
        connection_.reset();
        connected_ = false;
        LOG_INFO("MySQL 连接已断开");
    }
}

bool MySQLAdapter::isConnected() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return connected_;
}

std::string MySQLAdapter::getDbType() const {
    return "mysql";
}

bool MySQLAdapter::beginTransaction() {
    return execute("START TRANSACTION");
}

bool MySQLAdapter::commit() {
    return execute("COMMIT");
}

bool MySQLAdapter::rollback() {
    return execute("ROLLBACK");
}

bool MySQLAdapter::execute(const std::string& sql) {
    std::lock_guard<std::mutex> lock(mutex_);
    return executeInternal(sql);
}

bool MySQLAdapter::executeInternal(const std::string& sql) {
    if (!connected_ || !connection_) {
        LOG_ERROR("MySQL 未连接");
        return false;
    }
    
    try {
        connection_->execute(sql);
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("MySQL SQL 执行失败: " + std::string(e.what()) + ", SQL: " + sql);
        return false;
    }
}

std::string MySQLAdapter::generateColumnSQL(const ColumnDefinition& column) {
    std::ostringstream sql;
    sql << column.name << " ";
    
    switch (column.type) {
        case DataType::TEXT:
            if (column.varcharLength > 0) {
                sql << "VARCHAR(" << column.varcharLength << ")";
            } else {
                sql << "VARCHAR(255)";
            }
            break;
        case DataType::INTEGER:
            if (column.primaryKey && column.autoIncrement) {
                sql << "INT AUTO_INCREMENT PRIMARY KEY";
                return sql.str();
            }
            sql << "INT";
            break;
        case DataType::BIGINT:
            sql << "BIGINT";
            break;
        case DataType::DECIMAL:
            if (column.precision > 0) {
                sql << "DECIMAL(" << column.precision << "," << column.scale << ")";
            } else {
                sql << "DECIMAL(18,4)";
            }
            break;
        case DataType::REAL:
            sql << "DECIMAL(18,4)";
            break;
        case DataType::DATETIME:
            sql << "DATETIME";
            break;
        case DataType::TIMESTAMP:
            if (!column.defaultValue.empty() && column.defaultValue == "CURRENT_TIMESTAMP") {
                sql << "TIMESTAMP DEFAULT CURRENT_TIMESTAMP";
                return sql.str();
            }
            sql << "TIMESTAMP";
            break;
        case DataType::DATE:
            sql << "DATE";
            break;
        case DataType::JSON:
            sql << "JSON";
            break;
        case DataType::BOOLEAN:
            sql << "BOOLEAN";
            break;
        default:
            sql << "VARCHAR(255)";
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
    
    if (!column.defaultValue.empty() && 
        column.type != DataType::TIMESTAMP && 
        !(column.primaryKey && column.autoIncrement)) {
        sql << " DEFAULT " << column.defaultValue;
    }
    
    return sql.str();
}

std::string MySQLAdapter::generateCreateTableSQL(const TableDefinition& table) {
    std::ostringstream sql;
    sql << "CREATE TABLE IF NOT EXISTS " << table.name << " (\n";
    
    bool first = true;
    for (const auto& column : table.columns) {
        if (!first) sql << ",\n";
        sql << "    " << generateColumnSQL(column);
        first = false;
    }
    
    for (const auto& column : table.columns) {
        if (!column.foreignKeyTable.empty()) {
            if (!first) sql << ",\n";
            sql << "    FOREIGN KEY (" << column.name << ") REFERENCES " 
                << column.foreignKeyTable << "(" << column.foreignKeyColumn << ")";
            first = false;
        }
    }
    
    std::vector<std::string> uniqueCols;
    if (table.name == "prices") uniqueCols = {"ts_code", "trade_date"};
    else if (table.name == "analysis_results") uniqueCols = {"ts_code", "strategy_name", "trade_date", "freq"};
    else if (table.name == "chart_data") uniqueCols = {"ts_code", "freq", "analysis_date"};
    else if (table.name == "analysis_process_records") uniqueCols = {"ts_code", "trade_date"};
    
    if (!uniqueCols.empty()) {
        if (!first) sql << ",\n";
        sql << "    UNIQUE KEY uk_" << table.name << " (";
        for (size_t i = 0; i < uniqueCols.size(); i++) {
            if (i > 0) sql << ", ";
            sql << uniqueCols[i];
        }
        sql << ")";
    }
    
    sql << "\n) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4";
    return sql.str();
}

std::string MySQLAdapter::generateCreateIndexSQL(const IndexDefinition& index) {
    std::ostringstream sql;
    sql << "CREATE ";
    if (index.unique) sql << "UNIQUE ";
    sql << "INDEX " << index.name << " ON " << index.tableName << "(";
    for (size_t i = 0; i < index.columns.size(); i++) {
        if (i > 0) sql << ", ";
        sql << index.columns[i];
    }
    sql << ")";
    return sql.str();
}

bool MySQLAdapter::createTable(const TableDefinition& table) {
    std::string sql = generateCreateTableSQL(table);
    LOG_INFO("创建 MySQL 表: " + table.name);
    return executeInternal(sql);
}

bool MySQLAdapter::tableExists(const std::string& tableName) {
    std::ostringstream sql;
    sql << "SELECT TABLE_NAME FROM INFORMATION_SCHEMA.TABLES "
        << "WHERE TABLE_SCHEMA = DATABASE() AND TABLE_NAME = '" << tableName << "'";
    
    try {
        connection_->execute(sql.str());
        return true;
    } catch (...) {
        return false;
    }
}

bool MySQLAdapter::dropTable(const std::string& tableName) {
    return executeInternal("DROP TABLE IF EXISTS " + tableName);
}

bool MySQLAdapter::columnExists(const std::string& tableName, const std::string& columnName) {
    std::ostringstream sql;
    sql << "SELECT COLUMN_NAME FROM INFORMATION_SCHEMA.COLUMNS "
        << "WHERE TABLE_SCHEMA = DATABASE() AND TABLE_NAME = '" << tableName 
        << "' AND COLUMN_NAME = '" << columnName << "'";
    
    try {
        connection_->execute(sql.str());
        return true;
    } catch (...) {
        return false;
    }
}

bool MySQLAdapter::addColumn(const std::string& tableName,
                              const ColumnDefinition& column,
                              bool ifNotExists) {
    std::ostringstream sql;
    sql << "ALTER TABLE " << tableName << " ADD COLUMN ";
    if (ifNotExists) sql << "IF NOT EXISTS ";
    sql << generateColumnSQL(column);
    return executeInternal(sql.str());
}

bool MySQLAdapter::createIndex(const IndexDefinition& index, bool ifNotExists) {
    if (ifNotExists && indexExists(index.name)) {
        LOG_INFO("索引已存在: " + index.name);
        return true;
    }
    
    std::string sql = generateCreateIndexSQL(index);
    LOG_INFO("创建 MySQL 索引: " + index.name);
    return executeInternal(sql);
}

bool MySQLAdapter::indexExists(const std::string& indexName) {
    std::ostringstream sql;
    sql << "SELECT INDEX_NAME FROM INFORMATION_SCHEMA.STATISTICS "
        << "WHERE TABLE_SCHEMA = DATABASE() AND INDEX_NAME = '" << indexName << "'";
    
    try {
        connection_->execute(sql.str());
        return true;
    } catch (...) {
        return false;
    }
}

bool MySQLAdapter::dropIndex(const std::string& indexName) {
    return executeInternal("DROP INDEX " + indexName + " ON analysis_results");
}

bool MySQLAdapter::insertOrIgnore(const std::string& tableName,
                                   const std::map<std::string, std::string>& values) {
    std::ostringstream sql;
    sql << "INSERT IGNORE INTO " << tableName << " (";
    
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

bool MySQLAdapter::createTables() {
    LOG_INFO("创建所有 MySQL 表");
    
    for (const auto& table : schema::ALL_TABLES) {
        if (!createTable(table)) {
            LOG_ERROR("创建 MySQL 表失败: " + table.name);
            return false;
        }
        
        for (const auto& index : table.indexes) {
            if (!createIndex(index, true)) {
                LOG_WARN("创建 MySQL 索引失败: " + index.name);
            }
        }
    }
    
    insertOrIgnore("analysis_progress", {
        {"id", "1"},
        {"phase1_status", "'idle'"},
        {"phase2_status", "'idle'"}
    });
    
    LOG_INFO("MySQL 表创建成功");
    return true;
}

bool MySQLAdapter::configurePerformance() {
    return true;
}

int MySQLAdapter::getSchemaVersion() {
    executeInternal("CREATE TABLE IF NOT EXISTS schema_version (version INT PRIMARY KEY)");
    return 0;
}

bool MySQLAdapter::setSchemaVersion(int version) {
    std::ostringstream sql;
    sql << "INSERT INTO schema_version (version) VALUES (" << version 
        << ") ON DUPLICATE KEY UPDATE version = " << version;
    return executeInternal(sql.str());
}

bool MySQLAdapter::runMigrations() {
    return true;
}

sqlpp::mysql::connection* MySQLAdapter::getConnection() {
    std::lock_guard<std::mutex> lock(mutex_);
    return connection_.get();
}

MySQLAdapter::~MySQLAdapter() {
    disconnect();
}

} // namespace data

#endif // HAS_MYSQL