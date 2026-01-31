#include "Network.h"
#include "Logger.h"
#include "Config.h"
#include <iostream>

/**
 * @brief 网络层使用示例
 * 
 * 演示如何使用网络层模块获取股票数据
 */

void example1_basic_usage() {
    std::cout << "\n========== 示例1: 基础使用 ==========\n" << std::endl;
    
    // 创建数据源（从配置文件）
    auto source = network::DataSourceFactory::createFromConfig();
    
    // 测试连接
    if (source->testConnection()) {
        LOG_INFO("✅ Tushare 连接成功");
    } else {
        LOG_ERROR("❌ Tushare 连接失败");
        return;
    }
    
    // 获取股票列表（前10只）
    auto stocks = source->getStockList();
    LOG_INFO("获取到 " + std::to_string(stocks.size()) + " 只股票");
    
    std::cout << "\n前10只股票信息：" << std::endl;
    for (size_t i = 0; i < std::min(stocks.size(), size_t(10)); ++i) {
        const auto& stock = stocks[i];
        std::cout << stock.ts_code << " | " 
                  << stock.name << " | " 
                  << stock.industry << " | " 
                  << stock.area << std::endl;
    }
}

void example2_get_daily_data() {
    std::cout << "\n========== 示例2: 获取日线数据 ==========\n" << std::endl;
    
    auto source = network::DataSourceFactory::createFromConfig();
    
    // 获取平安银行最近30天的数据
    auto data = source->getDailyData("000001.SZ", "20240101", "20240131");
    
    LOG_INFO("获取到 " + std::to_string(data.size()) + " 条数据");
    
    std::cout << "\n平安银行(000001.SZ) 2024年1月行情：" << std::endl;
    std::cout << "日期       | 开盘   | 最高   | 最低   | 收盘   | 涨跌幅  | 成交量(手)" << std::endl;
    std::cout << "-----------|--------|--------|--------|--------|---------|------------" << std::endl;
    
    for (const auto& d : data) {
        printf("%s | %.2f | %.2f | %.2f | %.2f | %+.2f%% | %ld\n",
               d.trade_date.c_str(),
               d.open, d.high, d.low, d.close,
               d.pct_chg, d.volume);
    }
}

void example3_get_latest_quote() {
    std::cout << "\n========== 示例3: 获取最新行情 ==========\n" << std::endl;
    
    auto source = network::DataSourceFactory::createFromConfig();
    
    // 获取多只股票的最新行情
    std::vector<std::string> codes = {
        "000001.SZ",  // 平安银行
        "000002.SZ",  // 万科A
        "600000.SH",  // 浦发银行
        "600036.SH"   // 招商银行
    };
    
    std::cout << "\n最新行情：" << std::endl;
    std::cout << "股票代码    | 收盘价 | 涨跌幅  | 成交量(手)" << std::endl;
    std::cout << "------------|--------|---------|------------" << std::endl;
    
    for (const auto& code : codes) {
        auto quote = source->getLatestQuote(code);
        if (!quote.ts_code.empty()) {
            printf("%s | %.2f | %+.2f%% | %ld\n",
                   quote.ts_code.c_str(),
                   quote.close,
                   quote.pct_chg,
                   quote.volume);
        }
    }
}

void example4_direct_api_call() {
    std::cout << "\n========== 示例4: 直接调用 Tushare API ==========\n" << std::endl;
    
    // 创建 Tushare 客户端（从配置模块自动获取参数）
    network::TushareClient client;
    
    // 获取交易日历
    auto response = client.getTradeCal("20240101", "20240131", "SSE");
    
    if (response.isSuccess()) {
        LOG_INFO("✅ 获取交易日历成功");
        
        auto fields = response.data["fields"];
        auto items = response.data["items"];
        
        std::cout << "\n2024年1月上交所交易日历（前10天）：" << std::endl;
        std::cout << "日期       | 是否交易" << std::endl;
        std::cout << "-----------|----------" << std::endl;
        
        for (size_t i = 0; i < std::min(items.size(), size_t(10)); ++i) {
            std::string date = items[i][1].get<std::string>();
            int is_open = items[i][2].get<int>();
            std::cout << date << " | " << (is_open ? "✅ 交易日" : "❌ 休市") << std::endl;
        }
    } else {
        LOG_ERROR("❌ 获取交易日历失败: " + response.msg);
    }
}

void example5_http_client() {
    std::cout << "\n========== 示例5: 使用 HTTP 客户端 ==========\n" << std::endl;
    
    // 从配置获取参数
    auto& config = config::Config::getInstance();
    std::string url = config.getDataSourceUrl();
    int timeout = config.getDataSourceTimeout();
    std::string token = config.getDataSourceApiKey();
    
    // 创建 HTTP 客户端
    network::HttpClient client(url, timeout);
    
    // 配置重试
    client.setRetryTimes(config.getDataSourceRetryTimes());
    client.setRetryDelay(config.getDataSourceRetryDelay());
    
    // 构建请求
    std::string request_body = R"({
        "api_name": "stock_basic",
        "token": ")" + token + R"(",
        "params": {"list_status": "L"},
        "fields": ""
    })";
    
    // 发送 POST 请求
    auto response = client.post("/", request_body, "application/json");
    
    if (response.isSuccess()) {
        LOG_INFO("✅ HTTP 请求成功");
        LOG_INFO("状态码: " + std::to_string(response.status_code));
        LOG_INFO("响应长度: " + std::to_string(response.body.length()) + " 字节");
    } else {
        LOG_ERROR("❌ HTTP 请求失败: " + response.error_message);
    }
}

int main() {
    try {
        // 初始化配置和日志
        auto& config = config::Config::getInstance();
        config.initialize(".env");
        logger::init(config);
        
        LOG_INFO("========================================");
        LOG_INFO("    网络层模块使用示例");
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
        example1_basic_usage();
        example2_get_daily_data();
        example3_get_latest_quote();
        example4_direct_api_call();
        example5_http_client();
        
        LOG_INFO("\n========================================");
        LOG_INFO("    所有示例运行完成");
        LOG_INFO("========================================");
        
    } catch (const std::exception& e) {
        LOG_ERROR("程序异常: " + std::string(e.what()));
        return 1;
    }
    
    return 0;
}

