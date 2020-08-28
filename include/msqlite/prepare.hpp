#pragma once

#include "msqlite/cache/db.hpp"
#include "msqlite/db.hpp"
#include "msqlite/pipes/context.hpp"
#include "msqlite/result.hpp"
#include "msqlite/stmt.hpp"

#include "msqlite/detail/prepare.hpp"
#include "msqlite/pipes/detail/prepare.hpp"

namespace msqlite {

//Creates a prepared statement from 'stmt'
//
//This function creates a prepared statement to the statement 'stmt'
//using the database 'db'. In case of success, a 'stmt' object is
//returned, otherwise, if there is an error, it is returned as a
//std::error_code constructed through msqlite::error enumeration. This
//function calls sqlite3_prepare_v2 under the hood. The content of
//'stmt' is passed to sqlite3_prepare_v2 without any modification.
template<typename... Binds>
inline result<stmt> prepare(const db& db, std::string_view stmt, Binds&&... binds) noexcept
{ return detail::prepare(db, stmt, std::forward_as_tuple(std::forward<Binds>(binds)...)); }

template<typename... Binds>
inline result<stmt*> prepare(cache::db& db, std::string_view stmt, Binds&&... binds) noexcept
{ return detail::prepare(db, stmt, std::forward_as_tuple(std::forward<Binds>(binds)...)); }

//** Below is the API to use pipe operators to chain functions **

//Constructs a lazy statement to be chained with a pipe operator.
template<typename... Binds>
inline auto prepare(std::string_view stmt, Binds&&... binds) noexcept
{ return detail::prepare_wrapper<Binds...>{stmt, std::forward<Binds>(binds)...}; }

//Chains a database to a prepared statement constructed by 'prepare(stmt)'
//
//This overload matches an expression like 'result<db> |
//prepare(stmt)'. The left hand side operand should be a result<db> and
//the right hand side operand should be the return of the funcion
//'prepare(stmt)'. If there is a database into 'result<db>', the
//followed statement is prepared as if it was a call to 'prepare(db,
//stmt)'. If there is an error in the 'result<db>', it is propagated
//to next element in the chain until someone catch it. Example:
//
//open("database.db") | prepare("select name from person")
//
template<typename... Binds>
auto operator|(ExpectedDb auto&& res, detail::prepare_wrapper<Binds...> o) noexcept
{ return detail::pipe(std::forward<decltype(res)>(res), o); }

}
