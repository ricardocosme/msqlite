#pragma once

#include "msqlite/cache/db.hpp"
#include "msqlite/db.hpp"
#include "msqlite/pipes/context.hpp"
#include "msqlite/result.hpp"

#include "msqlite/detail/exec.hpp"
#include "msqlite/pipes/detail/exec.hpp"

namespace msqlite {

//Executes a statement 'stmt' without returning any results
//
//This function executes the SQL statement 'stmt' using the database
//'db'. If there is an error, it is returned as a std::error_code
//constructed through msqlite::error enumeration. This function calls
//sqlite3_exec under the hood. The content of 'stmt' is passed to
//sqlite3_exec without any modification.
template<typename... Values>
inline result<void>
exec(const db& db, std::string_view stmt, Values&&... values) noexcept {
    return detail::exec(db, stmt,
                        std::forward_as_tuple(std::forward<Values>(values)...));
}

//Executes a statement 'stmt' without returning any results and
//persists the prepared statement into 'db'
//
//This function behaves like the above, the only difference is that it
//persists the statement in the database connection.
template<typename... Values>
inline result<void>
exec(cache::db& db, std::string_view stmt, Values&&... values) noexcept {
    return detail::exec(db, stmt,
                        std::forward_as_tuple(std::forward<Values>(values)...));
}

template<typename... Values>
inline result<void>
exec(stmt& stmt, Values&&... values) noexcept {
    return detail::exec(stmt,
                        std::forward_as_tuple(std::forward<Values>(values)...));
}

//** Below is the API to use pipe operators to chain functions **

//Constructs a lazy statement to be chained with a pipe operator. 
template<typename... Values>
inline auto exec(std::string_view stmt, Values&&... values) noexcept
{ return detail::exec_wrapper<Values...>{stmt, std::forward<Values>(values)...}; }

template<typename... Values>
inline auto exec_with(Values&&... values) noexcept
{ return detail::exec_with_wrapper<Values...>{std::forward<Values>(values)...}; }

inline auto exec() noexcept
{ return detail::exec_with_wrapper{}; }

//Chains a database to a statement constructed by 'exec(stmt)'
//
//This overload matches an expression like 'result<db> |
//exec(stmt)'. The left hand side operand should be a result<db> and
//the right hand side operand should be the return of the funcion
//'exec(stmt)'. If there is a database into 'result<db>', the followed
//statement is executed as if it was a call to 'exec(db, stmt)'. If
//there is an error in the 'result<db>', this error is propagated to
//next element in the chain until someone catch it. Example:
//
//open("database.db") | exec("create table person(name TEXT)")
//
template<typename... Values>
auto operator|(ExpectedDb auto&& res, detail::exec_wrapper<Values...> o) noexcept
{ return detail::pipe(std::forward<decltype(res)>(res), o); }

template<typename... Values>
auto operator|(Context auto&& ctx, detail::exec_with_wrapper<Values...> o) noexcept
{ return detail::pipe(std::forward<decltype(ctx)>(ctx), std::move(o)); }

}
