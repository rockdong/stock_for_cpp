#include "DataSourceFactory.h"
#include "TushareDataSource.h"
#include "Config.h"
#include "Logger.h"

namespace network {

DataSourcePtr DataSourceFactory::createDataSource(DataSourceType type, const std::string& config) {
    LOG_DEBUG("创建数据源");
    
    switch (type) {
        case DataSourceType::TUSHARE:
            LOG_INFO("创建 Tushare 数据源");
            if (config.empty()) {
                // 从配置模块创建
                return std::make_shared<TushareDataSource>();
            } else {
                // 使用指定的 Token
                return std::make_shared<TushareDataSource>(config);
            }
        
        case DataSourceType::CSV:
            LOG_WARN("CSV 数据源尚未实现");
            throw std::runtime_error("CSV data source not implemented yet");
        
        case DataSourceType::DATABASE:
            LOG_WARN("数据库数据源尚未实现");
            throw std::runtime_error("Database data source not implemented yet");
        
        default:
            LOG_ERROR("未知的数据源类型");
            throw std::invalid_argument("Unknown data source type");
    }
}

DataSourcePtr DataSourceFactory::createFromConfig() {
    LOG_DEBUG("从配置创建数据源");
    
    // 默认创建 Tushare 数据源（从配置模块获取参数）
    return createDataSource(DataSourceType::TUSHARE, "");
}

} // namespace network

