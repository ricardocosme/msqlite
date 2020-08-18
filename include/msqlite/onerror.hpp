#pragma once

#include "msqlite/concepts.hpp"

#include "msqlite/detail/onerror.hpp"
#include "msqlite/pipes/detail/onerror.hpp"

namespace msqlite {

template<typename Expected,
         typename F,
         typename Ret = 
         std::invoke_result_t<F, typename std::remove_reference_t<Expected>::error_type>
         >
void onerror(Expected&& exp,
             F&& f,
             std::enable_if_t<
             std::is_same_v<Ret, void>>* = nullptr)
{
    detail::onerror(std::forward<Expected>(exp), std::forward<F>(f)); 
}

template<typename Expected,
         typename F,
         typename Ret = 
         std::invoke_result_t<F, typename std::remove_reference_t<Expected>::error_type>
         >
typename std::remove_reference_t<Expected>::value_type
onerror(Expected&& exp,
        F&& f,
        std::enable_if_t<
        !std::is_same_v<Ret, void>>* = nullptr)
{
    return detail::onerror(std::forward<Expected>(exp), std::forward<F>(f)); 
}

template<typename F>
inline auto onerror(F&& f)
{ return detail::onerror_wrapper<F>{std::forward<F>(f)}; }

template<typename F>
inline auto operator|(ValueOrError auto&& exp, detail::onerror_wrapper<F> o)
{ return detail::onerror(std::forward<decltype(exp)>(exp), std::move(o)); }

}
