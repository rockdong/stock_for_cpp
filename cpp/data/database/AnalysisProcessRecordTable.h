#ifndef DATA_ANALYSIS_PROCESS_RECORD_TABLE_H
#define DATA_ANALYSIS_PROCESS_RECORD_TABLE_H

#include <sqlpp11/table.h>
#include <sqlpp11/data_types.h>
#include <sqlpp11/char_sequence.h>

namespace data {

namespace StockProcessRecordTable_ {
    struct Id {
        struct _alias_t {
            static constexpr const char _literal[] = "id";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T id;
                T& operator()() { return id; }
                const T& operator()() const { return id; }
            };
        };
        using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::must_not_insert, sqlpp::tag::must_not_update>;
    };

    struct TsCode {
        struct _alias_t {
            static constexpr const char _literal[] = "ts_code";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T tsCode;
                T& operator()() { return tsCode; }
                const T& operator()() const { return tsCode; }
            };
        };
        using _traits = sqlpp::make_traits<sqlpp::text, sqlpp::tag::require_insert>;
    };

    struct StockName {
        struct _alias_t {
            static constexpr const char _literal[] = "stock_name";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T stockName;
                T& operator()() { return stockName; }
                const T& operator()() const { return stockName; }
            };
        };
        using _traits = sqlpp::make_traits<sqlpp::text, sqlpp::tag::can_be_null>;
    };

    struct TradeDate {
        struct _alias_t {
            static constexpr const char _literal[] = "trade_date";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T tradeDate;
                T& operator()() { return tradeDate; }
                const T& operator()() const { return tradeDate; }
            };
        };
        using _traits = sqlpp::make_traits<sqlpp::text, sqlpp::tag::require_insert>;
    };

    struct Data {
        struct _alias_t {
            static constexpr const char _literal[] = "data";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T data;
                T& operator()() { return data; }
                const T& operator()() const { return data; }
            };
        };
        using _traits = sqlpp::make_traits<sqlpp::text, sqlpp::tag::require_insert>;
    };

    struct CreatedAt {
        struct _alias_t {
            static constexpr const char _literal[] = "created_at";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T createdAt;
                T& operator()() { return createdAt; }
                const T& operator()() const { return createdAt; }
            };
        };
        using _traits = sqlpp::make_traits<sqlpp::text, sqlpp::tag::must_not_insert, sqlpp::tag::must_not_update>;
    };

    struct ExpiresAt {
        struct _alias_t {
            static constexpr const char _literal[] = "expires_at";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T expiresAt;
                T& operator()() { return expiresAt; }
                const T& operator()() const { return expiresAt; }
            };
        };
        using _traits = sqlpp::make_traits<sqlpp::text, sqlpp::tag::can_be_null>;
    };
}

struct StockProcessRecordTable : sqlpp::table_t<StockProcessRecordTable,
    StockProcessRecordTable_::Id,
    StockProcessRecordTable_::TsCode,
    StockProcessRecordTable_::StockName,
    StockProcessRecordTable_::TradeDate,
    StockProcessRecordTable_::Data,
    StockProcessRecordTable_::CreatedAt,
    StockProcessRecordTable_::ExpiresAt>
{
    struct _alias_t {
        static constexpr const char _literal[] = "analysis_process_records";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t {
            T analysisProcessRecords;
            T& operator()() { return analysisProcessRecords; }
            const T& operator()() const { return analysisProcessRecords; }
        };
    };
};

using AnalysisProcessRecordTable = StockProcessRecordTable;

} // namespace data

#endif // DATA_ANALYSIS_PROCESS_RECORD_TABLE_H