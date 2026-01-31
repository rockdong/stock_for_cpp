#ifndef TUSHARE_CLIENT_H
#define TUSHARE_CLIENT_H

#include "HttpClient.h"
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <map>

namespace network {

using json = nlohmann::json;

/**
 * @brief Tushare API 响应结构
 */
struct TushareResponse {
    int code;                    // 响应码（0表示成功）
    std::string msg;             // 响应消息
    json data;                   // 响应数据
    
    bool isSuccess() const {
        return code == 0;
    }
};

/**
 * @brief Tushare Pro API 客户端
 * 
 * 封装 Tushare Pro API 接口
 * API 文档: https://tushare.pro/document/1?doc_id=130
 * 
 * 从配置模块自动获取 URL 和 Token
 */
class TushareClient {
public:
    /**
     * @brief 默认构造函数（从配置模块获取参数）
     */
    TushareClient();
    
    /**
     * @brief 构造函数（手动指定参数）
     * @param api_token API Token
     * @param base_url API 基础 URL
     */
    explicit TushareClient(const std::string& api_token,
                          const std::string& base_url);
    
    ~TushareClient() = default;

    /**
     * @brief 通用 API 调用接口
     * @param api_name API 接口名称
     * @param params 请求参数
     * @param fields 返回字段（为空则返回所有字段）
     * @return Tushare 响应
     */
    TushareResponse query(const std::string& api_name,
                         const std::map<std::string, std::string>& params = {},
                         const std::vector<std::string>& fields = {});

    // ========== 基础数据接口 ==========
    
    /**
     * @brief 获取股票列表
     * @param list_status 上市状态（L上市 D退市 P暂停上市，默认L）
     * @param exchange 交易所（SSE上交所 SZSE深交所）
     * @return Tushare 响应
     */
    TushareResponse getStockBasic(const std::string& list_status = "L",
                                  const std::string& exchange = "");

    /**
     * @brief 获取交易日历
     * @param start_date 开始日期（YYYYMMDD）
     * @param end_date 结束日期（YYYYMMDD）
     * @param exchange 交易所（SSE上交所 SZSE深交所）
     * @return Tushare 响应
     */
    TushareResponse getTradeCal(const std::string& start_date = "",
                                const std::string& end_date = "",
                                const std::string& exchange = "");

    // ========== 行情数据接口 ==========
    
    /**
     * @brief 获取日线行情
     * @param ts_code 股票代码（如 000001.SZ）
     * @param trade_date 交易日期（YYYYMMDD）
     * @param start_date 开始日期（YYYYMMDD）
     * @param end_date 结束日期（YYYYMMDD）
     * @return Tushare 响应
     */
    TushareResponse getDailyQuote(const std::string& ts_code = "",
                                  const std::string& trade_date = "",
                                  const std::string& start_date = "",
                                  const std::string& end_date = "");

    /**
     * @brief 获取复权因子
     * @param ts_code 股票代码
     * @param trade_date 交易日期
     * @param start_date 开始日期
     * @param end_date 结束日期
     * @return Tushare 响应
     */
    TushareResponse getAdjFactor(const std::string& ts_code = "",
                                 const std::string& trade_date = "",
                                 const std::string& start_date = "",
                                 const std::string& end_date = "");

    /**
     * @brief 获取每日指标
     * @param ts_code 股票代码
     * @param trade_date 交易日期
     * @param start_date 开始日期
     * @param end_date 结束日期
     * @return Tushare 响应
     */
    TushareResponse getDailyBasic(const std::string& ts_code = "",
                                  const std::string& trade_date = "",
                                  const std::string& start_date = "",
                                  const std::string& end_date = "");

    // ========== 财务数据接口 ==========
    
    /**
     * @brief 获取利润表
     * @param ts_code 股票代码
     * @param period 报告期（YYYYMMDD）
     * @param start_date 开始日期
     * @param end_date 结束日期
     * @return Tushare 响应
     */
    TushareResponse getIncome(const std::string& ts_code = "",
                             const std::string& period = "",
                             const std::string& start_date = "",
                             const std::string& end_date = "");

    /**
     * @brief 获取资产负债表
     * @param ts_code 股票代码
     * @param period 报告期
     * @param start_date 开始日期
     * @param end_date 结束日期
     * @return Tushare 响应
     */
    TushareResponse getBalanceSheet(const std::string& ts_code = "",
                                    const std::string& period = "",
                                    const std::string& start_date = "",
                                    const std::string& end_date = "");

    /**
     * @brief 获取现金流量表
     * @param ts_code 股票代码
     * @param period 报告期
     * @param start_date 开始日期
     * @param end_date 结束日期
     * @return Tushare 响应
     */
    TushareResponse getCashFlow(const std::string& ts_code = "",
                                const std::string& period = "",
                                const std::string& start_date = "",
                                const std::string& end_date = "");

    // ========== 市场参考数据 ==========
    
    /**
     * @brief 获取沪深股通成份股
     * @param ts_code 股票代码
     * @param hs_type 沪深港通类型（SH沪股通 SZ深股通）
     * @return Tushare 响应
     */
    TushareResponse getHsConst(const std::string& ts_code = "",
                               const std::string& hs_type = "");

    /**
     * @brief 设置超时时间
     * @param seconds 超时秒数
     */
    void setTimeout(int seconds);

    /**
     * @brief 设置重试次数
     * @param times 重试次数
     */
    void setRetryTimes(int times);

private:
    /**
     * @brief 构建请求 JSON
     * @param api_name API 名称
     * @param params 参数
     * @param fields 字段
     * @return JSON 字符串
     */
    std::string buildRequestJson(const std::string& api_name,
                                 const std::map<std::string, std::string>& params,
                                 const std::vector<std::string>& fields);

    /**
     * @brief 解析响应 JSON
     * @param response_body 响应体
     * @return Tushare 响应
     */
    TushareResponse parseResponse(const std::string& response_body);

    std::string api_token_;
    std::unique_ptr<HttpClient> http_client_;
};

} // namespace network

#endif // TUSHARE_CLIENT_H

