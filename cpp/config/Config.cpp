#include "Config.h"
#include <laserpants/dotenv/dotenv.h>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <fstream>

namespace config {

Config& Config::getInstance() {
    static Config instance;
    return instance;
}

bool Config::initialize(const char* env_file) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (initialized_) {
        std::cerr << "Config: 配置已经初始化，使用 reload() 重新加载" << std::endl;
        return true;
    }

    try {
        dotenv::init(env_file);
        loadConfig();
        initialized_ = true;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Config: 初始化失败: " << e.what() << std::endl;
        return false;
    }
}

bool Config::reload(const char* env_file) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    try {
        dotenv::init(env_file);
        loadConfig();
        initialized_ = true;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Config: 重新加载失败: " << e.what() << std::endl;
        return false;
    }
}

void Config::loadConfig() {
    // ========== 日志配置 ==========
    log_level_ = getEnvString("LOG_LEVEL", "INFO");
    log_pattern_ = getEnvString("LOG_PATTERN", "[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%t] %v");
    log_file_path_ = getEnvString("LOG_FILE_PATH", "logs/app.log");
    log_console_enabled_ = getEnvBool("LOG_CONSOLE_ENABLED", true);
    log_file_enabled_ = getEnvBool("LOG_FILE_ENABLED", true);
    log_max_file_size_ = getEnvSize("LOG_MAX_FILE_SIZE", 10485760); // 10MB
    log_max_files_ = getEnvSize("LOG_MAX_FILES", 3);
    log_async_enabled_ = getEnvBool("LOG_ASYNC_ENABLED", false);
    log_async_queue_size_ = getEnvSize("LOG_ASYNC_QUEUE_SIZE", 8192);

    // ========== 数据库配置 ==========
    db_host_ = getEnvString("DB_HOST", "localhost");
    db_port_ = getEnvInt("DB_PORT", 3306);
    db_name_ = getEnvString("DB_NAME", "stock_db");
    db_user_ = getEnvString("DB_USER", "root");
    db_password_ = getEnvString("DB_PASSWORD", "");
    db_pool_size_ = getEnvInt("DB_POOL_SIZE", 10);
    db_timeout_ = getEnvInt("DB_TIMEOUT", 30);
    db_charset_ = getEnvString("DB_CHARSET", "utf8mb4");

    // ========== 数据源配置 ==========
    data_source_url_ = getEnvString("DATA_SOURCE_URL", "https://api.example.com");
    data_source_api_key_ = getEnvString("DATA_SOURCE_API_KEY", "");
    data_source_timeout_ = getEnvInt("DATA_SOURCE_TIMEOUT", 10);
    data_source_retry_times_ = getEnvInt("DATA_SOURCE_RETRY_TIMES", 3);
    data_source_retry_delay_ = getEnvInt("DATA_SOURCE_RETRY_DELAY", 1000); // 毫秒

    // ========== Tushare 限流配置 ==========
    tushare_rate_limit_ = getEnvInt("TUSHARE_RATE_LIMIT", 8);
    tushare_burst_size_ = getEnvInt("TUSHARE_BURST_SIZE", 10);

    // ========== 缓存配置 ==========
    cache_enabled_ = getEnvBool("CACHE_ENABLED", true);
    cache_size_ = getEnvSize("CACHE_SIZE", 1000);
    cache_ttl_ = getEnvInt("CACHE_TTL", 300); // 秒
    cache_policy_ = getEnvString("CACHE_POLICY", "LRU");

    // ========== 输出配置 ==========
    output_dir_ = getEnvString("OUTPUT_DIR", "output");
    export_format_ = getEnvString("EXPORT_FORMAT", "csv");
    chart_enabled_ = getEnvBool("CHART_ENABLED", false);
    console_width_ = getEnvInt("CONSOLE_WIDTH", 120);

    // ========== 分析配置 ==========
    default_ma_period_ = getEnvInt("DEFAULT_MA_PERIOD", 20);
    default_macd_fast_ = getEnvInt("DEFAULT_MACD_FAST", 12);
    default_macd_slow_ = getEnvInt("DEFAULT_MACD_SLOW", 26);
    default_macd_signal_ = getEnvInt("DEFAULT_MACD_SIGNAL", 9);
    default_rsi_period_ = getEnvInt("DEFAULT_RSI_PERIOD", 14);
    strategies_ = getEnvString("STRATEGIES", "EMA17TO25;MACD;RSI");
    analysis_date_switch_time_ = getEnvString("ANALYSIS_DATE_SWITCH_TIME", "02:00");

    // ========== 应用配置 ==========
    app_name_ = getEnvString("APP_NAME", "Stock Analysis System");
    app_version_ = getEnvString("APP_VERSION", "1.0.0");
    app_env_ = getEnvString("APP_ENV", "development");
    debug_mode_ = getEnvBool("DEBUG_MODE", false);

    // ========== 基本面筛选配置 ==========
    fundamental_pe_max_ = getEnvDouble("FUNDAMENTAL_PE_MAX", 30.0);
    fundamental_pb_max_ = getEnvDouble("FUNDAMENTAL_PB_MAX", 5.0);
    fundamental_peg_max_ = getEnvDouble("FUNDAMENTAL_PEG_MAX", 1.5);
    fundamental_roe_min_ = getEnvDouble("FUNDAMENTAL_ROE_MIN", 10.0);
    fundamental_gross_margin_min_ = getEnvDouble("FUNDAMENTAL_GROSS_MARGIN_MIN", 20.0);
    fundamental_net_margin_min_ = getEnvDouble("FUNDAMENTAL_NET_MARGIN_MIN", 10.0);
    fundamental_revenue_growth_min_ = getEnvDouble("FUNDAMENTAL_REVENUE_GROWTH_MIN", 5.0);
    fundamental_profit_growth_min_ = getEnvDouble("FUNDAMENTAL_PROFIT_GROWTH_MIN", 5.0);
    fundamental_debt_ratio_max_ = getEnvDouble("FUNDAMENTAL_DEBT_RATIO_MAX", 60.0);
    fundamental_current_ratio_min_ = getEnvDouble("FUNDAMENTAL_CURRENT_RATIO_MIN", 1.0);
    fundamental_total_score_min_ = getEnvDouble("FUNDAMENTAL_TOTAL_SCORE_MIN", 60.0);

    // ========== Web API 配置 ==========
    web_api_timeout_ = getEnvInt("WEB_API_TIMEOUT", 30000);
    fundamental_filter_timeout_ = getEnvInt("FUNDAMENTAL_FILTER_TIMEOUT", 60000);
}

