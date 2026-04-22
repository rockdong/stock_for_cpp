#include "MySQLConnection.h"
#include "Logger.h"
#include "Config.h"
#include <iostream>

namespace data {

MySQLConnection& MySQLConnection::getInstance() {
    static MySQLConnection instance;
    return instance;
}

bool MySQLConnection::initialize(const std::string& host, int port, 
                                  const std::string& database,
                                  const std::string& user, 
                                  const std::string& password,
                                  const std::string& charset) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (connected_) {
        LOG_WARN("MySQL 已经连接，先断开再重新连接");
        disconnect();
    }
    
    config_ = std::make_unique<sqlpp::mysql::connection_config>();
    config_->host = host;
    config_->port = port;
    config_->database = database;
    config_->user = user;
    config_->password = password;
    config_->charset = charset;
    config_->debug = false;
    config_->connect_timeout_seconds = 30;
    
    LOG_INFO("MySQL 配置: host=" + host + ", port=" + std::to_string(port) + 
             ", database=" + database + ", user=" + user);
    
    return true;
}

bool MySQLConnection::connect() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (connected_) {
        LOG_WARN("MySQL 已经连接");
        return true;
    }
    
    if (!config_) {
        LOG_ERROR("MySQL 配置未设置");
        return false;
    }
    
    try {
        db_ = std::make_unique<sqlpp::mysql::connection>(*config_);
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
        db_.reset();
        return false;
    }
}

void MySQLConnection::disconnect() {
    if (db_) {
        db_.reset();
        connected_ = false;
        LOG_INFO("MySQL 连接已断开");
    }
}

bool MySQLConnection::isConnected() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return connected_;
}

sqlpp::mysql::connection* MySQLConnection::getDb() {
    std::lock_guard<std::mutex> lock(mutex_);
    return db_.get();
}

bool MySQLConnection::execute(const std::string& sql) {
    std::lock_guard<std::mutex> lock(mutex_);
    return executeInternal(sql);
}

bool MySQLConnection::executeInternal(const std::string& sql) {
    LOG_INFO("执行 MySQL SQL: " + sql);
    
    if (!connected_ || !db_) {
        LOG_ERROR("MySQL 未连接");
        return false;
    }
    
    try {
        db_->execute(sql);
        LOG_INFO("MySQL SQL 执行成功");
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("MySQL SQL 执行失败: " + std::string(e.what()));
        return false;
    }
}

bool MySQLConnection::beginTransaction() {
    return execute("START TRANSACTION");
}

bool MySQLConnection::commit() {
    return execute("COMMIT");
}

bool MySQLConnection::rollback() {
    return execute("ROLLBACK");
}

