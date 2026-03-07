#include "CSVWriter.h"
#include "Logger.h"

namespace data {

CSVWriter::~CSVWriter() {
    close();
}

bool CSVWriter::open(const std::string& filepath, bool append) {
    auto mode = append ? std::ios::app : std::ios::out;
    file_.open(filepath, mode);
    
    if (!file_.is_open()) {
        LOG_ERROR("无法打开 CSV 文件: " + filepath);
        return false;
    }
    
    writtenLines_ = 0;
    LOG_INFO("CSV 文件打开成功: " + filepath + (append ? " (追加模式)" : " (覆盖模式)"));
    return true;
}

void CSVWriter::close() {
    if (file_.is_open()) {
        file_.close();
        LOG_DEBUG("CSV 文件已关闭，共写入 " + std::to_string(writtenLines_) + " 行");
    }
}

bool CSVWriter::writeLine(const std::vector<std::string>& row) {
    if (!file_.is_open()) {
        LOG_ERROR("文件未打开");
        return false;
    }
    
    if (row.empty()) {
        return true;
    }
    
    for (size_t i = 0; i < row.size(); ++i) {
        if (i > 0) {
            file_ << delimiter_;
        }
        file_ << escapeField(row[i]);
    }
    
    file_ << '\n';
    writtenLines_++;
    
    return true;
}

size_t CSVWriter::writeLines(const std::vector<std::vector<std::string>>& rows) {
    size_t successCount = 0;
    
    for (const auto& row : rows) {
        if (writeLine(row)) {
            successCount++;
        }
    }
    
    LOG_INFO("批量写入 CSV 数据完成: " + std::to_string(successCount) + "/" + std::to_string(rows.size()));
    return successCount;
}

bool CSVWriter::writeHeader(const std::vector<std::string>& headers) {
    return writeLine(headers);
}

void CSVWriter::setDelimiter(char delimiter) {
    delimiter_ = delimiter;
    LOG_DEBUG("CSV 分隔符设置为: " + std::string(1, delimiter_));
}

void CSVWriter::setAutoQuote(bool quote) {
    autoQuote_ = quote;
}

void CSVWriter::flush() {
    if (file_.is_open()) {
        file_.flush();
    }
}

size_t CSVWriter::getWrittenLines() const {
    return writtenLines_;
}

std::string CSVWriter::escapeField(const std::string& field) {
    if (!autoQuote_ && !needsQuotes(field)) {
        return field;
    }
    
    if (needsQuotes(field)) {
        std::string escaped = "\"";
        for (char c : field) {
            if (c == '"') {
                escaped += "\"\"";  // 双引号转义
            } else {
                escaped += c;
            }
        }
        escaped += "\"";
        return escaped;
    }
    
    return field;
}

bool CSVWriter::needsQuotes(const std::string& field) {
    // 如果字段包含分隔符、引号、换行符，则需要引号
    for (char c : field) {
        if (c == delimiter_ || c == '"' || c == '\n' || c == '\r') {
            return true;
        }
    }
    return false;
}

} // namespace data

