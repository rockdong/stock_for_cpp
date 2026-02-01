#include "Network.h"
#include "Logger.h"
#include "Config.h"
#include <iostream>
#include <iomanip>

/**
 * @brief 股票列表查询示例
 * 
 * 演示如何使用网络层模块查询股票列表
 */

void printStockBasic(const network::StockBasic& stock) {
    std::cout << std::left
              << std::setw(12) << stock.ts_code
              << std::setw(10) << stock.symbol
              << std::setw(20) << stock.name
              << std::setw(10) << stock.area
              << std::setw(15) << stock.industry
              << std::setw(10) << stock.market
              << std::setw(12) << stock.list_date
              << std::setw(8) << stock.list_status
              << std::endl;
}

void example1_get_all_stocks() {
    std::cout << "\n========== 示例1: 获取所有上市股票 ==========\n" << std::endl;
    
    auto source = network::DataSourceFactory::createFromConfig();
    
    // 获取所有上市股票（默认参数）
    auto stocks = source->getStockList();
    
    LOG_INFO("获取到 " + std::to_string(stocks.size()) + " 只股票");
    
    std::cout << "\n前20只股票信息：" << std::endl;
    std::cout << std::left
              << std::setw(12) << "TS代码"
              << std::setw(10) << "代码"
              << std::setw(20) << "名称"
              << std::setw(10) << "地域"
              << std::setw(15) << "行业"
              << std::setw(10) << "市场"
              << std::setw(12) << "上市日期"
              << std::setw(8) << "状态"
              << std::endl;
    std::cout << std::string(100, '-') << std::endl;
    
    for (size_t i = 0; i < std::min(stocks.size(), size_t(20)); ++i) {
        printStockBasic(stocks[i]);
    }
}

void example2_get_stocks_by_exchange() {
    std::cout << "\n========== 示例2: 按交易所查询股票 ==========\n" << std::endl;
    
    auto source = network::DataSourceFactory::createFromConfig();
    
    // 获取上交所上市股票
    std::cout << "\n上交所（SSE）上市股票：" << std::endl;
    auto sse_stocks = source->getStockList("L", "SSE");
    LOG_INFO("上交所股票数量: " + std::to_string(sse_stocks.size()));
    
    std::cout << std::left
              << std::setw(12) << "TS代码"
              << std::setw(10) << "代码"
              << std::setw(20) << "名称"
              << std::setw(10) << "地域"
              << std::setw(15) << "行业"
              << std::endl;
    std::cout << std::string(70, '-') << std::endl;
    
    for (size_t i = 0; i < std::min(sse_stocks.size(), size_t(10)); ++i) {
        const auto& stock = sse_stocks[i];
        std::cout << std::left
                  << std::setw(12) << stock.ts_code
                  << std::setw(10) << stock.symbol
                  << std::setw(20) << stock.name
                  << std::setw(10) << stock.area
                  << std::setw(15) << stock.industry
                  << std::endl;
    }
    
    // 获取深交所上市股票
    std::cout << "\n深交所（SZSE）上市股票：" << std::endl;
    auto szse_stocks = source->getStockList("L", "SZSE");
    LOG_INFO("深交所股票数量: " + std::to_string(szse_stocks.size()));
    
    std::cout << std::left
              << std::setw(12) << "TS代码"
              << std::setw(10) << "代码"
              << std::setw(20) << "名称"
              << std::setw(10) << "地域"
              << std::setw(15) << "行业"
              << std::endl;
    std::cout << std::string(70, '-') << std::endl;
    
    for (size_t i = 0; i < std::min(szse_stocks.size(), size_t(10)); ++i) {
        const auto& stock = szse_stocks[i];
        std::cout << std::left
                  << std::setw(12) << stock.ts_code
                  << std::setw(10) << stock.symbol
                  << std::setw(20) << stock.name
                  << std::setw(10) << stock.area
                  << std::setw(15) << stock.industry
                  << std::endl;
    }
}

