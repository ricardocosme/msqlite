#pragma once

#include "msqlite/cache/db.hpp"
#include "msqlite/db.hpp"
#include "msqlite/detail/bind.hpp"
#include "msqlite/detail/prepare_impl.hpp"
#include "msqlite/result.hpp"

#include <sqlite3.h>
#include <string_view>
#include <tuple>
#include <utility>

namespace msqlite::detail {

template<typename... Values>
inline result<void>
exec(const db& conn, std::string_view s, std::tuple<Values...> values) noexcept {
    sqlite3_stmt* pstmt;
    const char *sql, *leftover;
    sql = s.data();
    int rc;
    do {
        rc  = sqlite3_prepare_v2(
            conn.get(),
            sql,
            -1,
            &pstmt,
            &leftover);
        stmt scoped_stmt{pstmt};
        if(rc != SQLITE_OK)
            return std::error_code{error{rc}};
        //TODO: Reuse the function above
        bind_params(pstmt, values);
        int step_rc = sqlite3_step(pstmt);
        if(step_rc != SQLITE_DONE && step_rc != SQLITE_ROW)
            return std::error_code{error{step_rc}};
        sql = leftover;
        //TODO: Do I need to check white spaces?
    } while(rc == SQLITE_OK && sql[0]);
    return {};
}

template<typename... Values>
inline result<void>
exec(cache::db& conn, std::string_view s, std::tuple<Values...> values) noexcept {
    if(auto r = conn.prepare(s, std::move(values))) {
        auto stmt = (*r)->get();
        detail::bind_params(stmt, std::move(values));
        auto rc = sqlite3_step(stmt);
        sqlite3_reset(stmt);
        if(rc != SQLITE_DONE)
            return std::error_code{error{rc}};
        return {};
    } else return r.error();
}

template<typename... Values>
inline result<void>
exec(stmt& stmt, std::tuple<Values...> values) noexcept {
    bind_params(stmt.get(), std::move(values));
    auto rc = sqlite3_step(stmt.get());
    sqlite3_reset(stmt.get());
    if(rc != SQLITE_DONE && rc != SQLITE_ROW)
        return std::error_code{error{rc}};
    return {};
}

}
