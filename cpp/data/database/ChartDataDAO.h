#ifndef DATA_CHART_DATA_DAO_H
#define DATA_CHART_DATA_DAO_H

#include <string>
#include <vector>
#include <optional>

namespace data {

struct ChartCandle {
    std::string trade_date;
    double open;
    double high;
    double low;
    double close;
    double volume;
    double ema17;
    double ema25;
};

struct ChartData {
    std::string ts_code;
    std::string freq;
    std::string analysis_date;
    std::vector<ChartCandle> candles;
};

class ChartDataDAO {
public:
    ChartDataDAO() = default;
    ~ChartDataDAO() = default;

    bool save(const ChartData& data);

    std::optional<ChartData> findByTsCodeAndFreq(
        const std::string& ts_code,
        const std::string& freq
    );

    bool remove(const std::string& ts_code, const std::string& freq);

private:
    std::string toJson(const std::vector<ChartCandle>& candles);
    std::vector<ChartCandle> fromJson(const std::string& json);
};

} // namespace data

#endif // DATA_CHART_DATA_DAO_H