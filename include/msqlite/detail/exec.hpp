#pragma once

#include <sqlite3.h>
#include <string_view>

namespace msqlite::detail {

inline result<void>
exec(const db& conn, std::string_view s) {
    char* emsg;
    //TODO: Support to emsg
    if(auto ec = sqlite3_exec(
           conn.get(),
           s.data(),
           nullptr,
           nullptr,
           &emsg))
    {
        sqlite3_free(emsg);
        return std::error_code{error{ec}};
    }
    return {};
}

}
