#pragma once

#include <string>

namespace stock_exe {

/**
 * @brief 命令行选项结构
 */
struct ProgramOptions {
    bool onceMode = false;      // 单次执行模式
    std::string executeTime = "20:00";  // 执行时间 (HH:MM)
    bool help = false;          // 显示帮助
};

/**
 * @brief 解析命令行参数
 * @param argc 参数数量
 * @param argv 参数数组
 * @return 解析后的选项
 */
ProgramOptions parseArgs(int argc, char* argv[]);

/**
 * @brief 打印帮助信息
 * @param programName 程序名称
 */
void printHelp(const char* programName);

/**
 * @brief 打印版本信息
 */
void printVersion();

} // namespace stock_exe