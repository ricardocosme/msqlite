#pragma once

#include "msqlite/query.hpp"
#include "msqlite/throws/value_or_throw.hpp"

namespace msqlite::throws {

inline stmt query(const db& conn, std::string_view stmt)
{ return value_or_throw(::msqlite::query(conn, stmt)); }

}
