#pragma once

#include "msqlite/db.hpp"
#include "msqlite/detail/bind.hpp"
#include "msqlite/result.hpp"
#include "msqlite/stmt.hpp"

#include <functional>
#include <sqlite3.h>
#include <tuple>
#include <utility>

namespace msqlite::detail {

template<typename Binds>
inline result<stmt>
prepare_impl(const db& conn, std::string_view s, Binds&& binds) noexcept {
    sqlite3_stmt* pstmt;
    if(auto ec = sqlite3_prepare_v2(
           conn.get(),
           s.data(),
           -1,
           &pstmt,
           nullptr)) {
        sqlite3_finalize(pstmt);
        return std::error_code{error{ec}};
    }
    bind_params(pstmt, std::forward<Binds>(binds));
    return stmt{pstmt};
}

}
