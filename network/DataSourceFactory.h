#ifndef DATA_SOURCE_FACTORY_H
#define DATA_SOURCE_FACTORY_H

#include "IDataSource.h"
#include <string>
#include <memory>

namespace network {

/**
 * @brief 数据源工厂类
 * 
 * 使用工厂模式创建不同类型的数据源
 * 遵循开闭原则（OCP）和依赖倒置原则（DIP）
 */
class DataSourceFactory {
public:
    /**
     * @brief 数据源类型枚举
     */
    enum class DataSourceType {
        TUSHARE,        // Tushare Pro
        CSV,            // CSV 文件
        DATABASE        // 数据库
    };

    /**
     * @brief 创建数据源
     * @param type 数据源类型
     * @param config 配置参数（如 API Token、文件路径等）
     * @return 数据源指针
     */
    static DataSourcePtr createDataSource(DataSourceType type, const std::string& config);

    /**
     * @brief 从配置文件创建数据源
     * @return 数据源指针
     */
    static DataSourcePtr createFromConfig();
};

} // namespace network

#endif // DATA_SOURCE_FACTORY_H

