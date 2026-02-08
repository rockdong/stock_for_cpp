#ifndef DATA_ANALYSIS_RESULT_TABLE_H
#define DATA_ANALYSIS_RESULT_TABLE_H

#include <sqlpp11/table.h>
#include <sqlpp11/data_types.h>
#include <sqlpp11/char_sequence.h>

namespace data {

namespace AnalysisResultTable_ {
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

    struct StrategyName {
        struct _alias_t {
            static constexpr const char _literal[] = "strategy_name";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T strategyName;
                T& operator()() { return strategyName; }
                const T& operator()() const { return strategyName; }
            };
        };
        using _traits = sqlpp::make_traits<sqlpp::text, sqlpp::tag::require_insert>;
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

    struct Label {
        struct _alias_t {
            static constexpr const char _literal[] = "label";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T label;
                T& operator()() { return label; }
                const T& operator()() const { return label; }
            };
        };
        using _traits = sqlpp::make_traits<sqlpp::text, sqlpp::tag::require_insert>;
    };

    struct Opt {
        struct _alias_t {
            static constexpr const char _literal[] = "opt";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T opt;
                T& operator()() { return opt; }
                const T& operator()() const { return opt; }
            };
        };
        using _traits = sqlpp::make_traits<sqlpp::text, sqlpp::tag::can_be_null>;
    };

    struct Freq {
        struct _alias_t {
            static constexpr const char _literal[] = "freq";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T freq;
                T& operator()() { return freq; }
                const T& operator()() const { return freq; }
            };
        };
        using _traits = sqlpp::make_traits<sqlpp::text, sqlpp::tag::can_be_null>;
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

    struct UpdatedAt {
        struct _alias_t {
            static constexpr const char _literal[] = "updated_at";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T updatedAt;
                T& operator()() { return updatedAt; }
                const T& operator()() const { return updatedAt; }
            };
        };
        using _traits = sqlpp::make_traits<sqlpp::text, sqlpp::tag::must_not_insert, sqlpp::tag::must_not_update>;
    };
}

struct AnalysisResultTable : sqlpp::table_t<AnalysisResultTable,
    AnalysisResultTable_::Id,
    AnalysisResultTable_::TsCode,
    AnalysisResultTable_::StrategyName,
    AnalysisResultTable_::TradeDate,
    AnalysisResultTable_::Label,
    AnalysisResultTable_::Opt,
    AnalysisResultTable_::Freq,
    AnalysisResultTable_::CreatedAt,
    AnalysisResultTable_::UpdatedAt>
{
    struct _alias_t {
        static constexpr const char _literal[] = "analysis_results";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t {
            T analysisResults;
            T& operator()() { return analysisResults; }
            const T& operator()() const { return analysisResults; }
        };
    };
};

} // namespace data

#endif // DATA_ANALYSIS_RESULT_TABLE_H

