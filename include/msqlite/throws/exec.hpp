#pragma once

#include "msqlite/exec.hpp"
#include "msqlite/throws/value_or_throw.hpp"

namespace msqlite::throws {

inline void exec(const db& conn, std::string_view stmt)
{ return value_or_throw(::msqlite::exec(conn, stmt)); }

}
