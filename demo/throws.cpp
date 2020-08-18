#include <msqlite.hpp>

#include <iostream>

namespace sql = msqlite::throws;

int main() {
    try {
        auto conn = sql::open("dev.db");
        auto stmt = sql::query(conn, "select name, salary from person");
        sql::for_each(stmt, [](std::string_view name, float salary)
        { std::cout << name << "," << salary << std::endl; });
    } catch(const std::system_error& e)
    { std::cout << e.code() << std::endl; }
}
