#ifndef API_API_ROUTER_H
#define API_API_ROUTER_H

#include <string>
#include <vector>
#include <map>

namespace httplib { class Server; }

namespace api {

void setupRoutes(httplib::Server& server);

std::string jsonSuccess(const std::string& data);
std::string jsonError(const std::string& error);

std::string getStrategies();
std::string getStocks(int limit = 100);
std::string searchStocks(const std::string& keyword);
std::string getStockByCode(const std::string& code);
std::string getAnalysisSignals(const std::string& label, int limit = 50);
std::string getAnalysisProgress();
std::string getAnalysisProcess(const std::map<std::string, std::string>& params);
std::string getAnalysisProcessById(int id);
std::string getAnalysisProcessChart(const std::string& tsCode, const std::string& strategy, const std::string& freq);
std::string getAnalysisByCode(const std::string& code);
std::string getCharts(const std::string& code, const std::string& freq, int limit = 100);

} // namespace api

#endif // API_API_ROUTER_H