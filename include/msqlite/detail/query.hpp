#pragma once

#include <sqlite3.h>

namespace msqlite::detail {

inline result<stmt>
query(const db& conn, std::string_view s) {
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
    return stmt{pstmt};
}

}
