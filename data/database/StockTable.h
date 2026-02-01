#ifndef DATA_STOCK_TABLE_H
#define DATA_STOCK_TABLE_H

#include <sqlpp11/table.h>
#include <sqlpp11/data_types.h>
#include <sqlpp11/char_sequence.h>

namespace data {

namespace StockTable_ {
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

    struct Symbol {
        struct _alias_t {
            static constexpr const char _literal[] = "symbol";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T symbol;
                T& operator()() { return symbol; }
                const T& operator()() const { return symbol; }
            };
        };
        using _traits = sqlpp::make_traits<sqlpp::text, sqlpp::tag::require_insert>;
    };

    struct Name {
        struct _alias_t {
            static constexpr const char _literal[] = "name";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T name;
                T& operator()() { return name; }
                const T& operator()() const { return name; }
            };
        };
        using _traits = sqlpp::make_traits<sqlpp::text, sqlpp::tag::require_insert>;
    };

    struct Area {
        struct _alias_t {
            static constexpr const char _literal[] = "area";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T area;
                T& operator()() { return area; }
                const T& operator()() const { return area; }
            };
        };
        using _traits = sqlpp::make_traits<sqlpp::text, sqlpp::tag::can_be_null>;
    };

    struct Industry {
        struct _alias_t {
            static constexpr const char _literal[] = "industry";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T industry;
                T& operator()() { return industry; }
                const T& operator()() const { return industry; }
            };
        };
        using _traits = sqlpp::make_traits<sqlpp::text, sqlpp::tag::can_be_null>;
    };

    struct Market {
        struct _alias_t {
            static constexpr const char _literal[] = "market";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T market;
                T& operator()() { return market; }
                const T& operator()() const { return market; }
            };
        };
        using _traits = sqlpp::make_traits<sqlpp::text, sqlpp::tag::can_be_null>;
    };

    struct ListDate {
        struct _alias_t {
            static constexpr const char _literal[] = "list_date";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T listDate;
                T& operator()() { return listDate; }
                const T& operator()() const { return listDate; }
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

struct StockTable : sqlpp::table_t<StockTable,
    StockTable_::Id,
    StockTable_::TsCode,
    StockTable_::Symbol,
    StockTable_::Name,
    StockTable_::Area,
    StockTable_::Industry,
    StockTable_::Market,
    StockTable_::ListDate,
    StockTable_::CreatedAt,
    StockTable_::UpdatedAt>
{
    struct _alias_t {
        static constexpr const char _literal[] = "stocks";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t {
            T stocks;
            T& operator()() { return stocks; }
            const T& operator()() const { return stocks; }
        };
    };
};

} // namespace data

#endif // DATA_STOCK_TABLE_H

