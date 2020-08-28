#pragma once

#include "msqlite/stmt.hpp"

#include <variant>
#include <sqlite3.h>
#include <string_view>
#include <string>

namespace msqlite::detail {

struct param : std::variant<float, std::string_view> {
    using base = std::variant<float, std::string_view>;
    using base::base;

    //TODO: These function can't throw an exception that will not be
    //catched by value().
    operator float() const
    { return std::get<float>(*this); }
    
    operator std::string_view() const
    { return std::get<std::string_view>(*this); }
    
    operator std::string() const
    { return std::string{std::get<std::string_view>(*this)}; }
};

inline param value(sqlite3_stmt* stmt, int i) {
    switch(sqlite3_column_type(stmt, i)) {
    case SQLITE_FLOAT:
        return static_cast<float>(sqlite3_column_double(stmt, i));
    default: {
        auto s = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
        return s ? s : "";
    }
    }
}
template <typename F>
struct arity : arity<decltype(&F::operator())> { };

template <typename F, typename Ret, typename... Args>
struct arity<Ret(F::*)(Args...)const>
{ static const int value = sizeof...(Args); };

template<typename F, std::size_t... idx>
auto call(F&& f, sqlite3_stmt* stmt, std::index_sequence<idx...>)
{ return f(value(stmt, idx)...); }

template<typename F, typename...Args>
auto call_once_impl(F&& f, Args&&... args) {
    return f(std::forward<Args>(args)...);
}

template<typename F, std::size_t... idx>
auto call_once(F&& f, stmt& stmt, std::index_sequence<idx...>)
{ return call_once_impl(std::forward<F>(f), value(stmt.get(), idx)...); }

}
