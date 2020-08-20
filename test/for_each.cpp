#include <boost/core/lightweight_test.hpp>
#include <msqlite/open.hpp>
#include <msqlite/exec.hpp>
#include <msqlite/for_each.hpp>
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
        BOOST_TEST(for_each(*pstmt, [&](float salary){ res.emplace_back(salary); }));
        BOOST_TEST((res == res_t{1000.00, 1200.00, 1100.00}));
        BOOST_TEST(for_each(*pstmt, [&](float salary){ res.emplace_back(salary); }));
        BOOST_TEST((res == res_t{1000.00, 1200.00, 1100.00, 1000.00, 1200.00, 1100.00}));
    }

    //success with multiple rows
    {
        using res_t = std::vector<float>;
        res_t res;
        auto r = create()
            | query("select salary from person")
            | for_each([&](float salary){ res.emplace_back(salary); })
            ;
        BOOST_TEST(r);
        BOOST_TEST(r->get());
        BOOST_TEST((res == res_t{1000.00, 1200.00, 1100.00}));
    }

    //invalid query as an input
    {
        using res_t = std::vector<float>;
        res_t res;
        auto r = create()
            | query("invalid")
            | for_each([&](float salary){ res.emplace_back(salary); })
            ;
        BOOST_TEST(!r);
        BOOST_TEST(r.error() == error::error);
    }
    
    //success with a single row
    {
        using res_t = std::vector<float>;
        res_t res;
        auto r = create()
            | query("select salary from person where name = 'def'")
            | for_each([&](float salary){ res.emplace_back(salary); })
            ;
        BOOST_TEST(r);
        BOOST_TEST(r->get());
        BOOST_TEST((res == res_t{1200.00}));
    }

    //empty result
    {
        using res_t = std::vector<float>;
        res_t res;
        auto r = create()
            | query("select salary from person where name = 'not_exist'")
            | for_each([&](float salary){ res.emplace_back(salary); })
            ;
        BOOST_TEST(r);
        BOOST_TEST((res == res_t{}));
    }
    
    //success with multiple rows followed by an exec
    {
        using res_t = std::vector<float>;
        res_t res;
        auto r = create()
            | query("select salary from person")
            | for_each([&](float salary){ res.emplace_back(salary); })
            | exec("delete from person")
            ;
        BOOST_TEST(r);
        BOOST_TEST(r->get());
        BOOST_TEST((res == res_t{1000.00, 1200.00, 1100.00}));
    }

    //lvalue db
    {
        using res_t = std::vector<float>;
        res_t res;
        auto conn = create();

        auto r = conn
            | query("select salary from person")
            | for_each([&](float salary){ res.emplace_back(salary); })
            ;
        BOOST_TEST(r);
        BOOST_TEST((*r)->get());
        BOOST_TEST((res == res_t{1000.00, 1200.00, 1100.00}));
    }
    
    //lvalue prep_stmt with rvalue db
    {
        using res_t = std::vector<float>;
        res_t res;
        auto stmt = create()
            | query("select salary from person");
        
        auto r = stmt
            | for_each([&](float salary){ res.emplace_back(salary); })
            ;
        BOOST_TEST(r);
        BOOST_TEST((*r)->get());
        BOOST_TEST((res == res_t{1000.00, 1200.00, 1100.00}));
    }
    
    //lvalue prep_stmt with a pointer to db
    {
        using res_t = std::vector<float>;
        res_t res;
        auto conn = create();

        auto stmt = conn
            | query("select salary from person");
        
        auto r = stmt
            | for_each([&](float salary){ res.emplace_back(salary); })
            ;
        BOOST_TEST(r);
        BOOST_TEST((*r)->get());
        BOOST_TEST((res == res_t{1000.00, 1200.00, 1100.00}));
    }
    
    return boost::report_errors();
}
