#include "Connection.h"
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
    // 此方法假设调用者已经持有 mutex_ 锁（从 connect() 调用）
    LOG_INFO("创建数据库表");
    
    // 创建股票表
    std::string createStocksTable = R"(
        CREATE TABLE IF NOT EXISTS stocks (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            ts_code TEXT NOT NULL UNIQUE,
            symbol TEXT,
            name TEXT,
            area TEXT,
            industry TEXT,
            fullname TEXT,
            enname TEXT,
            cnspell TEXT,
            market TEXT,
            exchange TEXT,
            curr_type TEXT,
            list_status TEXT,
            list_date TEXT,
            delist_date TEXT,
            is_hs TEXT,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            updated_at DATETIME DEFAULT CURRENT_TIMESTAMP
        )
    )";
    
    LOG_INFO("创建 stocks 表");
    if (!executeInternal(createStocksTable)) {
        LOG_ERROR("创建 stocks 表失败");
        return false;
    }
    
    // 创建价格表
    std::string createPricesTable = R"(
        CREATE TABLE IF NOT EXISTS prices (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            stock_id INTEGER NOT NULL,
            trade_date TEXT NOT NULL,
            open REAL,
            high REAL,
            low REAL,
            close REAL,
            pre_close REAL,
            change_pct REAL,
            volume REAL,
            amount REAL,
            FOREIGN KEY (stock_id) REFERENCES stocks(id),
            UNIQUE(stock_id, trade_date)
        )
    )";
    
    LOG_INFO("创建 prices 表");
    if (!executeInternal(createPricesTable)) {
        LOG_ERROR("创建 prices 表失败");
        return false;
    }
    
    // 创建交易表
    std::string createTradesTable = R"(
        CREATE TABLE IF NOT EXISTS trades (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            stock_id INTEGER NOT NULL,
            trade_type TEXT NOT NULL,
            price REAL NOT NULL,
            quantity INTEGER NOT NULL,
            amount REAL NOT NULL,
            trade_time DATETIME NOT NULL,
            FOREIGN KEY (stock_id) REFERENCES stocks(id)
        )
    )";
    
    LOG_INFO("创建 trades 表");
    if (!executeInternal(createTradesTable)) {
        LOG_ERROR("创建 trades 表失败");
        return false;
    }
    
    // 创建分析结果表
    std::string createAnalysisResultsTable = R"(
        CREATE TABLE IF NOT EXISTS analysis_results (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            ts_code TEXT NOT NULL,
            strategy_name TEXT NOT NULL,
            trade_date TEXT NOT NULL,
            label TEXT NOT NULL,
            opt TEXT,
            freq TEXT,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            updated_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            UNIQUE(ts_code, strategy_name, trade_date, freq)
        )
    )";
    
    LOG_INFO("创建 analysis_results 表");
    if (!executeInternal(createAnalysisResultsTable)) {
        LOG_ERROR("创建 analysis_results 表失败");
        return false;
    }
    
    // 创建索引
    executeInternal("CREATE INDEX IF NOT EXISTS idx_stocks_ts_code ON stocks(ts_code)");
    executeInternal("CREATE INDEX IF NOT EXISTS idx_prices_stock_date ON prices(stock_id, trade_date)");
    executeInternal("CREATE INDEX IF NOT EXISTS idx_trades_stock_time ON trades(stock_id, trade_time)");
    executeInternal("CREATE INDEX IF NOT EXISTS idx_analysis_results_ts_code ON analysis_results(ts_code)");
    executeInternal("CREATE INDEX IF NOT EXISTS idx_analysis_results_strategy ON analysis_results(strategy_name)");
    executeInternal("CREATE INDEX IF NOT EXISTS idx_analysis_results_date ON analysis_results(trade_date)");
    executeInternal("CREATE INDEX IF NOT EXISTS idx_analysis_results_label ON analysis_results(label)");
    
    LOG_INFO("数据库表创建成功");
    return true;
}

Connection::~Connection() {
    disconnect();
}

} // namespace data

