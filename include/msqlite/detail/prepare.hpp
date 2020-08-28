#pragma once

#include "msqlite/db.hpp"
#include "msqlite/cache/db.hpp"
#include "msqlite/detail/prepare_impl.hpp"
#include "msqlite/result.hpp"
#include "msqlite/stmt.hpp"

#include <sqlite3.h>

namespace msqlite::detail {

template<typename... Binds>
inline result<stmt> prepare(const db& conn, std::string_view s, std::tuple<Binds...> binds)
{ return prepare_impl(conn, s, std::move(binds)); }

template<typename... Binds>
inline result<stmt*> prepare(cache::db& db_, std::string_view stmt, std::tuple<Binds...> binds)
{ return db_.prepare(stmt, std::move(binds)); }

}
