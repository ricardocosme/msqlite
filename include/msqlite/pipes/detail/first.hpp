#pragma once

#include "msqlite/detail/step.hpp"

namespace msqlite::detail {

template<typename F>
struct first_wrapper : F {
    using base = F;
    template<typename F_>
    first_wrapper(F_ f) : base(std::forward<F_>(f)) {}
};

template<typename Context,
         typename F,
         typename T = detail::result_of_F<F>,
         typename Ret = context<
             typename db_of<Context>::type,
             result<T>,
             typename stmt_of<Context>::type
             >
         >
requires(!std::is_void_v<T>)
Ret pipe(Context&& ctx, detail::first_wrapper<F> o)
{
    auto r = ctx.conn().and_then(
        [&](auto& conn) {
            return ctx.get_stmt().and_then(
                [&](auto& stmt){
                    auto ret = ::msqlite::detail::step(lvref(stmt), std::move(o));
                    ::msqlite::detail::reset(lvref(stmt));
                    return ret;
                });
        });
    if(r)
    return Ret(
        forward(*std::forward<Context>(ctx).conn()),
        forward(*std::forward<Context>(ctx).get_stmt()),
        std::move(r));
    else
    return Ret(
        forward(*std::forward<Context>(ctx).conn()),
        forward(*std::forward<Context>(ctx).get_stmt()),
        r.error());
}

template<typename Context,
         typename F,
         typename T = detail::result_of_F<F>,
         typename Ret = context<
             typename db_of<Context>::type,
             result<T>,
             typename stmt_of<Context>::type
             >
         >
requires(std::is_void_v<T>)
Ret pipe(Context&& ctx, detail::first_wrapper<F> o)
{
    auto r = ctx.conn().and_then(
        [&](auto& conn) {
            return ctx.get_stmt().and_then(
                [&](auto& stmt){
                    auto ret = ::msqlite::detail::step(lvref(stmt), std::move(o));
                    ::msqlite::detail::reset(lvref(stmt));
                    return ret;
                });
        });
    if(r)
        return Ret{
            forward(*std::forward<Context>(ctx).conn()),
                forward(*std::forward<Context>(ctx).get_stmt())};
    else
        return Ret{
            forward(*std::forward<Context>(ctx).conn()),
                r.error()};
}

}
