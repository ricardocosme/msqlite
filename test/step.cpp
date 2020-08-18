#include <boost/core/lightweight_test.hpp>
#include <msqlite/open.hpp>
#include <msqlite/exec.hpp>
#include <msqlite/step.hpp>
#include <msqlite/query.hpp>

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
        auto pstmt = query(*conn, "select salary from person");
        BOOST_TEST(pstmt);
        BOOST_TEST(pstmt->get());
        using res_t = std::vector<float>;
        res_t res;
        auto r = step(*pstmt, [&](float salary){ res.emplace_back(salary); });
        BOOST_TEST(r);
        BOOST_TEST((res == res_t{1000.00}));
    }
    
    {
        auto conn = create();
        BOOST_TEST(conn);
        auto pstmt = query(*conn, "select salary from person where name='not_exist'");
        BOOST_TEST(pstmt);
        BOOST_TEST(pstmt->get());
        using res_t = std::vector<float>;
        res_t res;
        auto r = step(*pstmt, [&](float salary){ res.emplace_back(salary); });
        BOOST_TEST(!r);
        BOOST_TEST(r.error() == error::empty);
        BOOST_TEST((res == res_t{}));
    }

    {
        auto r = create()
            | query("select salary from person")
            | step([&](float salary){ return salary; })
            ;
        BOOST_TEST(r);
        BOOST_TEST(*r == 1000.00);
        BOOST_TEST(r.conn());
        BOOST_TEST(r.conn()->get());
        BOOST_TEST(r.get_stmt());
        BOOST_TEST(r.get_stmt()->get());
    }
    
    {
        auto stmt = create()
            | query("select salary from person");
        auto r = stmt
            | step([&](float salary){ return salary; })
            ;
        BOOST_TEST(r);
        BOOST_TEST(*r == 1000.00);
        BOOST_TEST(r.conn());
        BOOST_TEST((*r.conn())->get());
        BOOST_TEST(r.get_stmt());
        BOOST_TEST(r.get_stmt()->get());
    }
    
    {
        auto conn = create();
        auto r = conn
            | query("select salary from person")
            | step([&](float salary){ return salary; })
            ;
        BOOST_TEST(r);
        BOOST_TEST(*r == 1000.00);
        BOOST_TEST(r.conn());
        BOOST_TEST((*r.conn())->get());
        BOOST_TEST(r.get_stmt());
        BOOST_TEST(r.get_stmt()->get());
    }
    
    {
        auto conn = create();
        auto stmt = conn
            | query("select salary from person");
        auto r = stmt
            | step([&](float salary){ return salary; })
            ;
        BOOST_TEST(r);
        BOOST_TEST(*r == 1000.00);
        BOOST_TEST(r.conn());
        BOOST_TEST((*r.conn())->get());
        BOOST_TEST(r.get_stmt());
        BOOST_TEST(r.get_stmt()->get());
    }
    
    {
        std::vector<float> res;
        auto r = create()
            | query("select salary from person")
            | step([&](float salary){ return res.emplace_back(salary); })
            | step([&](float salary){ return res.emplace_back(salary); })
            ;
        BOOST_TEST(r);
        BOOST_TEST((res == std::vector<float>{1000.00, 1200.00}));
        BOOST_TEST(r.conn());
        BOOST_TEST(r.conn()->get());
        BOOST_TEST(r.get_stmt());
        BOOST_TEST(r.get_stmt()->get());
    }
    
    return boost::report_errors();
}
