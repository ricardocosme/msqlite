#pragma once

#include "msqlite/detail/for_each.hpp"

namespace msqlite::detail {

template<typename F>
struct for_each_wrapper : F {
    using base = F;
    template<typename F_>
    for_each_wrapper(F_ f) : base(std::forward<F_>(f)) {}
};

template<typename Context,
         typename F>
inline auto pipe(Context&& ctx, detail::for_each_wrapper<F> o)
{
    return std::forward<Context>(ctx).conn().and_then(
        [&](auto&& conn) {
            return std::forward<Context>(ctx).get_stmt().and_then(
                [&](auto&& stmt){
                    return ::msqlite::detail::for_each(lvref(stmt), std::move(o)).map(
                        [&]{ return forward(std::forward<decltype(conn)>(conn)); });
                });
        });
}

}
