#include <msqlite.hpp>

#include <iostream>

namespace sql = msqlite;
using namespace std;

auto salary(string_view name) {
    return sql::open("dev.db")
    | sql::prepare("select salary from person where name= ?", name)
    | sql::step([](float salary){ return salary; });
}

int main() {
    if(auto s = salary("john"))
        cout << *s << endl;
    else {
        if(s.error() == sql::error::empty) cout << "not exists";
        else cout << s.error();
        cout << endl;
    }
}
