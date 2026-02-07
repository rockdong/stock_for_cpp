#ifndef DATA_ANALYSIS_RESULT_DAO_H
#define DATA_ANALYSIS_RESULT_DAO_H

#include <string>
#include <vector>
#include <optional>
#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/sqlite3/sqlite3.h>
#include "AnalysisResultTable.h"
#include "../../core/AnalysisResult.h"

namespace data {

// 使用核心模块的数据结构
using AnalysisResult = core::AnalysisResult;
using AnalysisResultEntity = core::AnalysisResultEntity;

/**
 * @brief 分析结果数据访问对象
 * 职责：提供分析结果数据的 CRUD 操作
 */
class AnalysisResultDAO {
public:
    AnalysisResultDAO() = default;
    ~AnalysisResultDAO() = default;

    /**
     * @brief 插入分析结果
     * @param result 分析结果对象
     * @return 是否成功
     */
    bool insert(const AnalysisResult& result);

    /**
     * @brief 批量插入分析结果
     * @param results 分析结果列表
     * @return 成功插入的数量
     */
    int batchInsert(const std::vector<AnalysisResult>& results);

    /**
     * @brief 根据 ID 查找
     * @param id 分析结果 ID
     * @return 分析结果对象（可选）
     */
    std::optional<AnalysisResult> findById(int id);

    /**
     * @brief 查找所有分析结果
     * @return 分析结果列表
     */
    std::vector<AnalysisResult> findAll();

    /**
     * @brief 根据股票代码查找
     * @param ts_code 股票代码
     * @return 分析结果列表
     */
    std::vector<AnalysisResult> findByTsCode(const std::string& ts_code);

    /**
     * @brief 根据策略名称查找
     * @param strategy_name 策略名称
     * @return 分析结果列表
     */
    std::vector<AnalysisResult> findByStrategy(const std::string& strategy_name);

    /**
     * @brief 根据标签查找
     * @param label 标签
     * @return 分析结果列表
     */
    std::vector<AnalysisResult> findByLabel(const std::string& label);

    /**
     * @brief 根据股票代码和策略名称查找
     * @param ts_code 股票代码
     * @param strategy_name 策略名称
     * @return 分析结果列表
     */
    std::vector<AnalysisResult> findByTsCodeAndStrategy(
        const std::string& ts_code,
        const std::string& strategy_name
    );

    /**
     * @brief 根据日期范围查找
     * @param start_date 开始日期（格式：YYYYMMDD）
     * @param end_date 结束日期（格式：YYYYMMDD）
     * @return 分析结果列表
     */
    std::vector<AnalysisResult> findByDateRange(
        const std::string& start_date,
        const std::string& end_date
    );

    /**
     * @brief 更新分析结果
     * @param id 分析结果 ID
     * @param result 分析结果对象
     * @return 是否成功
     */
    bool update(int id, const AnalysisResult& result);

    /**
     * @brief 删除分析结果
     * @param id 分析结果 ID
     * @return 是否成功
     */
    bool remove(int id);

    /**
     * @brief 删除指定股票的所有分析结果
     * @param ts_code 股票代码
     * @return 是否成功
     */
    bool removeByTsCode(const std::string& ts_code);

    /**
     * @brief 删除指定策略的所有分析结果
     * @param strategy_name 策略名称
     * @return 是否成功
     */
    bool removeByStrategy(const std::string& strategy_name);

    /**
     * @brief 获取分析结果总数
     * @return 分析结果数量
     */
    int count();

    /**
     * @brief 获取指定股票的分析结果数量
     * @param ts_code 股票代码
     * @return 分析结果数量
     */
    int countByTsCode(const std::string& ts_code);
};

} // namespace data

#endif // DATA_ANALYSIS_RESULT_DAO_H

