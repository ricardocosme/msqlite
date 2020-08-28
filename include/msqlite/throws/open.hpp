#pragma once

#include "msqlite/open.hpp"
#include "msqlite/throws/error.hpp"

namespace msqlite::throws {

//Take a look at 'msqlite/open.hpp' to see more details.
//
//These are alternatives to msqlite::open that throws
//std::system_error instead of returning result<T>.

inline db open(std::string_view filename)
{ return value_or_throw(::msqlite::open(filename)); }

constexpr inline cache_stmts_tag cache_stmts;

inline cache::db open(std::string_view filename, cache_stmts_tag t)
{ return value_or_throw(::msqlite::open(filename, t)); }

inline auto open()
{ return open({}); }

inline auto open(cache_stmts_tag t)
{ return throws::open({}, t); }

}
