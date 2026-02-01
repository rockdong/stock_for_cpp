#include "Data.h"
#include "IDataSource.h"  // 包含 network::StockBasic 定义

namespace data {

Stock convertFromStockBasic(const network::StockBasic& stockBasic) {
    Stock stock;
    stock.ts_code = stockBasic.ts_code;
    stock.symbol = stockBasic.symbol;
    stock.name = stockBasic.name;
    stock.area = stockBasic.area;
    stock.industry = stockBasic.industry;
    stock.fullname = stockBasic.fullname;
    stock.enname = stockBasic.enname;
    stock.cnspell = stockBasic.cnspell;
    stock.market = stockBasic.market;
    stock.exchange = stockBasic.exchange;
    stock.curr_type = stockBasic.curr_type;
    stock.list_status = stockBasic.list_status;
    stock.list_date = stockBasic.list_date;
    stock.delist_date = stockBasic.delist_date;
    stock.is_hs = stockBasic.is_hs;
    return stock;
}

std::vector<Stock> convertFromStockBasics(const std::vector<network::StockBasic>& stockBasics) {
    std::vector<Stock> stocks;
    stocks.reserve(stockBasics.size());
    
    for (const auto& stockBasic : stockBasics) {
        stocks.push_back(convertFromStockBasic(stockBasic));
    }
    
    return stocks;
}

} // namespace data

