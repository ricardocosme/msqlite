#pragma once

#include "msqlite/result.hpp"
#include "msqlite/pipes/context.hpp"
#include "msqlite/stmt.hpp"

#include "msqlite/detail/reset.hpp"
#include "msqlite/detail/step.hpp"
#include "msqlite/pipes/detail/first.hpp"

namespace msqlite {

//It's the same as step() followed by reset()
template<typename F, typename T = detail::result_of_F<F> >
result<T> first(stmt& stmt, F&& f) {
    auto r = detail::step<F, T>(stmt, std::forward<F>(f));
    if(r) detail::reset(r.get_stmt());
    return r;
}

//** Below is the API to use pipe operators to chain functions **

template<typename F>
inline auto first(F&& f)
{ return detail::first_wrapper<F>{std::forward<F>(f)}; }

template<typename F>
inline auto operator|(Context auto&& ctx, detail::first_wrapper<F> o)
{ return detail::pipe(std::forward<decltype(ctx)>(ctx), o); }

}
