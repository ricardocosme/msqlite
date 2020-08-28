#include <msqlite.hpp>

#include <iostream>

namespace sql = msqlite::throws;
using namespace std;

int main() {
    try {
        auto conn = sql::open("dev.db");
        auto stmt = sql::prepare(conn, "select name, salary from person");
        sql::for_each(stmt, [](string_view name, float salary)
        { cout << name << "," << salary << endl; });
    } catch(const system_error& e)
    { cout << e.code() << endl; }
}
