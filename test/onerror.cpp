#include <boost/core/lightweight_test.hpp>
#include <msqlite/open.hpp>
#include <msqlite/exec.hpp>
#include <msqlite/step.hpp>
#include <msqlite/onerror.hpp>
#include <msqlite/prepare.hpp>

using namespace std;
using namespace msqlite;

auto create() {
    return open()
        | exec("create table person(name TEXT, salary REAL);"
               "insert into person values('abc', 1000.00);"
               "insert into person values('def', 1200.00);"
               "insert into person values('ghi', 1100.00);");
}

int main(){
    {
        auto conn = create();
        BOOST_TEST(conn);
        auto pstmt = prepare(*conn, "select salary from person where name='invalid'");
        BOOST_TEST(pstmt);
        BOOST_TEST(pstmt->get());
        using res_t = std::vector<float>;
        res_t res;
        auto r = step(*pstmt, [&](float salary){ res.emplace_back(salary); });
        BOOST_TEST(!r);
        bool called{};
        onerror(r, [&](auto e){ called = true; });
        BOOST_TEST(called);
    }
    
    {
        bool called{};
        create()
            | prepare("select salary from person where name='invalid'")
            | step([&](float salary){ return salary; })
            | onerror([&](auto e){ called = true; })
            ;
        BOOST_TEST(called);
    }

    return boost::report_errors();
}
