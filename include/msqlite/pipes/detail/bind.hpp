#pragma once

#include "msqlite/detail/bind.hpp"

#include <tuple>
#include <utility>

namespace msqlite::detail {

template<typename... Binds>
struct bind_wrapper {
    template<typename... Binds_>
    bind_wrapper(Binds_&&... b)
        : binds(std::forward<Binds_>(b)...)
    {}
    std::tuple<Binds...> binds;
};

template<typename Context, typename... Binds>
auto pipe(Context&& ctx, detail::bind_wrapper<Binds...> o) {
    std::forward<Context>(ctx).conn().and_then(
        [&](auto&& db) {
            return std::forward<Context>(ctx).get_stmt().map(
                [&](auto&& stmt){
                    detail::bind_params(lvref(stmt).get(), std::move(o.binds));
                });
        });
    using ret = context<
        std::remove_reference_t<decltype(forward(*std::forward<Context>(ctx).conn()))>,
        void,
        std::remove_reference_t<decltype(forward(*std::forward<Context>(ctx).get_stmt()))> >;
        
    return ret(
        forward(*std::forward<Context>(ctx).conn()),
        forward(*std::forward<Context>(ctx).get_stmt())
        );
}

}
