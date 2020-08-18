#include <msqlite.hpp>

#include <iostream>

namespace sql = msqlite;

auto salary(const std::string& name) {
    return sql::open("dev.db")
        | sql::query("select salary from person where name='" + name + "'")
        | sql::step([](float salary){ return salary; });
}

int main() {
    if(auto s = salary("john"))
        std::cout << *s << std::endl;
    else
        std::cout << (s.error() == sql::error::empty
                      ? "not exists" : s.error().message()) << std::endl;
}
