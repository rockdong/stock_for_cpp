#ifndef DATA_MONEY_FLOW_TABLE_H
#define DATA_MONEY_FLOW_TABLE_H

#include <sqlpp11/table.h>
#include <sqlpp11/data_types.h>
#include <sqlpp11/char_sequence.h>

namespace data {

namespace MoneyFlowTable_ {
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

    struct BuySmVol {
        struct _alias_t {
            static constexpr const char _literal[] = "buy_sm_vol";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T buySmVol;
                T& operator()() { return buySmVol; }
                const T& operator()() const { return buySmVol; }
            };
        };
        using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
    };

    struct BuySmAmount {
        struct _alias_t {
            static constexpr const char _literal[] = "buy_sm_amount";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T buySmAmount;
                T& operator()() { return buySmAmount; }
                const T& operator()() const { return buySmAmount; }
            };
        };
        using _traits = sqlpp::make_traits<sqlpp::floating_point, sqlpp::tag::can_be_null>;
    };

    struct SellSmVol {
        struct _alias_t {
            static constexpr const char _literal[] = "sell_sm_vol";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T sellSmVol;
                T& operator()() { return sellSmVol; }
                const T& operator()() const { return sellSmVol; }
            };
        };
        using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
    };

    struct SellSmAmount {
        struct _alias_t {
            static constexpr const char _literal[] = "sell_sm_amount";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T sellSmAmount;
                T& operator()() { return sellSmAmount; }
                const T& operator()() const { return sellSmAmount; }
            };
        };
        using _traits = sqlpp::make_traits<sqlpp::floating_point, sqlpp::tag::can_be_null>;
    };

    struct BuyMdVol {
        struct _alias_t {
            static constexpr const char _literal[] = "buy_md_vol";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T buyMdVol;
                T& operator()() { return buyMdVol; }
                const T& operator()() const { return buyMdVol; }
            };
        };
        using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
    };

    struct BuyMdAmount {
        struct _alias_t {
            static constexpr const char _literal[] = "buy_md_amount";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T buyMdAmount;
                T& operator()() { return buyMdAmount; }
                const T& operator()() const { return buyMdAmount; }
            };
        };
        using _traits = sqlpp::make_traits<sqlpp::floating_point, sqlpp::tag::can_be_null>;
    };

    struct SellMdVol {
        struct _alias_t {
            static constexpr const char _literal[] = "sell_md_vol";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T sellMdVol;
                T& operator()() { return sellMdVol; }
                const T& operator()() const { return sellMdVol; }
            };
        };
        using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
    };

    struct SellMdAmount {
        struct _alias_t {
            static constexpr const char _literal[] = "sell_md_amount";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T sellMdAmount;
                T& operator()() { return sellMdAmount; }
                const T& operator()() const { return sellMdAmount; }
            };
        };
        using _traits = sqlpp::make_traits<sqlpp::floating_point, sqlpp::tag::can_be_null>;
    };

    struct BuyLgVol {
        struct _alias_t {
            static constexpr const char _literal[] = "buy_lg_vol";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T buyLgVol;
                T& operator()() { return buyLgVol; }
                const T& operator()() const { return buyLgVol; }
            };
        };
        using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
    };

    struct BuyLgAmount {
        struct _alias_t {
            static constexpr const char _literal[] = "buy_lg_amount";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T buyLgAmount;
                T& operator()() { return buyLgAmount; }
                const T& operator()() const { return buyLgAmount; }
            };
        };
        using _traits = sqlpp::make_traits<sqlpp::floating_point, sqlpp::tag::can_be_null>;
    };

    struct SellLgVol {
        struct _alias_t {
            static constexpr const char _literal[] = "sell_lg_vol";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T sellLgVol;
                T& operator()() { return sellLgVol; }
                const T& operator()() const { return sellLgVol; }
            };
        };
        using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
    };

    struct SellLgAmount {
        struct _alias_t {
            static constexpr const char _literal[] = "sell_lg_amount";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T sellLgAmount;
                T& operator()() { return sellLgAmount; }
                const T& operator()() const { return sellLgAmount; }
            };
        };
        using _traits = sqlpp::make_traits<sqlpp::floating_point, sqlpp::tag::can_be_null>;
    };

    struct BuyElgVol {
        struct _alias_t {
            static constexpr const char _literal[] = "buy_elg_vol";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T buyElgVol;
                T& operator()() { return buyElgVol; }
                const T& operator()() const { return buyElgVol; }
            };
        };
        using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
    };

    struct BuyElgAmount {
        struct _alias_t {
            static constexpr const char _literal[] = "buy_elg_amount";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T buyElgAmount;
                T& operator()() { return buyElgAmount; }
                const T& operator()() const { return buyElgAmount; }
            };
        };
        using _traits = sqlpp::make_traits<sqlpp::floating_point, sqlpp::tag::can_be_null>;
    };

    struct SellElgVol {
        struct _alias_t {
            static constexpr const char _literal[] = "sell_elg_vol";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T sellElgVol;
                T& operator()() { return sellElgVol; }
                const T& operator()() const { return sellElgVol; }
            };
        };
        using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
    };

    struct SellElgAmount {
        struct _alias_t {
            static constexpr const char _literal[] = "sell_elg_amount";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T sellElgAmount;
                T& operator()() { return sellElgAmount; }
                const T& operator()() const { return sellElgAmount; }
            };
        };
        using _traits = sqlpp::make_traits<sqlpp::floating_point, sqlpp::tag::can_be_null>;
    };

    struct NetMfVol {
        struct _alias_t {
            static constexpr const char _literal[] = "net_mf_vol";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T netMfVol;
                T& operator()() { return netMfVol; }
                const T& operator()() const { return netMfVol; }
            };
        };
        using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
    };

    struct NetMfAmount {
        struct _alias_t {
            static constexpr const char _literal[] = "net_mf_amount";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
            struct _member_t {
                T netMfAmount;
                T& operator()() { return netMfAmount; }
                const T& operator()() const { return netMfAmount; }
            };
        };
        using _traits = sqlpp::make_traits<sqlpp::floating_point, sqlpp::tag::can_be_null>;
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
}

struct MoneyFlowTable : sqlpp::table_t<MoneyFlowTable,
    MoneyFlowTable_::Id,
    MoneyFlowTable_::TsCode,
    MoneyFlowTable_::TradeDate,
    MoneyFlowTable_::BuySmVol,
    MoneyFlowTable_::BuySmAmount,
    MoneyFlowTable_::SellSmVol,
    MoneyFlowTable_::SellSmAmount,
    MoneyFlowTable_::BuyMdVol,
    MoneyFlowTable_::BuyMdAmount,
    MoneyFlowTable_::SellMdVol,
    MoneyFlowTable_::SellMdAmount,
    MoneyFlowTable_::BuyLgVol,
    MoneyFlowTable_::BuyLgAmount,
    MoneyFlowTable_::SellLgVol,
    MoneyFlowTable_::SellLgAmount,
    MoneyFlowTable_::BuyElgVol,
    MoneyFlowTable_::BuyElgAmount,
    MoneyFlowTable_::SellElgVol,
    MoneyFlowTable_::SellElgAmount,
    MoneyFlowTable_::NetMfVol,
    MoneyFlowTable_::NetMfAmount,
    MoneyFlowTable_::CreatedAt>
{
    struct _alias_t {
        static constexpr const char _literal[] = "money_flow";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t {
            T moneyFlow;
            T& operator()() { return moneyFlow; }
            const T& operator()() const { return moneyFlow; }
        };
    };
};

} // namespace data

#endif // DATA_MONEY_FLOW_TABLE_H