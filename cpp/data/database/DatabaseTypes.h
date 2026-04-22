#ifndef DATA_DATABASE_DATABASE_TYPES_H
#define DATA_DATABASE_DATABASE_TYPES_H

#include <string>
#include <vector>
#include <map>

namespace data {

enum class DataType {
    TEXT,
    INTEGER,
    DECIMAL,
    DATETIME,
    TIMESTAMP,
    DATE,
    JSON,
    BOOLEAN,
    REAL,
    BIGINT
};

struct ColumnDefinition {
    std::string name;
    DataType type;
    
    int precision = 0;
    int scale = 0;
    int varcharLength = 0;
    
    bool nullable = true;
    std::string defaultValue;
    bool autoIncrement = false;
    bool primaryKey = false;
    bool unique = false;
    std::string checkConstraint;
    std::string foreignKeyTable;
    std::string foreignKeyColumn;
    
    // 不提供自定义构造器，允许聚合初始化
};

struct IndexDefinition {
    std::string name;
    std::string tableName;
    std::vector<std::string> columns;
    bool unique = false;
    
    // 不提供自定义构造器，允许聚合初始化
};

struct TableDefinition {
    std::string name;
    std::vector<ColumnDefinition> columns;
    std::vector<IndexDefinition> indexes;
    
    // 不提供自定义构造器，允许聚合初始化
};

inline std::string dataTypeToString(DataType type) {
    switch (type) {
        case DataType::TEXT:      return "TEXT";
        case DataType::INTEGER:   return "INTEGER";
        case DataType::DECIMAL:   return "DECIMAL";
        case DataType::DATETIME:  return "DATETIME";
        case DataType::TIMESTAMP: return "TIMESTAMP";
        case DataType::DATE:      return "DATE";
        case DataType::JSON:      return "JSON";
        case DataType::BOOLEAN:   return "BOOLEAN";
        case DataType::REAL:      return "REAL";
        case DataType::BIGINT:    return "BIGINT";
        default:                  return "TEXT";
    }
}

} // namespace data

#endif // DATA_DATABASE_DATABASE_TYPES_H