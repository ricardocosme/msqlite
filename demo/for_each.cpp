#include <msqlite.hpp>

#include <iostream>

namespace sql = msqlite;
using namespace std;

int main() {
    sql::open("dev.db")
    | sql::prepare("select name, salary from person")
    | sql::for_each([](string_view name, float salary)
      { cout << name << "," << salary << endl; })
    | sql::onerror([](auto e){ cout << e << endl; });
}
