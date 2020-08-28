#pragma once

#include "msqlite/prepare.hpp"
#include "msqlite/throws/value_or_throw.hpp"

namespace msqlite::throws {

template<typename... Binds>
inline stmt prepare(const db& db, std::string_view stmt, Binds&&... binds)
{ return value_or_throw(::msqlite::prepare(db, stmt, std::forward<Binds>(binds)...)); }

template<typename... Binds>
inline stmt* prepare(cache::db& db, std::string_view stmt, Binds&&... binds)
{ return value_or_throw(::msqlite::prepare(db, stmt, std::forward<Binds>(binds)...)); }

}
