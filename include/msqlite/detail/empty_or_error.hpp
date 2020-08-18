#pragma once

#include "msqlite/error.hpp"

#include <type_traits>

namespace msqlite::detail {

template<typename Expected,
         typename F,
         typename Ret = 
         std::invoke_result_t<F, std::optional<typename std::remove_reference_t<Expected>::error_type>>
         >
void empty_or_error(Expected&& exp,
             F&& f,
             std::enable_if_t<
             std::is_void_v<Ret>>* = nullptr)
{
    if(!exp) {
        using opt_e = std::optional<typename std::remove_reference_t<Expected>::error_type>;
        f(exp.error() == error::empty
          ? opt_e{} : exp.error());
    }
}

template<typename Expected,
         typename F,
         typename Ret = 
         std::invoke_result_t<F, std::optional<typename std::remove_reference_t<Expected>::error_type>>
         >
typename std::remove_reference_t<Expected>::value_type
empty_or_error(Expected&& exp,
        F&& f,
        std::enable_if_t<
        !std::is_void_v<Ret>>* = nullptr)
{
    if(!exp) {
        using opt_e = std::optional<typename std::remove_reference_t<Expected>::error_type>;
        return f(exp.error() == error::empty
                 ? opt_e{} : exp.error());
    }
    return *std::forward<Expected>(exp);
}

}