std::string Config::getDbConnectionString() const {
    std::ostringstream oss;
    oss << "host=" << db_host_
        << ";port=" << db_port_
        << ";database=" << db_name_
        << ";user=" << db_user_
        << ";password=" << db_password_
        << ";charset=" << db_charset_;
    return oss.str();
}

std::string Config::getEnvString(const char* key, const std::string& default_value) const {
    const char* value = std::getenv(key);
    return value ? std::string(value) : default_value;
}

int Config::getEnvInt(const char* key, int default_value) const {
    const char* value = std::getenv(key);
    if (!value) return default_value;
    
    try {
        return std::stoi(value);
    } catch (...) {
        std::cerr << "Config: 无法解析整数值 " << key << "=" << value 
                  << ", 使用默认值 " << default_value << std::endl;
        return default_value;
    }
}

bool Config::getEnvBool(const char* key, bool default_value) const {
    const char* value = std::getenv(key);
    if (!value) return default_value;
    
    std::string str_value(value);
    // 转换为小写
    std::transform(str_value.begin(), str_value.end(), str_value.begin(), ::tolower);
    
    return str_value == "true" || str_value == "1" || str_value == "yes" || str_value == "on";
}

size_t Config::getEnvSize(const char* key, size_t default_value) const {
    const char* value = std::getenv(key);
    if (!value) return default_value;
    
    try {
        return static_cast<size_t>(std::stoull(value));
    } catch (...) {
        std::cerr << "Config: 无法解析 size_t 值 " << key << "=" << value 
                  << ", 使用默认值 " << default_value << std::endl;
        return default_value;
    }
}

double Config::getEnvDouble(const char* key, double default_value) const {
    const char* value = std::getenv(key);
    if (!value) return default_value;
    
    try {
        return std::stod(value);
    } catch (...) {
        std::cerr << "Config: 无法解析 double 值 " << key << "=" << value 
                  << ", 使用默认值 " << default_value << std::endl;
        return default_value;
    }
}

} // namespace config

