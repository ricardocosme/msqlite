#pragma once

#include "msqlite/detail/step.hpp"

namespace msqlite::detail {

template<typename F>
struct step_wrapper : F {
    using base = F;
    template<typename F_>
    step_wrapper(F_ f) : base(std::forward<F_>(f)) {}
};

template<typename Context,
         typename F,
         typename T = detail::result_of_F<F>,
         typename Ret = context<
             typename db_of<Context>::type,
             result<T>
             >
         >
Ret pipe(Context&& ctx, detail::step_wrapper<F> o)
{
    return std::forward<Context>(ctx).conn().and_then(
        [&](auto&& conn) {
            return std::forward<Context>(ctx).get_stmt().and_then(
                [&](auto&& stmt){
                    auto r = ::msqlite::detail::step(lvref(stmt), std::move(o));
                    return Ret{
                        forward(std::forward<decltype(conn)>(conn)),
                        std::move(stmt),
                        std::move(r)};
                });
        });
}

}
