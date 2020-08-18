#pragma once

#include "msqlite/db.hpp"
#include "msqlite/pipes/context.hpp"
#include "msqlite/result.hpp"
#include "msqlite/stmt.hpp"

#include "msqlite/detail/query.hpp"
#include "msqlite/pipes/detail/query.hpp"

namespace msqlite {

//Creates a prepared statement from 'stmt'
//
//This function creates a prepared statement to the statement 'stmt'
//using the database 'db_'. In case of success, a 'stmt' object is
//returned, otherwise, if there is an error, it is returned as a
//std::error_code constructed through msqlite::error enumeration. This
//function calls sqlite3_prepare_v2 under the hood. The content of
//'stmt' is passed to sqlite3_prepare_v2 without any modification.
inline result<stmt> query(const db& db_, std::string_view stmt)
{ return detail::query(db_, stmt); }

//** Below is the API to use pipe operators to chain functions **

//Constructs a lazy statement to be chained with a pipe operator. 
inline auto query(std::string_view stmt)
{ return detail::query_wrapper{stmt}; }

//Chains a database to a prepared statement constructed by 'query(stmt)'
//
//This overload matches an expression like 'result<db> |
//query(stmt)'. The left hand side operand should be a result<db> and
//the right hand side operand should be the return of the funcion
//'query(stmt)'. If there is a database into 'result<db>', the
//followed statement is prepared as if it was a call to 'query(db,
//stmt)'. If there is an error in the 'result<db>', it is propagated
//to next element in the chain until someone catch it. Example:
//
//open("database.db") | query("select name from person")
//
auto operator|(ExpectedDb auto&& res, detail::query_wrapper o)
{ return detail::pipe(std::forward<decltype(res)>(res), o); }

}
