#pragma once

#include "msqlite/pipes/context.hpp"
#include "msqlite/pipes/detail/for_each.hpp"
#include "msqlite/result.hpp"
#include "msqlite/stmt.hpp"

#include "msqlite/detail/for_each.hpp"

namespace msqlite {

//Evaluates a prepared statement 'stmt'
//
//This function evaluates the prepared statement 'stmt' and calls the
//function 'f' to handle each returned tuple(or row). The function
//must receive the elements from the tuple in the order that were
//declared in the statement. Each parameter of the function must have
//a type related to the type of the result-column. The function is not
//called if there isn't any row to be returned. If there is any error,
//it is returned as a std::error_code constructed through
//msqlite::error enumeration. This function calls sqlite3_step under
//the hood until a result SQLITE_DONE is returned.
//
//The table below shows the supported types for the parameters of 'f':
//
//| result-column | C++ type         |
//| TEXT          | std::string_view |
//|               | std::string      |
//| REAL          | float            |
//
//Example:
//
//  if(auto dbase = open("person.db"))
//      if(auto stmt = query(*dbase, "select name, salary from person")) {
//          auto r = step(*stmt, [](std::string_view name, float salary)
//              { std::cout << name << "|" << salary << std::endl; });
//          if(!r) std::cout << r.error() << std::endl;
//      } else std::cout << stmt.error() << std::endl;
//  else std::cout << dbase.error() << std::endl;
//
//Take a look at the functions below to see the usage of pipes in
//order to reduce the boilerplate.
//
template<typename F>
result<void> for_each(stmt& stmt, F&& f)
{ return detail::for_each(stmt, std::forward<F>(f)); }

//** Below is the API to use pipe operators to chain functions **

//Constructs a lazy for_each to be chained with a pipe operator. 
template<typename F>
inline auto for_each(F&& f)
{ return detail::for_each_wrapper<F>{std::forward<F>(f)}; }

//Chains a prepared statement to a lazy for_each constructed by 'for_each(F)'
//
//This overload matches an expression like 'result<stmt> |
//for_each(F)'. The left hand side operand should be a result<stmt>
//and the right hand side operand should be the return of the funcion
//'for_each(F)'. If there is a prepared statement into 'result<stmt>',
//the statement is evaluated as if it was a call to 'for_each(stmt,
//F)'. If there is an error in the 'result<stmt>', it is propagated to
//next element in the chain until someone catch it. Example:
//
//  auto r = open("person.db")
//  | query("select name, salary from person")
//  | for_each([](std::string_view name, float salary)
//    { std::cout << name << "|" << salary << std::endl; });
//  if(!r) std::cout << r.error() << std::endl;
//
template<typename F>
inline auto operator|(Context auto&& ctx, detail::for_each_wrapper<F> o)
{ return detail::pipe(std::forward<decltype(ctx)>(ctx), o); }

}
