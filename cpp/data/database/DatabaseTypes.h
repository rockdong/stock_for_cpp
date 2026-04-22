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
    
    ColumnDefinition() = default;
    
    ColumnDefinition(const std::string& n, DataType t, bool pk = false, bool ai = false)
        : name(n), type(t), primaryKey(pk), autoIncrement(ai) {}
};

struct IndexDefinition {
    std::string name;
    std::string tableName;
    std::vector<std::string> columns;
    bool unique = false;
    
    IndexDefinition() = default;
    
    IndexDefinition(const std::string& n, const std::string& tbl, 
                    const std::vector<std::string>& cols, bool u = false)
        : name(n), tableName(tbl), columns(cols), unique(u) {}
};

struct TableDefinition {
    std::string name;
    std::vector<ColumnDefinition> columns;
    std::vector<IndexDefinition> indexes;
    
    TableDefinition() = default;
    
    TableDefinition(const std::string& n, const std::vector<ColumnDefinition>& cols)
        : name(n), columns(cols) {}
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