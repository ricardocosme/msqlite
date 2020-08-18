#pragma once

#include "msqlite/db.hpp"
#include "msqlite/pipes/context.hpp"
#include "msqlite/result.hpp"

#include "msqlite/detail/exec.hpp"
#include "msqlite/pipes/detail/exec.hpp"

namespace msqlite {

//Executes a statement 'stmt' without returning any results
//
//This function executes the SQL statement 'stmt' using the database
//'db_'. If there is an error, it is returned as a std::error_code
//constructed through msqlite::error enumeration. This function calls
//sqlite3_exec under the hood. The content of 'stmt' is passed to
//sqlite3_exec without any modification.
inline result<void> exec(const db& db_, std::string_view stmt)
{ return detail::exec(db_, stmt); }


//** Below is the API to use pipe operators to chain functions **

//Constructs a lazy statement to be chained with a pipe operator. 
inline auto exec(std::string_view stmt) noexcept
{ return detail::exec_wrapper{stmt}; }

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
auto operator|(ExpectedDb auto&& res, detail::exec_wrapper o)
{ return detail::pipe(std::forward<decltype(res)>(res), o); }

}
