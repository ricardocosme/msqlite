#include <boost/core/lightweight_test.hpp>
#include <msqlite.hpp>

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
        auto pstmt = prepare(*conn, "select salary from person");
        BOOST_TEST(pstmt);
        BOOST_TEST(pstmt->get());
        using res_t = std::vector<float>;
        res_t res;
        auto r = step(*pstmt, [&](float salary){ res.emplace_back(salary); });
        BOOST_TEST(r);
        BOOST_TEST((res == res_t{1000.00}));
        auto r2 = reset(*pstmt);
        BOOST_TEST(r2);
        auto r3 = step(*pstmt, [&](float salary){ res.emplace_back(salary); });
        BOOST_TEST(r3);
        BOOST_TEST((res == res_t{1000.00, 1000.00}));
    }

    //TODO: sqlite_reset returning error?

    {
        using res_t = std::vector<float>;
        res_t res;
        auto r = create()
            | prepare("select salary from person")
            | step([&](float salary){ res.emplace_back(salary); })
            | reset()
            | step([&](float salary){ res.emplace_back(salary); })
            ;
        BOOST_TEST(r.conn());
        BOOST_TEST(r.conn()->get());
        BOOST_TEST(r.get_stmt());
        BOOST_TEST(r.get_stmt()->get());
        BOOST_TEST((res == res_t{1000.00, 1000.00}));
    }
    
    {
        using res_t = std::vector<float>;
        res_t res;
        auto r = create()
            | prepare("select salary from person")
            | step([&](float salary){ res.emplace_back(salary); return salary; })
            | reset()
            | step([&](float salary){ res.emplace_back(salary); return salary; })
            ;
        BOOST_TEST(r);
        BOOST_TEST(*r == 1000.00);
        BOOST_TEST(r.conn());
        BOOST_TEST(r.conn()->get());
        BOOST_TEST(r.get_stmt());
        BOOST_TEST(r.get_stmt()->get());
        BOOST_TEST((res == res_t{1000.00, 1000.00}));
    }
    
    {
        using res_t = std::vector<float>;
        res_t res;
        auto s = create()
            | prepare("select salary from person")
            | step([&](float salary){ res.emplace_back(salary); });
        auto r = s | reset() | step([&](float salary){ res.emplace_back(salary); });
        BOOST_TEST((res == res_t{1000.00, 1000.00}));
        BOOST_TEST(r.conn());
        BOOST_TEST((*r.conn())->get());
        BOOST_TEST(r.get_stmt());
        BOOST_TEST((*r.get_stmt())->get());
    }
    
    {
        using res_t = std::vector<float>;
        res_t res;
        auto conn = create();
        auto r = conn
            | prepare("select salary from person")
            | step([&](float salary){ res.emplace_back(salary); })
            | reset()
            | step([&](float salary){ res.emplace_back(salary); })
            ;
        BOOST_TEST((res == res_t{1000.00, 1000.00}));
        BOOST_TEST(r.conn());
        BOOST_TEST((*r.conn())->get());
        BOOST_TEST(r.get_stmt());
        BOOST_TEST(r.get_stmt()->get());
    }
    
    {
        using res_t = std::vector<float>;
        res_t res;
        auto conn = create();
        auto stmt = conn
            | prepare("select salary from person");
        auto r = stmt
            | step([&](float salary){ res.emplace_back(salary); })
            | reset()
            | step([&](float salary){
                       res.emplace_back(salary);
                       return salary;
                   })
            | reset()
            ;
        BOOST_TEST(r);
        BOOST_TEST(*r == 1000.00);
        BOOST_TEST((res == res_t{1000.00, 1000.00}));
        BOOST_TEST(r.conn());
        BOOST_TEST((*r.conn())->get());
        BOOST_TEST(r.get_stmt());
        BOOST_TEST((*r.get_stmt())->get());
    }
    
    return boost::report_errors();
}
