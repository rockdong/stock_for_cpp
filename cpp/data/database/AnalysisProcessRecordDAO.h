#ifndef DATA_STOCK_PROCESS_RECORD_DAO_H
#define DATA_STOCK_PROCESS_RECORD_DAO_H

#include <string>
#include <vector>
#include <optional>
#include "AnalysisProcessRecord.h"

namespace data {

struct PhaseProgress {
    std::string status;
    int total;
    int completed;
    int qualified;
    int failed;
};

struct AnalysisProgress {
    PhaseProgress phase1;
    PhaseProgress phase2;
    std::string started_at;
    std::string phase1_completed_at;
    std::string updated_at;
};

class StockProcessRecordDAO {
public:
    StockProcessRecordDAO() = default;
    ~StockProcessRecordDAO() = default;

    bool upsert(const StockProcessRecord& record);

    std::optional<StockProcessRecord> findByTsCode(
        const std::string& ts_code,
        const std::string& trade_date = ""
    );

    std::vector<StockProcessRecord> findAll(int limit = 100);

    std::vector<StockProcessRecord> findByDateRange(
        const std::string& start_date,
        const std::string& end_date
    );

    bool remove(const std::string& ts_code);

    int count();
    
    AnalysisProgress getProgress() const;
};

using AnalysisProcessRecordDAO = StockProcessRecordDAO;

} // namespace data

#endif // DATA_STOCK_PROCESS_RECORD_DAO_H