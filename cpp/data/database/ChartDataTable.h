#ifndef DATA_CHART_DATA_TABLE_H
#define DATA_CHART_DATA_TABLE_H

#include <sqlpp11/table.h>
#include <sqlpp11/data_types.h>
#include <sqlpp11/char_sequence.h>

namespace data {

namespace ChartDataTable_ {
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
        using _traits = sqlpp::make_traits<sqlpp::text, sqlpp::tag::require_insert>;
    };

    struct AnalysisDate {
        struct _alias_t {
            static constexpr const char _literal[] = "analysis_date";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T analysisDate;
                T& operator()() { return analysisDate; }
                const T& operator()() const { return analysisDate; }
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

struct ChartDataTable : sqlpp::table_t<ChartDataTable,
    ChartDataTable_::Id,
    ChartDataTable_::TsCode,
    ChartDataTable_::Freq,
    ChartDataTable_::AnalysisDate,
    ChartDataTable_::Data,
    ChartDataTable_::UpdatedAt>
{
    struct _alias_t {
        static constexpr const char _literal[] = "chart_data";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t {
            T chartData;
            T& operator()() { return chartData; }
            const T& operator()() const { return chartData; }
        };
    };
};

} // namespace data

#endif // DATA_CHART_DATA_TABLE_H