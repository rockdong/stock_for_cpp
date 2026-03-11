#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <memory>
#include <mutex>

namespace config {

/**
 * @brief 全局配置管理类
 * 单例模式：全局唯一的配置管理器
 * 单一职责原则：只负责管理系统配置
 * 从环境变量中读取所有配置信息
 */
class Config {
public:
    /**
     * @brief 获取单例实例
     * @return 配置管理器引用
     */
    static Config& getInstance();

    /**
     * @brief 初始化配置（从 .env 文件加载）
     * @param env_file .env 文件路径
     * @return 是否初始化成功
     */
    bool initialize(const char* env_file = ".env");

    /**
     * @brief 重新加载配置
     * @param env_file .env 文件路径
     * @return 是否重新加载成功
     */
    bool reload(const char* env_file = ".env");

    /**
     * @brief 检查配置是否已初始化
     * @return 是否已初始化
     */
    bool isInitialized() const { return initialized_; }

    // ========== 日志配置 ==========
    std::string getLogLevel() const { return log_level_; }
    std::string getLogPattern() const { return log_pattern_; }
    std::string getLogFilePath() const { return log_file_path_; }
    bool isLogConsoleEnabled() const { return log_console_enabled_; }
    bool isLogFileEnabled() const { return log_file_enabled_; }
    size_t getLogMaxFileSize() const { return log_max_file_size_; }
    size_t getLogMaxFiles() const { return log_max_files_; }
    bool isLogAsyncEnabled() const { return log_async_enabled_; }
    size_t getLogAsyncQueueSize() const { return log_async_queue_size_; }

    // ========== 数据库配置 ==========
    std::string getDbHost() const { return db_host_; }
    int getDbPort() const { return db_port_; }
    std::string getDbName() const { return db_name_; }
    std::string getDbUser() const { return db_user_; }
    std::string getDbPassword() const { return db_password_; }
    int getDbPoolSize() const { return db_pool_size_; }
    int getDbTimeout() const { return db_timeout_; }
    std::string getDbCharset() const { return db_charset_; }

    /**
     * @brief 获取数据库连接字符串
     * @return 连接字符串
     */
    std::string getDbConnectionString() const;

    // ========== 数据源配置 ==========
    std::string getDataSourceUrl() const { return data_source_url_; }
    std::string getDataSourceApiKey() const { return data_source_api_key_; }
    int getDataSourceTimeout() const { return data_source_timeout_; }
    int getDataSourceRetryTimes() const { return data_source_retry_times_; }
    int getDataSourceRetryDelay() const { return data_source_retry_delay_; }

    // ========== Tushare 限流配置 ==========
    int getTushareRateLimit() const { return tushare_rate_limit_; }
    int getTushareBurstSize() const { return tushare_burst_size_; }

    // ========== 缓存配置 ==========
    bool isCacheEnabled() const { return cache_enabled_; }
    size_t getCacheSize() const { return cache_size_; }
    int getCacheTtl() const { return cache_ttl_; }
    std::string getCachePolicy() const { return cache_policy_; }

    // ========== 输出配置 ==========
    std::string getOutputDir() const { return output_dir_; }
    std::string getExportFormat() const { return export_format_; }
    bool isChartEnabled() const { return chart_enabled_; }
    int getConsoleWidth() const { return console_width_; }

    // ========== 分析配置 ==========
    int getDefaultMaPeriod() const { return default_ma_period_; }
    int getDefaultMacdFast() const { return default_macd_fast_; }
    int getDefaultMacdSlow() const { return default_macd_slow_; }
    int getDefaultMacdSignal() const { return default_macd_signal_; }
    int getDefaultRsiPeriod() const { return default_rsi_period_; }
    std::string getStrategies() const { return strategies_; }
    std::string getAnalysisDateSwitchTime() const { return analysis_date_switch_time_; }

    // ========== 应用配置 ==========
    std::string getAppName() const { return app_name_; }
    std::string getAppVersion() const { return app_version_; }
    std::string getAppEnv() const { return app_env_; }
    bool isDebugMode() const { return debug_mode_; }

    // 禁用拷贝和赋值
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

private:
    Config() = default;
    ~Config() = default;

    /**
     * @brief 加载配置
     */
    void loadConfig();

    /**
     * @brief 从环境变量获取字符串值
     * @param key 环境变量名
     * @param default_value 默认值
     * @return 环境变量值或默认值
     */
    std::string getEnvString(const char* key, const std::string& default_value) const;

    /**
     * @brief 从环境变量获取整数值
     * @param key 环境变量名
     * @param default_value 默认值
     * @return 环境变量值或默认值
     */
    int getEnvInt(const char* key, int default_value) const;

    /**
     * @brief 从环境变量获取布尔值
     * @param key 环境变量名
     * @param default_value 默认值
     * @return 环境变量值或默认值
     */
    bool getEnvBool(const char* key, bool default_value) const;

    /**
     * @brief 从环境变量获取 size_t 值
     * @param key 环境变量名
     * @param default_value 默认值
     * @return 环境变量值或默认值
     */
    size_t getEnvSize(const char* key, size_t default_value) const;

    // 线程安全
    mutable std::mutex mutex_;
    bool initialized_ = false;

    // ========== 日志配置 ==========
    std::string log_level_;
    std::string log_pattern_;
    std::string log_file_path_;
    bool log_console_enabled_;
    bool log_file_enabled_;
    size_t log_max_file_size_;
    size_t log_max_files_;
    bool log_async_enabled_;
    size_t log_async_queue_size_;

    // ========== 数据库配置 ==========
    std::string db_host_;
    int db_port_;
    std::string db_name_;
    std::string db_user_;
    std::string db_password_;
    int db_pool_size_;
    int db_timeout_;
    std::string db_charset_;

    // ========== 数据源配置 ==========
    std::string data_source_url_;
    std::string data_source_api_key_;
    int data_source_timeout_;
    int data_source_retry_times_;
    int data_source_retry_delay_;

    // ========== Tushare 限流配置 ==========
    int tushare_rate_limit_;
    int tushare_burst_size_;

    // ========== 缓存配置 ==========
    bool cache_enabled_;
    size_t cache_size_;
    int cache_ttl_;
    std::string cache_policy_;

    // ========== 输出配置 ==========
    std::string output_dir_;
    std::string export_format_;
    bool chart_enabled_;
    int console_width_;

    // ========== 分析配置 ==========
    int default_ma_period_;
    int default_macd_fast_;
    int default_macd_slow_;
    int default_macd_signal_;
    int default_rsi_period_;
    std::string strategies_;
    std::string analysis_date_switch_time_;

    // ========== 应用配置 ==========
    std::string app_name_;
    std::string app_version_;
    std::string app_env_;
    bool debug_mode_;
};

} // namespace config

#endif // CONFIG_H

