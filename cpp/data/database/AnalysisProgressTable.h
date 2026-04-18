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

    struct Phase1Status {
        struct _alias_t {
            static constexpr const char _literal[] = "phase1_status";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T phase1Status;
                T& operator()() { return phase1Status; }
                const T& operator()() const { return phase1Status; }
            };
        };
        using _traits = sqlpp::make_traits<sqlpp::text>;
    };

    struct Phase1Total {
        struct _alias_t {
            static constexpr const char _literal[] = "phase1_total";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T phase1Total;
                T& operator()() { return phase1Total; }
                const T& operator()() const { return phase1Total; }
            };
        };
        using _traits = sqlpp::make_traits<sqlpp::integer>;
    };

    struct Phase1Completed {
        struct _alias_t {
            static constexpr const char _literal[] = "phase1_completed";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T phase1Completed;
                T& operator()() { return phase1Completed; }
                const T& operator()() const { return phase1Completed; }
            };
        };
        using _traits = sqlpp::make_traits<sqlpp::integer>;
    };

    struct Phase1Qualified {
        struct _alias_t {
            static constexpr const char _literal[] = "phase1_qualified";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T phase1Qualified;
                T& operator()() { return phase1Qualified; }
                const T& operator()() const { return phase1Qualified; }
            };
        };
        using _traits = sqlpp::make_traits<sqlpp::integer>;
    };

    struct Phase2Status {
        struct _alias_t {
            static constexpr const char _literal[] = "phase2_status";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T phase2Status;
                T& operator()() { return phase2Status; }
                const T& operator()() const { return phase2Status; }
            };
        };
        using _traits = sqlpp::make_traits<sqlpp::text>;
    };

    struct Phase2Total {
        struct _alias_t {
            static constexpr const char _literal[] = "phase2_total";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T phase2Total;
                T& operator()() { return phase2Total; }
                const T& operator()() const { return phase2Total; }
            };
        };
        using _traits = sqlpp::make_traits<sqlpp::integer>;
    };

    struct Phase2Completed {
        struct _alias_t {
            static constexpr const char _literal[] = "phase2_completed";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T phase2Completed;
                T& operator()() { return phase2Completed; }
                const T& operator()() const { return phase2Completed; }
            };
        };
        using _traits = sqlpp::make_traits<sqlpp::integer>;
    };

    struct Phase2Failed {
        struct _alias_t {
            static constexpr const char _literal[] = "phase2_failed";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T phase2Failed;
                T& operator()() { return phase2Failed; }
                const T& operator()() const { return phase2Failed; }
            };
        };
        using _traits = sqlpp::make_traits<sqlpp::integer>;
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

    struct Phase1CompletedAt {
        struct _alias_t {
            static constexpr const char _literal[] = "phase1_completed_at";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T phase1CompletedAt;
                T& operator()() { return phase1CompletedAt; }
                const T& operator()() const { return phase1CompletedAt; }
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
    AnalysisProgressTable_::Phase1Status,
    AnalysisProgressTable_::Phase1Total,
    AnalysisProgressTable_::Phase1Completed,
    AnalysisProgressTable_::Phase1Qualified,
    AnalysisProgressTable_::Phase2Status,
    AnalysisProgressTable_::Phase2Total,
    AnalysisProgressTable_::Phase2Completed,
    AnalysisProgressTable_::Phase2Failed,
    AnalysisProgressTable_::StartedAt,
    AnalysisProgressTable_::Phase1CompletedAt,
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