void example3_get_stock_info() {
    std::cout << "\n========== 示例3: 查询指定股票信息 ==========\n" << std::endl;
    
    auto source = network::DataSourceFactory::createFromConfig();
    
    // 查询几只知名股票
    std::vector<std::string> codes = {
        "000001.SZ",  // 平安银行
        "000002.SZ",  // 万科A
        "600000.SH",  // 浦发银行
        "600036.SH",  // 招商银行
        "000858.SZ"   // 五粮液
    };
    
    std::cout << "\n股票详细信息：" << std::endl;
    std::cout << std::string(120, '=') << std::endl;
    
    for (const auto& code : codes) {
        auto stock = source->getStockInfo(code);
        
        if (!stock.ts_code.empty()) {
            std::cout << "TS代码: " << stock.ts_code << std::endl;
            std::cout << "股票代码: " << stock.symbol << std::endl;
            std::cout << "股票名称: " << stock.name << std::endl;
            std::cout << "股票全称: " << stock.fullname << std::endl;
            std::cout << "英文名称: " << stock.enname << std::endl;
            std::cout << "拼音缩写: " << stock.cnspell << std::endl;
            std::cout << "所属地域: " << stock.area << std::endl;
            std::cout << "所属行业: " << stock.industry << std::endl;
            std::cout << "市场类型: " << stock.market << std::endl;
            std::cout << "交易所: " << stock.exchange << std::endl;
            std::cout << "交易货币: " << stock.curr_type << std::endl;
            std::cout << "上市状态: " << stock.list_status << std::endl;
            std::cout << "上市日期: " << stock.list_date << std::endl;
            std::cout << "沪深港通: " << stock.is_hs << std::endl;
            std::cout << std::string(120, '-') << std::endl;
        }
    }
}

void example4_get_delisted_stocks() {
    std::cout << "\n========== 示例4: 查询退市股票 ==========\n" << std::endl;
    
    auto source = network::DataSourceFactory::createFromConfig();
    
    // 获取退市股票
    auto delisted = source->getStockList("D", "");
    
    LOG_INFO("退市股票数量: " + std::to_string(delisted.size()));
    
    std::cout << "\n部分退市股票信息（前10只）：" << std::endl;
    std::cout << std::left
              << std::setw(12) << "TS代码"
              << std::setw(10) << "代码"
              << std::setw(20) << "名称"
              << std::setw(12) << "上市日期"
              << std::setw(12) << "退市日期"
              << std::endl;
    std::cout << std::string(70, '-') << std::endl;
    
    for (size_t i = 0; i < std::min(delisted.size(), size_t(10)); ++i) {
        const auto& stock = delisted[i];
        std::cout << std::left
                  << std::setw(12) << stock.ts_code
                  << std::setw(10) << stock.symbol
                  << std::setw(20) << stock.name
                  << std::setw(12) << stock.list_date
                  << std::setw(12) << stock.delist_date
                  << std::endl;
    }
}

void example5_filter_stocks() {
    std::cout << "\n========== 示例5: 筛选特定行业股票 ==========\n" << std::endl;
    
    auto source = network::DataSourceFactory::createFromConfig();
    
    // 获取所有上市股票
    auto all_stocks = source->getStockList();
    
    // 筛选银行业股票
    std::vector<network::StockBasic> bank_stocks;
    for (const auto& stock : all_stocks) {
        if (stock.industry == "银行") {
            bank_stocks.push_back(stock);
        }
    }
    
    LOG_INFO("银行业股票数量: " + std::to_string(bank_stocks.size()));
    
    std::cout << "\n银行业股票列表：" << std::endl;
    std::cout << std::left
              << std::setw(12) << "TS代码"
              << std::setw(10) << "代码"
              << std::setw(20) << "名称"
              << std::setw(10) << "地域"
              << std::setw(12) << "上市日期"
              << std::endl;
    std::cout << std::string(70, '-') << std::endl;
    
    for (const auto& stock : bank_stocks) {
        std::cout << std::left
                  << std::setw(12) << stock.ts_code
                  << std::setw(10) << stock.symbol
                  << std::setw(20) << stock.name
                  << std::setw(10) << stock.area
                  << std::setw(12) << stock.list_date
                  << std::endl;
    }
}

int main() {
    try {
        // 初始化配置和日志
        auto& config = config::Config::getInstance();
        config.initialize(".env");
        logger::init(config);
        
        LOG_INFO("========================================");
        LOG_INFO("    股票列表查询示例");
        LOG_INFO("========================================");
        
        // 检查 API Token
        std::string token = config.getDataSourceApiKey();
        if (token.empty() || token == "your_tushare_token_here") {
            LOG_ERROR("请先在 .env 文件中配置 DATA_SOURCE_API_KEY");
            std::cout << "\n⚠️  请先配置 Tushare API Token：" << std::endl;
            std::cout << "1. 访问 https://tushare.pro/ 注册并获取 Token" << std::endl;
            std::cout << "2. 在 .env 文件中设置 DATA_SOURCE_API_KEY=你的Token" << std::endl;
            return 1;
        }
        
        // 运行示例
        example1_get_all_stocks();
        example2_get_stocks_by_exchange();
        example3_get_stock_info();
        example4_get_delisted_stocks();
        example5_filter_stocks();
        
        LOG_INFO("\n========================================");
        LOG_INFO("    所有示例运行完成");
        LOG_INFO("========================================");
        
    } catch (const std::exception& e) {
        LOG_ERROR("程序异常: " + std::string(e.what()));
        return 1;
    }
    
    return 0;
}

