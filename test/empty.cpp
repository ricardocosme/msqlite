#include "msqlite/open.hpp"
#include "msqlite/empty.hpp"
#include "msqlite/exec.hpp"
#include "msqlite/query.hpp"

#include <boost/core/lightweight_test.hpp>

using namespace std;
using namespace msqlite;

inline auto db() {
    return open()
        .exec("create table person(name TEXT, salary REAL);"
              "insert into person values ('joao', 10000.00);"
              "insert into person values ('maria', 13000.00)");
}

int main() {
    //empty
    {
        auto r = db()
            .query("select salary from person where name = 'not_exist'");
        
        BOOST_TEST(r);
        auto e = empty(*r);
        BOOST_TEST(e);
        BOOST_TEST(*e == true);
    }

    //not empty
    {
        auto r = db()
            .query("select salary from person where name = 'joao'");
        
        BOOST_TEST(r);
        auto e = empty(*r);
        BOOST_TEST(e);
        BOOST_TEST(*e == false);
    }

    //TODO: error

    
    return boost::report_errors();
}
