#pragma once

#include "msqlite/result.hpp"
#include "msqlite/pipes/context.hpp"
#include "msqlite/stmt.hpp"

#include "msqlite/detail/step.hpp"
#include "msqlite/pipes/detail/step.hpp"

namespace msqlite {

//Evaluates a prepared statement 'stmt'
//
//This function evaluates the prepared statement 'stmt' and calls once
//the function 'f' to handle the returned tuple(or row). The function
//must receive the elements from the tuple in the order that were
//declared in the statement. Each parameter of the function must have
//a type related to the type of the result-column. The function is not
//called if there isn't any row to be returned and an error
//msqlite::error::empty is returned. If there is any other error, it
//is returned as a std::error_code constructed through msqlite::error
//enumeration. This function calls sqlite3_step under the hood.
//
//The table below shows the supported types for the parameters of 'f':
//
//| result-column | C++ type         |
//| TEXT          | std::string_view |
//|               | std::string      |
//| REAL          | float            |
//
//If 'f' returns void, than 'result<void>' is returned, otherwise
//'result<T>' is returned while 'T' is the returned type of 'f'.
//
//Example:
//
//  if(auto dbase = open("person.db"))
//      if(auto stmt = prepare(*dbase, "select name, salary from person")) {
//          auto r = step(*stmt, [](std::string_view name, float salary)
//              { std::cout << name << "|" << salary << std::endl; });
//          if(!r) std::cout << r.error() << std::endl;
//      } else std::cout << stmt.error() << std::endl;
//  else std::cout << dbase.error() << std::endl;
//
//Take a look below to see the usage of pipes in order to reduce the
//boilerplate.
//
template<typename F, typename T = detail::result_of_F<F> >
result<T> step(stmt& stmt, F&& f)
{ return detail::step<F, T>(stmt, std::forward<F>(f)); }

//** Below is the API to use pipe operators to chain functions **

//Constructs a lazy step to be chained with a pipe operator. 
template<typename F>
inline auto step(F&& f)
{ return detail::step_wrapper<F>{std::forward<F>(f)}; }

//Chains a prepared statement to a lazy step constructed by 'step(F)'
//
//This overload matches an expression like 'result<stmt> |
//step(F)'. The left hand side operand should be a result<stmt> and
//the right hand side operand should be the return of the funcion
//'step(F)'. If there is a prepared statement into 'result<stmt>', the
//statement is evaluated as if it was a call to 'step(stmt, F)'. If
//there is an error in the 'result<stmt>', it is propagated to next
//element in the chain until someone catch it. Example:
//
//  auto r = open("person.db")
//  | prepare("select name, salary from person")
//  | step([](std::string_view name, float salary)
//    { std::cout << name << "|" << salary << std::endl; });
//  if(!r) std::cout << r.error() << std::endl;
//
template<typename F>
inline auto operator|(Context auto&& ctx, detail::step_wrapper<F> o)
{ return detail::pipe(std::forward<decltype(ctx)>(ctx), o); }

}
