#ifndef DATA_ANALYSIS_PROGRESS_TABLE_H
#define DATA_ANALYSIS_PROGRESS_TABLE_H

#include <sqlpp11/table.h>
#include <sqlpp11/data_types.h>
#include <sqlpp11/char_sequence.h>

namespace data {

namespace AnalysisProgressTable_ {
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

    struct Total {
        struct _alias_t {
            static constexpr const char _literal[] = "total";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T total;
                T& operator()() { return total; }
                const T& operator()() const { return total; }
            };
        };
        using _traits = sqlpp::make_traits<sqlpp::integer>;
    };

    struct Completed {
        struct _alias_t {
            static constexpr const char _literal[] = "completed";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T completed;
                T& operator()() { return completed; }
                const T& operator()() const { return completed; }
            };
        };
        using _traits = sqlpp::make_traits<sqlpp::integer>;
    };

    struct Failed {
        struct _alias_t {
            static constexpr const char _literal[] = "failed";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T failed;
                T& operator()() { return failed; }
                const T& operator()() const { return failed; }
            };
        };
        using _traits = sqlpp::make_traits<sqlpp::integer>;
    };

    struct Status {
        struct _alias_t {
            static constexpr const char _literal[] = "status";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T status;
                T& operator()() { return status; }
                const T& operator()() const { return status; }
            };
        };
        using _traits = sqlpp::make_traits<sqlpp::text>;
    };

    struct StartedAt {
        struct _alias_t {
            static constexpr const char _literal[] = "started_at";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T startedAt;
                T& operator()() { return startedAt; }
                const T& operator()() const { return startedAt; }
            };
        };
        using _traits = sqlpp::make_traits<sqlpp::text, sqlpp::tag::can_be_null>;
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

struct AnalysisProgressTable : sqlpp::table_t<AnalysisProgressTable,
    AnalysisProgressTable_::Id,
    AnalysisProgressTable_::Total,
    AnalysisProgressTable_::Completed,
    AnalysisProgressTable_::Failed,
    AnalysisProgressTable_::Status,
    AnalysisProgressTable_::StartedAt,
    AnalysisProgressTable_::UpdatedAt>
{
    struct _alias_t {
        static constexpr const char _literal[] = "analysis_progress";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t {
            T analysisProgress;
            T& operator()() { return analysisProgress; }
            const T& operator()() const { return analysisProgress; }
        };
    };
};

} // namespace data

#endif // DATA_ANALYSIS_PROGRESS_TABLE_H