#pragma once

#include "msqlite/cache/db.hpp"
#include "msqlite/db.hpp"
#include "msqlite/result.hpp"

#include <sqlite3.h>
#include <string_view>
#include <utility>

namespace msqlite {
struct cache_stmts_tag {};
} //namespace msqlite

namespace msqlite::detail {

inline result<db>
open(std::string_view filename) noexcept {
    sqlite3* pdb;
    if(auto rc = sqlite3_open(filename.data(), &pdb)) {
        sqlite3_close(pdb);
        return std::error_code{error{rc}};
    }
    return db{pdb};
}

inline result<cache::db>
open_with_cache(std::string_view filename) noexcept {
    return static_cast<result<cache::db>&&>(
        open(filename).map([](auto&& db)
        { return cache::db(std::forward<decltype(db)>(db)); }));
}

}
