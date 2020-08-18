#include <msqlite.hpp>

namespace sql = msqlite;

int main() {
    sql::open("dev.db")
    | sql::exec("create table person(name TEXT unique, salary REAL);"
                "insert into person values('jimmy', 10000.00);"
                "insert into person values('john', 9500.00);"
                "insert into person values('robert', 11000.00)");
}
