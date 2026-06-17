#include <iostream>
#include <vector>
#include <string>

// 核心库
#include "../stock_core/stock_core.h"
#include "../core/Stock.h"  // 包含完整的 Stock 定义

// exe 客户端模块
#include "CliOptions.h"
#include "CliProgress.h"
#include "SignalHandler.h"

// 版本信息
#include "version.h"  // CMake 生成的 version.h 在 build 目录

using namespace stock_exe;

int main(int argc, char* argv[]) {
    try {
        // 1. 解析命令行参数
        auto options = parseArgs(argc, argv);
        
        if (options.help) {
            printHelp(argv[0]);
            return 0;
        }
        
        // 2. 注册信号处理器
        SignalHandler::registerHandlers();
        
        // 3. 初始化核心库
        if (!stock_core::initialize(".env")) {
            std::cerr << "核心库初始化失败" << std::endl;
            return 1;
        }
        
        // 4. 打印版本信息
        printVersion();
        
        // 5. 加载股票列表
        auto stocks = stock_core::api::loadStockList();
        
        if (stocks.empty()) {
            std::cerr << "股票列表为空，无法执行分析" << std::endl;
            stock_core::shutdown();
            return 1;
        }
        
        std::cout << "加载股票数量: " << stocks.size() << std::endl;
        
        // 6. 设置进度回调
        CliProgress progress;
        stock_core::api::setProgressCallback(progress.getCallback());
        
        // 7. 执行分析
        if (options.onceMode) {
            std::cout << "单次执行模式..." << std::endl;
            stock_core::api::performDailyAnalysis(stocks);
        } else {
            std::cout << "定时模式，执行时间: " << options.executeTime << std::endl;
            
            // 设置调度器指针（用于信号处理）
            SignalHandler::setScheduler(&stock_core::api::getScheduler());
            
            stock_core::api::runScheduler(options.executeTime);
        }
        
        // 8. 清理核心库
        stock_core::shutdown();
        
        std::cout << std::endl;
        std::cout << "应用程序正常退出" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << std::endl;
        std::cerr << "程序异常: " << e.what() << std::endl;
        stock_core::shutdown();
        return 1;
    } catch (...) {
        std::cerr << std::endl;
        std::cerr << "程序发生未知异常" << std::endl;
        stock_core::shutdown();
        return 1;
    }
}