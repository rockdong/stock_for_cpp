#ifndef CORE_FUNDAMENTAL_DATA_H
#define CORE_FUNDAMENTAL_DATA_H

#include <string>
#include <map>
#include <vector>

namespace core {

/**
 * @brief 财务指标数据结构
 * 
 * 从 Tushare fina_indicator API 获取的财务指标数据
 */
struct FinancialIndicator {
    std::string ts_code;            // 股票代码
    std::string ann_date;           // 公告日期
    std::string end_date;           // 报告期
    
    // 估值指标
    double pe = 0.0;                // 市盈率
    double pe_ttm = 0.0;            // 市盈率TTM
    double pb = 0.0;                // 市净率
    double ps = 0.0;                // 市销率
    double pcf = 0.0;               // 市现率
    
    // 盈利能力指标
    double roe = 0.0;               // 净资产收益率
    double roe_ttm = 0.0;           // ROE TTM
    double roa = 0.0;               // 总资产收益率
    double grossprofit_margin = 0.0;// 毛利率
    double netprofit_margin = 0.0;  // 净利率
    double operating_margin = 0.0;  // 营业利润率
    
    // 增长能力指标
    double or_yoy = 0.0;            // 营业收入同比增长
    double op_yoy = 0.0;            // 营业利润同比增长
    double ebt_yoy = 0.0;           // 利润总额同比增长
    double netprofit_yoy = 0.0;     // 净利润同比增长
    double tr_yoy = 0.0;            // 营业总收入同比增长
    
    // 健康指标
    double debt_to_assets = 0.0;    // 资产负债率
    double assets_turn = 0.0;       // 总资产周转率
    double current_ratio = 0.0;     // 流动比率
    
    // 现金流指标
    double ocf_to_operatingprofit = 0.0; // 经营现金流/营业利润
    double ocf_to_or = 0.0;         // 经营现金流/营业收入
};

/**
 * @brief 资金流向数据
 * 
 * 来自 Tushare moneyflow 接口
 * 小单：<5万；中单：5万~20万；大单：20万~100万；特大单：>=100万
 */
struct MoneyFlow {
    std::string ts_code;           // TS股票代码
    std::string trade_date;        // 交易日期
    
    // 小单（<5万）
    int buy_sm_vol = 0;            // 小单买入量（手）
    double buy_sm_amount = 0.0;    // 小单买入金额（万元）
    int sell_sm_vol = 0;           // 小单卖出量（手）
    double sell_sm_amount = 0.0;   // 小单卖出金额（万元）
    
    // 中单（5万~20万）
    int buy_md_vol = 0;            // 中单买入量（手）
    double buy_md_amount = 0.0;    // 中单买入金额（万元）
    int sell_md_vol = 0;           // 中单卖出量（手）
    double sell_md_amount = 0.0;   // 中单卖出金额（万元）
    
    // 大单（20万~100万）
    int buy_lg_vol = 0;            // 大单买入量（手）
    double buy_lg_amount = 0.0;    // 大单买入金额（万元）
    int sell_lg_vol = 0;           // 大单卖出量（手）
    double sell_lg_amount = 0.0;   // 大单卖出金额（万元）
    
    // 特大单（>=100万）
    int buy_elg_vol = 0;           // 特大单买入量（手）
    double buy_elg_amount = 0.0;   // 特大单买入金额（万元）
    int sell_elg_vol = 0;          // 特大单卖出量（手）
    double sell_elg_amount = 0.0;  // 特大单卖出金额（万元）
    
    // 净流入
    int net_mf_vol = 0;            // 净流入量（手）
    double net_mf_amount = 0.0;    // 净流入额（万元）
};

/**
 * @brief 基本面评分结果
 * 
 * 基于财务指标计算的4维度综合评分
 */
struct FundamentalScore {
    std::string ts_code;            // 股票代码
    
    double total = 0.0;             // 总分 (0-100)
    double valuation = 0.0;         // 估值得分 (0-25)
    double quality = 0.0;           // 盈利能力得分 (0-35)
    double growth = 0.0;            // 增长能力得分 (0-25)
    double health = 0.0;            // 财务健康得分 (0-15)
    
    std::string grade;              // 评级: A/B/C/D
    bool is_qualified = false;      // 是否通过筛选
    std::string reason;             // 评分原因说明
};

/**
 * @brief 筛选阈值配置
 * 
 * 用于基本面筛选的条件阈值
 */
struct FilterThresholds {
    // 估值阈值
    double pe_max = 30.0;           // PE上限
    double pb_max = 5.0;            // PB上限
    double peg_max = 1.5;           // PEG上限
    
    // 盈利能力阈值
    double roe_min = 10.0;          // ROE最低
    double gross_margin_min = 20.0; // 毛利率最低
    double net_margin_min = 10.0;   // 净利率最低
    
    // 增长能力阈值
    double revenue_growth_min = 5.0;// 营收增长最低
    double profit_growth_min = 5.0; // 利润增长最低
    
    // 财务健康阈值
    double debt_ratio_max = 60.0;   // 资产负债率上限
    double current_ratio_min = 1.0; // 流动比率最低
    
    // 总分阈值
    double total_score_min = 60.0;  // 总分最低要求
    
    /**
     * @brief 从环境变量/配置加载阈值
     */
    void loadFromConfig(const std::map<std::string, std::string>& config);
};

/**
 * @brief 优质股票池结果
 */
struct QualifiedPool {
    std::string update_date;        // 更新日期
    int total_count = 0;            // 优质股票总数
    std::vector<std::string> stocks; // 优质股票代码列表
    std::map<std::string, FundamentalScore> scores; // 各股票评分详情
};

} // namespace core

#endif // CORE_FUNDAMENTAL_DATA_H