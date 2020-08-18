#pragma once

#include "msqlite/detail/call.hpp"

namespace msqlite::detail {

template<typename F>
using result_of_F = decltype(
    detail::call_once(
        std::declval<F>(),
        std::declval<stmt&>(),
        std::make_index_sequence<
        detail::arity<std::remove_reference_t<F>>::value>{}));

template<typename F, typename T = result_of_F<F> >
result<T>
step(stmt& stmt, F&& f) {
    auto res = sqlite3_step(stmt.get());
    if(res == SQLITE_ROW) {
        if constexpr(std::is_same_v<T, void>) {
            detail::call_once(
                f, stmt, std::make_index_sequence<
                detail::arity<std::remove_reference_t<decltype(f)>>::value>{});
            return {};
        } else {
            if constexpr(std::is_same_v<T, std::error_code>)
                return 
                    detail::call_once(
                        f, stmt, std::make_index_sequence<
                        detail::arity<std::remove_reference_t<decltype(f)>>::value>{});
            else
                return detail::call_once(
                    f, stmt, std::make_index_sequence<
                    detail::arity<std::remove_reference_t<decltype(f)>>::value>{});
        }
    } else if(res == SQLITE_DONE)
        return std::error_code{error::empty};
        
    return std::error_code{error{res}};
}

}
