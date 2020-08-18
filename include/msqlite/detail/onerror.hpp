#pragma once

#include "msqlite/error.hpp"
#include <type_traits>

namespace msqlite::detail {

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
    if(!exp) f(exp.error());
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
    if(!exp) return f(exp.error());
    return *std::forward<Expected>(exp);
}

}
