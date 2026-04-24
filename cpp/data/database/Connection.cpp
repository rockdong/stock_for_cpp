#include "Connection.h"
#include "SQLiteAdapter.h"
#include "Logger.h"
#include <iostream>

namespace data {

Connection& Connection::getInstance() {
    static Connection instance;
    return instance;
}

bool Connection::initialize(const std::string& dbPath) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (connected_) {
        LOG_WARN("数据库已经连接，先断开再重新连接");
        disconnect();
    }
    
    dbPath_ = dbPath;
    LOG_INFO("数据库路径设置为: " + dbPath_);
    
    return true;
}

bool Connection::connect() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (connected_) {
        LOG_WARN("数据库已经连接");
        return true;
    }
    
    if (dbPath_.empty()) {
        LOG_ERROR("数据库路径未设置");
        return false;
    }
    
    try {
        // 创建 sqlpp11 SQLite 连接配置
        sqlpp::sqlite3::connection_config config;
        config.path_to_database = dbPath_;
        config.flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
        config.debug = false;
        
        // 创建连接
        db_ = std::make_unique<sqlpp::sqlite3::connection>(config);
        
        // 设置 busy_timeout (5秒等待锁释放)
        db_->execute("PRAGMA busy_timeout = 5000");
        
        // 启用 WAL 模式 (提高并发性能)
        db_->execute("PRAGMA journal_mode = WAL");
        
        // 设置同步模式为 NORMAL (性能优化)
        db_->execute("PRAGMA synchronous = NORMAL");
        
        connected_ = true;
        LOG_INFO("数据库连接成功: " + dbPath_);
        
        // 创建表
        if (!createTables()) {
            LOG_ERROR("创建数据库表失败");
            disconnect();
            return false;
        }
        
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("无法打开数据库: " + std::string(e.what()));
        db_.reset();
        return false;
    }
}

void Connection::disconnect() {
    if (db_) {
        db_.reset();
        connected_ = false;
        LOG_INFO("数据库连接已断开");
    }
}

bool Connection::isConnected() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return connected_;
}

sqlpp::sqlite3::connection* Connection::getDb() {
    std::lock_guard<std::mutex> lock(mutex_);
    return db_.get();
}

bool Connection::execute(const std::string& sql) {
    std::lock_guard<std::mutex> lock(mutex_);
    return executeInternal(sql);
}

bool Connection::executeInternal(const std::string& sql) {
    // 此方法假设调用者已经持有 mutex_ 锁
    LOG_INFO("执行 SQL: " + sql);
    
    if (!connected_ || !db_) {
        LOG_ERROR("数据库未连接");
        return false;
    }
    
    try {
        db_->execute(sql);
        LOG_INFO("SQL 执行成功");
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("SQL 执行失败: " + std::string(e.what()));
        return false;
    }
}

bool Connection::beginTransaction() {
    return execute("BEGIN TRANSACTION");
}

bool Connection::commit() {
    return execute("COMMIT");
}

bool Connection::rollback() {
    return execute("ROLLBACK");
}

bool Connection::createTables() {
    LOG_INFO("创建数据库表");
    
    SQLiteAdapter adapter;
    adapter.initialize(dbPath_);
    if (!adapter.connect()) {
        LOG_ERROR("SQLiteAdapter 连接失败");
        return false;
    }
    
    if (!adapter.createTables()) {
        LOG_ERROR("创建数据库表失败");
        return false;
    }
    
    if (!adapter.ensureAnalysisProgressRecord()) {
        LOG_ERROR("初始化 analysis_progress 记录失败");
        return false;
    }
    
    LOG_INFO("数据库表创建成功");
    return true;
}

bool Connection::ensureAnalysisProgressRecord() {
    return executeInternal(
        "INSERT OR REPLACE INTO analysis_progress (id, phase1_status, phase2_status) "
        "VALUES (1, 'idle', 'idle')"
    );
}

Connection::~Connection() {
    disconnect();
}

} // namespace data

