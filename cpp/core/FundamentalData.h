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

/**
 * @brief 停复牌信息
 * 
 * 来自 Tushare suspend_d 接口
 * 记录股票停牌和复牌的时间和类型
 */
struct SuspendInfo {
    std::string ts_code;            // TS股票代码
    std::string suspend_date;       // 停牌日期
    std::string resume_date;        // 复牌日期
    std::string suspend_type;       // 停牌类型 (S-停牌, R-复牌)
    std::string suspend_reason;     // 停牌原因（可选）
};

/**
 * @brief 业绩预告
 * 
 * 来自 Tushare forecast 接口
 * 记录上市公司业绩预告数据
 */
struct Forecast {
    std::string ts_code;            // TS股票代码
    std::string ann_date;           // 公告日期
    std::string end_date;           // 报告期
    std::string type;               // 预告类型 (1-业绩预告, 2-业绩快报等)
    double p_change_min = 0.0;      // 预告净利润变动幅度下限 (%)
    double p_change_max = 0.0;      // 预告净利润变动幅度上限 (%)
    double net_profit_min = 0.0;    // 预告净利润下限 (万元)
    double net_profit_max = 0.0;    // 预告净利润上限 (万元)
    std::string summary;            // 预告摘要
    std::string change_reason;      // 变动原因
};

/**
 * @brief 业绩快报
 * 
 * 来自 Tushare express 接口
 * 记录上市公司业绩快报数据
 */
struct Express {
    std::string ts_code;            // TS股票代码
    std::string ann_date;           // 公告日期
    std::string end_date;           // 报告期
    double revenue = 0.0;           // 营业收入 (万元)
    double operate_profit = 0.0;    // 营业利润 (万元)
    double total_profit = 0.0;      // 利润总额 (万元)
    double n_income = 0.0;          // 净利润 (万元)
    double total_assets = 0.0;      // 总资产 (万元)
    double total_hldr_eqy_exc_min_int = 0.0; // 股东权益合计 (万元)
    double total_share = 0.0;       // 总股本 (万股)
    double eps = 0.0;               // 每股收益
};

/**
 * @brief 分红送股
 * 
 * 来自 Tushare dividend 接口
 * 记录上市公司分红送股数据
 */
struct Dividend {
    std::string ts_code;            // TS股票代码
    std::string ann_date;           // 公告日期
    std::string end_date;           // 报告期/年度
    std::string div_proc;           // 实施进度
    double stk_div = 0.0;           // 送股比例 (每股送股)
    double stk_bo_rate = 0.0;       // 转增比例 (每股转增)
    double cash_div = 0.0;          // 每股现金分红 (元)
    double cash_div_tax = 0.0;      // 每股现金分红税后 (元)
    std::string record_date;        // 公告日期
    std::string ex_date;            // 除权除息日
    std::string pay_date;           // 派息日
    std::string div_listdate;       // 红股上市日
};

/**
 * @brief 涨停板个股数据
 * 
 * 来自 Tushare limit_list_d 接口
 * 记录每日涨停/跌停股票详情
 * 用于暴涨预警：追踪近期涨停股票
 */
struct LimitListStock {
    std::string ts_code;            // TS股票代码
    std::string trade_date;         // 交易日期
    std::string name;               // 股票名称
    double close = 0.0;             // 收盘价
    double pct_chg = 0.0;           // 涨跌幅 (%)
    double amp = 0.0;               // 振幅 (%)
    double fc_ratio = 0.0;          // 封板时间比（涨停封住时间占比）
    double fl_ratio = 0.0;          // 第一次涨停时间比
    double fd_amount = 0.0;         // 封板金额（万元）
    double first_time = 0.0;        // 第一次涨停时间
    double last_time = 0.0;         // 最后一次涨停时间
    int limit_times = 0;            // 当日涨停次数
    std::string up_stat;            // 涨跌停状态（U-涨停，D-跌停）
    std::string limit;              // 涨跌停类型（1-首板，2-二板，3-三板等）
};

/**
 * @brief 板块热度数据
 * 
 * 来自 Tushare limit_cpt_list 接口
 * 记录涨停板块排行数据
 * 用于暴涨预警：判断所属板块热度
 */
struct SectorHeat {
    std::string trade_date;         // 交易日期
    std::string sector_name;        // 板块名称（概念板块）
    std::string sector_code;        // 板块代码
    int limit_count = 0;            // 板块内涨停股票数
    int total_count = 0;            // 板块内股票总数
    double limit_ratio = 0.0;       // 涨停比例 (%)
    double avg_pct_chg = 0.0;       // 板块平均涨跌幅 (%)
    double avg_amount = 0.0;        // 板块平均成交额（万元）
    std::string top_stock;          // 板块领涨股票代码
    std::string top_stock_name;     // 板块领涨股票名称
};

/**
 * @brief 市场热度数据
 * 
 * 整合涨停板和板块热度的市场整体数据
 * 用于暴涨预警策略的上下文数据
 */
struct MarketHeatData {
    std::string trade_date;         // 交易日期
    
    // 涨停板数据
    std::vector<LimitListStock> limit_up_stocks;   // 涨停股票列表
    int limit_up_count = 0;                        // 涨停股票总数
    
    // 板块热度数据
    std::vector<SectorHeat> hot_sectors;           // 热门板块列表（按涨停数排序）
    
    // 快速查询索引
    std::map<std::string, int> stock_limit_count;  // 各股票近期涨停次数
    std::map<std::string, SectorHeat> stock_sector; // 各股票所属热门板块
    
    /**
     * @brief 获取指定股票近期涨停次数
     * @param ts_code 股票代码
     * @return 近5日涨停次数
     */
    int getStockLimitCount(const std::string& ts_code) const {
        auto it = stock_limit_count.find(ts_code);
        return it != stock_limit_count.end() ? it->second : 0;
    }
    
    /**
     * @brief 获取指定股票所属热门板块
     * @param ts_code 股票代码
     * @return 所属板块热度数据（如果不在热门板块返回空）
     */
    SectorHeat getStockSector(const std::string& ts_code) const {
        auto it = stock_sector.find(ts_code);
        return it != stock_sector.end() ? it->second : SectorHeat();
    }
    
    /**
     * @brief 判断股票是否在热门板块
     * @param ts_code 股票代码
     * @return 是否在热门板块（板块涨停数>=3）
     */
    bool isInHotSector(const std::string& ts_code) const {
        auto sector = getStockSector(ts_code);
        return sector.limit_count >= 3;
    }
    
    /**
     * @brief 判断股票是否近期有涨停
     * @param ts_code 股票代码
     * @return 近5日是否涨停
     */
    bool hasRecentLimitUp(const std::string& ts_code) const {
        return getStockLimitCount(ts_code) > 0;
    }
};

} // namespace core

#endif // CORE_FUNDAMENTAL_DATA_H