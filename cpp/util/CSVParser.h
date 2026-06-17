#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "../core/Stock.h"

namespace util {

/**
 * @brief CSV 文件解析器
 * 
 * 支持从 CSV 文件导入股票列表到系统
 */
class CSVParser {
public:
    /**
     * @brief 解析 CSV 文件并返回股票列表
     * @param filePath CSV 文件路径
     * @return 股票列表
     * @throws std::runtime_error 如果文件不存在或格式错误
     */
    static std::vector<core::Stock> parseStockList(const std::string& filePath);
    
private:
    /**
     * @brief 解析单行 CSV 数据
     * @param line CSV 行数据
     * @param lineNum 行号（用于错误提示）
     * @return Stock 对象
     */
    static core::Stock parseStockLine(const std::string& line, int lineNum);
    
    /**
     * @brief 分割 CSV 行为字段列表
     * @param line CSV 行
     * @return 字段列表
     */
    static std::vector<std::string> splitCSVLine(const std::string& line);
};

} // namespace util