#pragma once

#include "msqlite/exec.hpp"
#include "msqlite/throws/error.hpp"

namespace msqlite::throws {

//Take a look at 'msqlite/exec.hpp' to see more details.
//
//These are alternatives to msqlite::exec that throws
//std::system_error instead of returning result<T>.

inline void exec(const db& db, std::string_view stmt)
{ return value_or_throw(::msqlite::exec(db, stmt)); }

template<typename... Values>
inline void exec(stmt& stmt, Values&&... values) {
    return value_or_throw(
        ::msqlite::exec(stmt, std::forward<Values>(values)...));
}

template<typename... Values>
inline void exec(cache::db& db, std::string_view stmt, Values&&... values) {
    return value_or_throw(
        ::msqlite::exec(db, stmt, std::forward<Values>(values)...));
}

}
