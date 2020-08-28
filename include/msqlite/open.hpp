#pragma once

#include "msqlite/cache/db.hpp"
#include "msqlite/db.hpp"
#include "msqlite/result.hpp"

#include "msqlite/detail/open.hpp"

namespace msqlite {

//Opens a new database connection
//
//It calls sqlite3_open under the hood. The value of 'filename' is
//passed directly to the sqlite function.
//
//In case of success a 'db' is returned to represent the established
//connection, otherwise a msqlite::error is encapsulated into a
//std::error_code and it is returned as an unexpected value by
//result<db>.
//
//Take a look at 'msqlite/throws/open.hpp' if you want to use C++
//exceptions instead of result<T>.
inline result<db>
open(std::string_view filename) noexcept 
{ return detail::open(filename); }

constexpr inline cache_stmts_tag cache_stmts;

//Opens a new database connection that caches prepared statements
//
//This function behaves like open(filename), the only difference is
//that it constructs a database connection('cache::db') that persists
//each prepared statements that are evaluated through the connection.
//
//Example: open("dev.db", cache_stmts);
inline result<cache::db>
open(std::string_view filename, cache_stmts_tag t) noexcept 
{ return detail::open_with_cache(filename); }

//Opens a new database connection as an in-memory database
inline auto open() noexcept
{ return open({}); }

inline auto open(cache_stmts_tag t) noexcept
{ return open({}, t); }

}
