#pragma once

#include "msqlite/detail/exec.hpp"

#include <tuple>
#include <utility>

namespace msqlite::detail {

template<typename... Binds>
struct exec_wrapper {
    template<typename... Binds_>
    exec_wrapper(std::string_view s, Binds_&&... b)
        : stmt(s)
        , binds(std::forward<Binds_>(b)...)
    {}
    std::string_view stmt;
    std::tuple<Binds...> binds;
};

template<typename... Binds>
struct exec_with_wrapper {
    template<typename... Binds_>
    exec_with_wrapper(Binds_&&... b)
        : binds(std::forward<Binds_>(b)...)
    {}
    std::tuple<Binds...> binds;
};

template<typename ResultDb, typename... Binds>
auto pipe(ResultDb&& res, detail::exec_wrapper<Binds...> o) {
    return std::forward<ResultDb>(res).and_then(
        [&](auto&& db) {
            return detail::exec(lvref(db), o.stmt, std::move(o.binds)).map(
                [&]{ return forward(std::forward<decltype(db)>(db)); });
        });
}

template<typename Context, typename... Binds>
auto pipe(Context&& ctx, detail::exec_with_wrapper<Binds...> o) {
    auto r = std::forward<Context>(ctx).conn().and_then(
        [&](auto&& db) {
            return std::forward<Context>(ctx).get_stmt().and_then(
                [&](auto&& stmt){
                    return detail::exec(lvref(stmt), std::move(o.binds));
                });
        });
    using ret = context<
        std::remove_reference_t<decltype(forward(*std::forward<Context>(ctx).conn()))>,
        result<void>,
        std::remove_reference_t<decltype(forward(*std::forward<Context>(ctx).get_stmt()))> >;
        
    return ret(
        forward(*std::forward<Context>(ctx).conn()),
        forward(*std::forward<Context>(ctx).get_stmt()),
        std::move(r)
        );
}

}
