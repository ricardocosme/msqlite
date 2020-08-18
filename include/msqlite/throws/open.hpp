#pragma once

#include "msqlite/open.hpp"
#include "msqlite/throws/value_or_throw.hpp"

namespace msqlite::throws {

//Take a look at 'msqlite/open.hpp' to see more details.
//
//This is an alternative to msqlite::open that throws
//std::system_error in case of errors instead of returning result<T>
inline db open(std::string_view filename)
{ return value_or_throw(::msqlite::open(filename)); }

inline auto open() { return open({}); }

}
