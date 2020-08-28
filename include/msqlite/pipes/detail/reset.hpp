#pragma once

#include "msqlite/detail/reset.hpp"

namespace msqlite::detail {

struct reset_wrapper {};

template<typename Context,
         typename Ret = context<
             typename db_of<Context>::type,
             typename std::remove_reference_t<Context>::base,
             typename stmt_of<Context>::type
             >
         >
requires(!std::is_void_v<typename std::remove_reference_t<Context>::base>)
Ret pipe(Context&& ctx, detail::reset_wrapper o)
{
    ctx.get_stmt().map(
        [&](auto& stmt) {
            ::msqlite::detail::reset(lvref(stmt));
        });
    return Ret{forward(*std::forward<Context>(ctx).conn()),
            forward(*std::forward<Context>(ctx).get_stmt()),
            std::move(ctx)};
}

template<typename Context,
         typename Ret = context<
             typename db_of<Context>::type,
             typename std::remove_reference_t<Context>::base,
             typename stmt_of<Context>::type
             >
         >
requires(std::is_void_v<typename std::remove_reference_t<Context>::base>)
Ret pipe(Context&& ctx, detail::reset_wrapper o)
{
    ctx.get_stmt().map(
        [&](auto& stmt) {
            ::msqlite::detail::reset(lvref(stmt));
        });
    return Ret{forward(*std::forward<Context>(ctx).conn()),
            forward(*std::forward<Context>(ctx).get_stmt())};
}

}
