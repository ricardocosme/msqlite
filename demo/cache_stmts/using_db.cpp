#include <msqlite.hpp>

#include <iostream>

namespace sql = msqlite;
using namespace std;

//The argument `cache_stmts` defines that the returned
//database(cache::db) caches each prepared statement which is compiled
//through the database.
//
//The big advantage of this approach is that it doesn't require
//anything else than the argument `cache_stmts` to cache
//statements. Dynamic allocation is used to store the statements and
//if this is a concern, it's a good idea to consider a static solution
//like one demonstrated at 'demo/cache_stmts/static.cpp'. BTW, don't
//assume that this solution is slower than a static approach, take
//some measures before any conclusion.
auto db = sql::open("dev.db", sql::cache_stmts);

int main() {
    string_view name{"john"};
    db
    | sql::prepare("select name from person where name = ?", name)
    | sql::for_each([](string_view s){ cout << s << endl; })
    | sql::onerror([](auto e){ cout << e << endl; });
}
