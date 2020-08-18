#pragma once

#include "msqlite/db.hpp"

#include <string>

namespace msqlite {

template<typename Db>
struct exp_db;

exp_db<db*> exec(db& conn, std::string_view s);
exp_db<db> exec(db&& conn, std::string_view s);

}
