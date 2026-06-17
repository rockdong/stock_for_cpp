#include "CliOptions.h"
#include <iostream>
#include "version.h"  // CMake 生成的 version.h 在 build 目录

namespace stock_exe {

ProgramOptions parseArgs(int argc, char* argv[]) {
    ProgramOptions options;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--once" || arg == "-o") {
            options.onceMode = true;
        } else if (arg == "--help" || arg == "-h") {
            options.help = true;
        } else if (arg == "--time" || arg == "-t") {
            if (i + 1 < argc) {
                options.executeTime = argv[++i];
            }
        }
    }
    
    return options;
}

void printHelp(const char* programName) {
    std::cout << "用法: " << programName << " [选项]" << std::endl;
    std::cout << std::endl;
    std::cout << "选项:" << std::endl;
    std::cout << "  --once, -o       单次执行模式（默认：定时模式）" << std::endl;
    std::cout << "  --time, -t TIME  设置执行时间，格式 HH:MM（默认：20:00）" << std::endl;
    std::cout << "  --help, -h       显示帮助信息" << std::endl;
    std::cout << std::endl;
    std::cout << "示例:" << std::endl;
    std::cout << "  " << programName << "           # 定时模式，每天 20:00 执行" << std::endl;
    std::cout << "  " << programName << " --once   # 单次执行模式" << std::endl;
    std::cout << "  " << programName << " -t 09:30 # 每天 09:30 执行" << std::endl;
}

void printVersion() {
    std::cout << "========================================" << std::endl;
    std::cout << "📊 Stock Analysis System (exe 客户端)" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "版本号: " << VERSION_STRING << "." << VERSION_BUILD << std::endl;
    std::cout << "构建日期: " << BUILD_DATE << std::endl;
    std::cout << "Git 提交: " << GIT_COMMIT << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
}

} // namespace stock_exe