bool MySQLConnection::createTables() {
    LOG_INFO("创建 MySQL 数据库表");
    
    std::string createStocksTable = R"(
        CREATE TABLE IF NOT EXISTS stocks (
            id INT AUTO_INCREMENT PRIMARY KEY,
            ts_code VARCHAR(20) NOT NULL UNIQUE,
            symbol VARCHAR(10),
            name VARCHAR(100),
            area VARCHAR(20),
            industry VARCHAR(50),
            fullname VARCHAR(200),
            enname VARCHAR(200),
            cnspell VARCHAR(20),
            market VARCHAR(20),
            exchange VARCHAR(20),
            curr_type VARCHAR(10),
            list_status VARCHAR(10),
            list_date DATE,
            delist_date DATE,
            is_hs VARCHAR(10),
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
            updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4
    )";
    
    if (!executeInternal(createStocksTable)) {
        LOG_ERROR("创建 stocks 表失败");
        return false;
    }
    
    std::string createPricesTable = R"(
        CREATE TABLE IF NOT EXISTS prices (
            id INT AUTO_INCREMENT PRIMARY KEY,
            stock_id INT NOT NULL,
            trade_date DATE NOT NULL,
            open DECIMAL(18,4),
            high DECIMAL(18,4),
            low DECIMAL(18,4),
            close DECIMAL(18,4),
            pre_close DECIMAL(18,4),
            change_pct DECIMAL(10,4),
            volume BIGINT,
            amount DECIMAL(20,4),
            FOREIGN KEY (stock_id) REFERENCES stocks(id),
            UNIQUE KEY uk_stock_date (stock_id, trade_date)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4
    )";
    
    if (!executeInternal(createPricesTable)) {
        LOG_ERROR("创建 prices 表失败");
        return false;
    }
    
    std::string createTradesTable = R"(
        CREATE TABLE IF NOT EXISTS trades (
            id INT AUTO_INCREMENT PRIMARY KEY,
            stock_id INT NOT NULL,
            trade_type VARCHAR(20) NOT NULL,
            price DECIMAL(18,4) NOT NULL,
            quantity INT NOT NULL,
            amount DECIMAL(20,4) NOT NULL,
            trade_time DATETIME NOT NULL,
            FOREIGN KEY (stock_id) REFERENCES stocks(id)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4
    )";
    
    if (!executeInternal(createTradesTable)) {
        LOG_ERROR("创建 trades 表失败");
        return false;
    }
    
    std::string createAnalysisResultsTable = R"(
        CREATE TABLE IF NOT EXISTS analysis_results (
            id INT AUTO_INCREMENT PRIMARY KEY,
            ts_code VARCHAR(20) NOT NULL,
            strategy_name VARCHAR(50) NOT NULL,
            trade_date DATE NOT NULL,
            label VARCHAR(20) NOT NULL,
            opt VARCHAR(100),
            freq VARCHAR(5),
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
            updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
            UNIQUE KEY uk_analysis (ts_code, strategy_name, trade_date, freq)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4
    )";
    
    if (!executeInternal(createAnalysisResultsTable)) {
        LOG_ERROR("创建 analysis_results 表失败");
        return false;
    }
    
    std::string createAnalysisProgressTable = R"(
        CREATE TABLE IF NOT EXISTS analysis_progress (
            id INT PRIMARY KEY,
            phase1_status VARCHAR(20) DEFAULT 'idle',
            phase1_total INT DEFAULT 0,
            phase1_completed INT DEFAULT 0,
            phase1_qualified INT DEFAULT 0,
            phase2_status VARCHAR(20) DEFAULT 'idle',
            phase2_total INT DEFAULT 0,
            phase2_completed INT DEFAULT 0,
            phase2_failed INT DEFAULT 0,
            started_at TIMESTAMP NULL,
            phase1_completed_at TIMESTAMP NULL,
            updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4
    )";
    
    if (!executeInternal(createAnalysisProgressTable)) {
        LOG_ERROR("创建 analysis_progress 表失败");
        return false;
    }
    
    executeInternal("INSERT IGNORE INTO analysis_progress (id, phase1_status, phase2_status) VALUES (1, 'idle', 'idle')");
    
    std::string createChartDataTable = R"(
        CREATE TABLE IF NOT EXISTS chart_data (
            id INT AUTO_INCREMENT PRIMARY KEY,
            ts_code VARCHAR(20) NOT NULL,
            freq VARCHAR(5) NOT NULL,
            analysis_date DATE NOT NULL,
            data JSON NOT NULL,
            updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
            UNIQUE KEY uk_chart (ts_code, freq, analysis_date)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4
    )";
    
    if (!executeInternal(createChartDataTable)) {
        LOG_ERROR("创建 chart_data 表失败");
        return false;
    }
    
    std::string createProcessRecordsTable = R"(
        CREATE TABLE IF NOT EXISTS analysis_process_records (
            id INT AUTO_INCREMENT PRIMARY KEY,
            ts_code VARCHAR(20) NOT NULL,
            stock_name VARCHAR(100),
            trade_date DATE NOT NULL,
            data JSON NOT NULL DEFAULT '{"strategies":[]}',
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
            expires_at TIMESTAMP NULL,
            UNIQUE KEY uk_process (ts_code, trade_date)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4
    )";
    
    if (!executeInternal(createProcessRecordsTable)) {
        LOG_ERROR("创建 analysis_process_records 表失败");
        return false;
    }
    
    executeInternal("CREATE INDEX IF NOT EXISTS idx_stocks_ts_code ON stocks(ts_code)");
    executeInternal("CREATE INDEX IF NOT EXISTS idx_prices_stock_date ON prices(stock_id, trade_date)");
    executeInternal("CREATE INDEX IF NOT EXISTS idx_analysis_ts_code ON analysis_results(ts_code)");
    executeInternal("CREATE INDEX IF NOT EXISTS idx_analysis_strategy ON analysis_results(strategy_name)");
    executeInternal("CREATE INDEX IF NOT EXISTS idx_analysis_date ON analysis_results(trade_date)");
    executeInternal("CREATE INDEX IF NOT EXISTS idx_chart_lookup ON chart_data(ts_code, freq, analysis_date)");
    executeInternal("CREATE INDEX IF NOT EXISTS idx_process_ts_code ON analysis_process_records(ts_code)");
    executeInternal("CREATE INDEX IF NOT EXISTS idx_process_date ON analysis_process_records(trade_date)");
    
    LOG_INFO("MySQL 数据库表创建成功");
    return true;
}

MySQLConnection::~MySQLConnection() {
    disconnect();
}

} // namespace data