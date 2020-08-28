#pragma once

#include "msqlite/stmt.hpp"
#include "msqlite/pipes/context.hpp"
#include "msqlite/pipes/detail/bind.hpp"

#include <tuple>
#include <utility>

namespace msqlite {

template<typename... Values>
inline void bind(stmt& stmt, Values&&... values) noexcept {
    return detail::bind_params(
        stmt.get(), std::forward_as_tuple(std::forward<Values>(values)...));
}

template<typename... Values>
inline auto bind(Values&&... values) noexcept
{ return detail::bind_wrapper<Values...>{std::forward<Values>(values)...}; }

template<typename... Values>
auto operator|(Context auto&& ctx, detail::bind_wrapper<Values...> o) noexcept
{ return detail::pipe(std::forward<decltype(ctx)>(ctx), std::move(o)); }

}
