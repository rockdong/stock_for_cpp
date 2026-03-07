#include "CSVReader.h"
#include "Logger.h"
#include <sstream>
#include <algorithm>

namespace data {

bool CSVReader::open(const std::string& filepath) {
    file_.open(filepath);
    if (!file_.is_open()) {
        LOG_ERROR("无法打开 CSV 文件: " + filepath);
        return false;
    }
    
    currentLine_ = 0;
    LOG_INFO("CSV 文件打开成功: " + filepath);
    return true;
}

void CSVReader::close() {
    if (file_.is_open()) {
        file_.close();
        LOG_DEBUG("CSV 文件已关闭");
    }
}

std::vector<std::string> CSVReader::readLine() {
    if (!file_.is_open()) {
        LOG_ERROR("文件未打开");
        return {};
    }
    
    std::string line;
    while (std::getline(file_, line)) {
        currentLine_++;
        
        // 跳过空行
        if (skipEmptyLines_ && line.empty()) {
            continue;
        }
        
        return parseLine(line);
    }
    
    return {};
}

std::vector<std::vector<std::string>> CSVReader::readAll() {
    std::vector<std::vector<std::string>> data;
    
    std::vector<std::string> row;
    while (!(row = readLine()).empty()) {
        data.push_back(row);
    }
    
    LOG_INFO("读取 CSV 数据完成，共 " + std::to_string(data.size()) + " 行");
    return data;
}

std::vector<std::string> CSVReader::readHeader() {
    if (!file_.is_open()) {
        LOG_ERROR("文件未打开");
        return {};
    }
    
    // 重置到文件开头
    file_.clear();
    file_.seekg(0);
    currentLine_ = 0;
    
    return readLine();
}

bool CSVReader::eof() const {
    return file_.eof();
}

void CSVReader::setDelimiter(char delimiter) {
    delimiter_ = delimiter;
    LOG_DEBUG("CSV 分隔符设置为: " + std::string(1, delimiter_));
}

void CSVReader::setSkipEmptyLines(bool skip) {
    skipEmptyLines_ = skip;
}

size_t CSVReader::getCurrentLine() const {
    return currentLine_;
}

std::vector<std::string> CSVReader::parseLine(const std::string& line) {
    std::vector<std::string> fields;
    std::string field;
    bool inQuotes = false;
    
    for (size_t i = 0; i < line.length(); ++i) {
        char c = line[i];
        
        if (c == '"') {
            // 处理引号
            if (inQuotes && i + 1 < line.length() && line[i + 1] == '"') {
                // 双引号转义
                field += '"';
                ++i;
            } else {
                // 切换引号状态
                inQuotes = !inQuotes;
            }
        } else if (c == delimiter_ && !inQuotes) {
            // 分隔符（不在引号内）
            fields.push_back(trim(field));
            field.clear();
        } else {
            // 普通字符
            field += c;
        }
    }
    
    // 添加最后一个字段
    fields.push_back(trim(field));
    
    return fields;
}

std::string CSVReader::trim(const std::string& str) {
    size_t start = 0;
    size_t end = str.length();
    
    // 去除前导空白
    while (start < end && std::isspace(static_cast<unsigned char>(str[start]))) {
        ++start;
    }
    
    // 去除尾部空白
    while (end > start && std::isspace(static_cast<unsigned char>(str[end - 1]))) {
        --end;
    }
    
    return str.substr(start, end - start);
}

} // namespace data

