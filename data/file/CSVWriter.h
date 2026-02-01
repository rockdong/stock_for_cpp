#ifndef DATA_CSV_WRITER_H
#define DATA_CSV_WRITER_H

#include <string>
#include <vector>
#include <fstream>

namespace data {

/**
 * @brief CSV 写入器
 * 职责：写入数据到 CSV 文件
 */
class CSVWriter {
public:
    CSVWriter() = default;
    ~CSVWriter();

    /**
     * @brief 打开 CSV 文件
     * @param filepath 文件路径
     * @param append 是否追加模式（默认为覆盖）
     * @return 是否成功
     */
    bool open(const std::string& filepath, bool append = false);

    /**
     * @brief 关闭文件
     */
    void close();

    /**
     * @brief 写入一行数据
     * @param row 行数据
     * @return 是否成功
     */
    bool writeLine(const std::vector<std::string>& row);

    /**
     * @brief 写入多行数据
     * @param rows 多行数据
     * @return 成功写入的行数
     */
    size_t writeLines(const std::vector<std::vector<std::string>>& rows);

    /**
     * @brief 写入表头
     * @param headers 表头列名
     * @return 是否成功
     */
    bool writeHeader(const std::vector<std::string>& headers);

    /**
     * @brief 设置分隔符
     * @param delimiter 分隔符（默认为逗号）
     */
    void setDelimiter(char delimiter);

    /**
     * @brief 设置是否自动添加引号
     * @param quote 是否添加引号
     */
    void setAutoQuote(bool quote);

    /**
     * @brief 刷新缓冲区
     */
    void flush();

    /**
     * @brief 获取已写入的行数
     * @return 行数
     */
    size_t getWrittenLines() const;

private:
    /**
     * @brief 转义字段（如果包含特殊字符）
     * @param field 字段
     * @return 转义后的字段
     */
    std::string escapeField(const std::string& field);

    /**
     * @brief 检查字段是否需要引号
     * @param field 字段
     * @return 是否需要
     */
    bool needsQuotes(const std::string& field);

    std::ofstream file_;
    char delimiter_ = ',';
    bool autoQuote_ = true;
    size_t writtenLines_ = 0;
};

} // namespace data

#endif // DATA_CSV_WRITER_H

