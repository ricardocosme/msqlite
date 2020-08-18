#pragma once

#include "msqlite/db.hpp"
#include "msqlite/result.hpp"

#include "msqlite/detail/open.hpp"

namespace msqlite {

//Opens a new database connection
//
//It calls sqlite3_open under the hood. The value of 'filename' is
//passed directly to the sqlite3 function.
//
//In case of success a 'db' is returned to represent the established
//connection, otherwise a msqlite::error is encapsulated into a
//std::error_code and it is returned as an unexpected value by
//result<db>.
//
//You want to use C++ exceptions?
//Take a look at 'msqlite/throws/open.hpp' if you want to use
//exceptions instead of result<T>, which is by the way an alias to
//expected<T, std::error_code>.
inline result<db>
open(std::string_view filename) noexcept 
{ return detail::open(filename); }

//Opens a new database connection as an in-memory database
inline auto open() noexcept
{ return open({}); }

}
