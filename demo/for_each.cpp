#include <msqlite.hpp>

#include <iostream>

namespace sql = msqlite;

int main() {
    sql::open("dev.db")
    | sql::query("select name, salary from person")
    | sql::for_each([](std::string_view name, float salary)
      { std::cout << name << "," << salary << std::endl; })
    | sql::onerror([](auto e){ std::cout << e << std::endl; });
}
