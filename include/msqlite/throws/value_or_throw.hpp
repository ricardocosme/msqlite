#pragma once

namespace msqlite::throws {

template<typename E = std::system_error, typename Exp>
std::enable_if_t<
    std::is_void_v<typename std::remove_reference_t<Exp>::value_type>
    >
value_or_throw(Exp&& exp)
{ if(!exp) throw E{exp.error()}; }

template<typename E = std::system_error, typename Exp>
std::enable_if_t<
    !std::is_void_v<typename std::remove_reference_t<Exp>::value_type>,
    typename std::remove_reference_t<Exp>::value_type
    >
value_or_throw(Exp&& exp)
{
    if(exp) return *std::forward<Exp>(exp);
    throw E{exp.error()};
}

}
