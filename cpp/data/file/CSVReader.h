#ifndef DATA_CSV_READER_H
#define DATA_CSV_READER_H

#include <string>
#include <vector>
#include <fstream>

namespace data {

/**
 * @brief CSV 读取器
 * 职责：读取和解析 CSV 文件
 */
class CSVReader {
public:
    CSVReader() = default;
    ~CSVReader() = default;

    /**
     * @brief 打开 CSV 文件
     * @param filepath 文件路径
     * @return 是否成功
     */
    bool open(const std::string& filepath);

    /**
     * @brief 关闭文件
     */
    void close();

    /**
     * @brief 读取下一行
     * @return 行数据（列的向量），如果到达文件末尾返回空向量
     */
    std::vector<std::string> readLine();

    /**
     * @brief 读取所有行
     * @return 所有行数据
     */
    std::vector<std::vector<std::string>> readAll();

    /**
     * @brief 读取表头
     * @return 表头列名
     */
    std::vector<std::string> readHeader();

    /**
     * @brief 检查是否到达文件末尾
     * @return 是否到达末尾
     */
    bool eof() const;

    /**
     * @brief 设置分隔符
     * @param delimiter 分隔符（默认为逗号）
     */
    void setDelimiter(char delimiter);

    /**
     * @brief 设置是否跳过空行
     * @param skip 是否跳过
     */
    void setSkipEmptyLines(bool skip);

    /**
     * @brief 获取当前行号
     * @return 行号
     */
    size_t getCurrentLine() const;

private:
    /**
     * @brief 解析一行 CSV 数据
     * @param line 原始行数据
     * @return 解析后的列数据
     */
    std::vector<std::string> parseLine(const std::string& line);

    /**
     * @brief 去除字符串两端的空白字符
     * @param str 字符串
     * @return 去除空白后的字符串
     */
    std::string trim(const std::string& str);

    std::ifstream file_;
    char delimiter_ = ',';
    bool skipEmptyLines_ = true;
    size_t currentLine_ = 0;
};

} // namespace data

#endif // DATA_CSV_READER_H

