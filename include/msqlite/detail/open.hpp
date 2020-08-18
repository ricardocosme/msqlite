#pragma once

#include <sqlite3.h>
#include <string_view>

namespace msqlite::detail {

inline result<db>
open(std::string_view filename) noexcept {
    sqlite3* pdb;
    if(auto ec = sqlite3_open(filename.data(), &pdb)) {
        sqlite3_close(pdb);
        return std::error_code{error{ec}};
    }
    return db{pdb};
}

}
