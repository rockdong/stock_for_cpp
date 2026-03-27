#ifndef DATA_ANALYSIS_PROCESS_RECORD_DAO_H
#define DATA_ANALYSIS_PROCESS_RECORD_DAO_H

#include <string>
#include <vector>
#include <optional>
#include "AnalysisProcessRecord.h"

namespace data {

class AnalysisProcessRecordDAO {
public:
    AnalysisProcessRecordDAO() = default;
    ~AnalysisProcessRecordDAO() = default;

    bool insert(const AnalysisProcessRecord& record);
    
    bool upsert(const AnalysisProcessRecord& record);

    std::optional<AnalysisProcessRecord> findById(int id);

    std::vector<AnalysisProcessRecord> findByTsCode(const std::string& ts_code);

    std::vector<AnalysisProcessRecord> findByStrategy(const std::string& strategy_name);

    std::vector<AnalysisProcessRecord> findBySignal(const std::string& signal);

    std::optional<AnalysisProcessRecord> findLatest(
        const std::string& ts_code,
        const std::string& strategy_name,
        const std::string& freq = "d"
    );

    std::vector<AnalysisProcessRecord> findAll(int limit = 100);

    bool remove(int id);

    int count();
};

} // namespace data

#endif // DATA_ANALYSIS_PROCESS_RECORD_DAO_H