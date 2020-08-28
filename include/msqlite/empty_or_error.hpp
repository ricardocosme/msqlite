#pragma once

#include "msqlite/concepts.hpp"
#include "msqlite/error.hpp"

#include "msqlite/pipes/detail/empty_or_error.hpp"

#include <concepts>
#include <optional>

namespace msqlite {

template<typename F, typename... Args>
constexpr bool returns_void_v = std::is_void_v<
    std::invoke_result<F, Args...>>;

template<ValueOrError T>
using optional_error = std::optional<typename std::remove_reference_t<T>::error_type>;

//Executes 'f' with an empty 'optional<std::error_code>' if 'exp' has
//'msqlite::error::empty' or executes 'f' with exp.error() if there is
//an error. If there isn't an error into 'exp', than 'f' is no
//executed.
template<ValueOrError Exp, typename F>
requires (returns_void_v<F, optional_error<Exp>>)
void empty_or_error(Exp&& exp, F&& f) {
    if(!exp)
        f(exp.error() == error::empty
          ? optional_error<Exp>{} : exp.error());
}

//Executes 'f' with an empty 'optional<std::error_code>' if 'exp' has
//'msqlite::error::empty', or, executes 'f' with exp.error() if there
//is an error. The result of 'f' is returned. If there isn't an error
//into 'exp', than 'f' is no executed.
template<ValueOrError Exp, typename F>
requires (!returns_void_v<F, optional_error<Exp>>)
auto empty_or_error(Exp&& exp, F&& f) {
    if(!exp)
        return f(exp.error() == error::empty
                 ? optional_error<Exp>{} : exp.error());
    return *std::forward<Exp>(exp);
}

template<typename F>
inline auto empty_or_error(F&& f)
{ return detail::empty_or_error_wrapper<F>{std::forward<F>(f)}; }

template<typename F>
inline auto operator|(ValueOrError auto&& exp, detail::empty_or_error_wrapper<F> o)
{ return empty_or_error(std::forward<decltype(exp)>(exp), std::move(o)); }

}
