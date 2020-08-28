#include <msqlite.hpp>

#include <iostream>

namespace sql = msqlite;
using namespace std;

//The statements can be persisted storing the database and each
//prepared statement. The user choose how to do this and it uses
//lvalue expressions to reuse the statements. This solution is more
//flexible and efficient than using cache::db but the user has more
//work to do. Take a look at demo/cache_stmts/using_db.cpp to see an
//alternative which is simpler than this.

auto db = sql::open("dev.db");
auto select_person =
    db | sql::prepare("select name from person where name = ?");

int main() {
    select_person
    | sql::bind("john")
    | sql::for_each([](string_view s){ cout << s << endl; })
    | sql::onerror([](auto e){ cout << e << endl; });
}
