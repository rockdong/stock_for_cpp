#include "CSVParser.h"
#include <stdexcept>
#include <algorithm>

namespace util {

std::vector<core::Stock> CSVParser::parseStockList(const std::string& filePath) {
    std::ifstream file(filePath);
    
    if (!file.is_open()) {
        throw std::runtime_error("无法打开 CSV 文件: " + filePath);
    }
    
    std::vector<core::Stock> stocks;
    std::string line;
    int lineNum = 0;
    
    // 读取标题行
    if (!std::getline(file, line)) {
        throw std::runtime_error("CSV 文件为空或格式错误");
    }
    
    lineNum = 1;
    
    // 验证标题行格式（可选，只做简单检查）
    if (line.find("ts_code") == std::string::npos) {
        throw std::runtime_error("CSV 格式错误：缺少 ts_code 列（第一列应为 ts_code）");
    }
    
    // 逐行解析数据
    while (std::getline(file, line)) {
        lineNum++;
        
        // 跳过空行
        if (line.empty() || line.find_first_not_of(" \t\r\n") == std::string::npos) {
            continue;
        }
        
        try {
            auto stock = parseStockLine(line, lineNum);
            stocks.push_back(stock);
        } catch (const std::exception& e) {
            // 记录错误但继续解析下一行（宽松模式）
            // 可以在这里添加日志记录
            // LOG_WARN("CSV 第 " << lineNum << " 行解析失败: " << e.what());
        }
    }
    
    file.close();
    
    return stocks;
}

core::Stock CSVParser::parseStockLine(const std::string& line, int lineNum) {
    auto fields = splitCSVLine(line);
    
    // 验证字段数量（至少需要 ts_code 和 name）
    if (fields.size() < 2) {
        throw std::runtime_error("字段数量不足（至少需要 ts_code 和 name）");
    }
    
    core::Stock stock;
    
    // 解析字段（按 CSV 格式规范）
    stock.ts_code = fields[0];  // 第一列：ts_code
    stock.name = fields[1];     // 第二列：name
    
    // 第三列：industry（可选）
    if (fields.size() >= 3) {
        stock.industry = fields[2];
    }
    
    // 第四列：list_date（可选，格式：YYYY-MM-DD）
    if (fields.size() >= 4) {
        stock.list_date = fields[3];
    }
    
    // 第五列：market（可选）
    if (fields.size() >= 5) {
        stock.market = fields[4];
    }
    
    // 验证 ts_code 格式（应包含 .SS 或 .SZ）
    if (stock.ts_code.find(".") == std::string::npos) {
        throw std::runtime_error("ts_code 格式错误（应为 XXXXXX.SS 或 XXXXXX.SZ）");
    }
    
    return stock;
}

std::vector<std::string> CSVParser::splitCSVLine(const std::string& line) {
    std::vector<std::string> fields;
    std::stringstream ss(line);
    std::string field;
    
    while (std::getline(ss, field, ',')) {
        // 移除字段两端的空白字符
        field.erase(0, field.find_first_not_of(" \t\r\n"));
        field.erase(field.find_last_not_of(" \t\r\n") + 1);
        
        fields.push_back(field);
    }
    
    return fields;
}

} // namespace util