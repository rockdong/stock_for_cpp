#ifndef DATA_PRICE_TABLE_H
#define DATA_PRICE_TABLE_H

#include <sqlpp11/table.h>
#include <sqlpp11/data_types.h>
#include <sqlpp11/char_sequence.h>

namespace data {

namespace PriceTable_ {
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

    struct StockId {
        struct _alias_t {
            static constexpr const char _literal[] = "stock_id";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T stockId;
                T& operator()() { return stockId; }
                const T& operator()() const { return stockId; }
            };
        };
        using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::require_insert>;
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

    struct Open {
        struct _alias_t {
            static constexpr const char _literal[] = "open";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T open;
                T& operator()() { return open; }
                const T& operator()() const { return open; }
            };
        };
        using _traits = sqlpp::make_traits<sqlpp::floating_point, sqlpp::tag::can_be_null>;
    };

    struct High {
        struct _alias_t {
            static constexpr const char _literal[] = "high";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T high;
                T& operator()() { return high; }
                const T& operator()() const { return high; }
            };
        };
        using _traits = sqlpp::make_traits<sqlpp::floating_point, sqlpp::tag::can_be_null>;
    };

    struct Low {
        struct _alias_t {
            static constexpr const char _literal[] = "low";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T low;
                T& operator()() { return low; }
                const T& operator()() const { return low; }
            };
        };
        using _traits = sqlpp::make_traits<sqlpp::floating_point, sqlpp::tag::can_be_null>;
    };

    struct Close {
        struct _alias_t {
            static constexpr const char _literal[] = "close";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T close;
                T& operator()() { return close; }
                const T& operator()() const { return close; }
            };
        };
        using _traits = sqlpp::make_traits<sqlpp::floating_point, sqlpp::tag::can_be_null>;
    };

    struct PreClose {
        struct _alias_t {
            static constexpr const char _literal[] = "pre_close";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T preClose;
                T& operator()() { return preClose; }
                const T& operator()() const { return preClose; }
            };
        };
        using _traits = sqlpp::make_traits<sqlpp::floating_point, sqlpp::tag::can_be_null>;
    };

    struct ChangePct {
        struct _alias_t {
            static constexpr const char _literal[] = "change_pct";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T changePct;
                T& operator()() { return changePct; }
                const T& operator()() const { return changePct; }
            };
        };
        using _traits = sqlpp::make_traits<sqlpp::floating_point, sqlpp::tag::can_be_null>;
    };

    struct Volume {
        struct _alias_t {
            static constexpr const char _literal[] = "volume";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T volume;
                T& operator()() { return volume; }
                const T& operator()() const { return volume; }
            };
        };
        using _traits = sqlpp::make_traits<sqlpp::floating_point, sqlpp::tag::can_be_null>;
    };

    struct Amount {
        struct _alias_t {
            static constexpr const char _literal[] = "amount";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T amount;
                T& operator()() { return amount; }
                const T& operator()() const { return amount; }
            };
        };
        using _traits = sqlpp::make_traits<sqlpp::floating_point, sqlpp::tag::can_be_null>;
    };
}

struct PriceTable : sqlpp::table_t<PriceTable,
    PriceTable_::Id,
    PriceTable_::StockId,
    PriceTable_::TradeDate,
    PriceTable_::Open,
    PriceTable_::High,
    PriceTable_::Low,
    PriceTable_::Close,
    PriceTable_::PreClose,
    PriceTable_::ChangePct,
    PriceTable_::Volume,
    PriceTable_::Amount>
{
    struct _alias_t {
        static constexpr const char _literal[] = "prices";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t {
            T prices;
            T& operator()() { return prices; }
            const T& operator()() const { return prices; }
        };
    };
};

} // namespace data

#endif // DATA_PRICE_TABLE_H

