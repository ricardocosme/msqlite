#include <boost/core/lightweight_test.hpp>
#include <msqlite/open.hpp>
#include <msqlite/exec.hpp>
#include <msqlite/query.hpp>

using namespace std;
using namespace msqlite;

auto create() {
    return open()
        | exec("create table person(name TEXT);"
               "insert into person values('abc');"
               "insert into person values('def');"
               "insert into person values('ghi');");
}

int main(){
    {
        auto conn = create();
        BOOST_TEST(conn);
        auto r = query(*conn, "select name from person");
        BOOST_TEST(r);
        BOOST_TEST(r->get());
    }
    
    {
        auto conn = create();
        BOOST_TEST(conn);
        auto r = query(*conn, "invalid query");
        BOOST_TEST(!r);
        BOOST_TEST(r.error() == error::error);
    }
    
    {
        auto r = create()
            | query("select name from person");
        BOOST_TEST(r.get_stmt());
        BOOST_TEST(r.conn());
        BOOST_TEST(r.conn()->get());
        BOOST_TEST(r.get_stmt()->get());
    }
    
    {
        auto r = create()
            | query("invalid stmt");

        BOOST_TEST(!r.get_stmt());
        BOOST_TEST(r.get_stmt().error() == error::error);
    }
        
    //lvalue db

    {
        auto conn = create();
        auto r = conn
            | query("select name from person");

        BOOST_TEST(r.get_stmt());
        BOOST_TEST(conn->get());
        BOOST_TEST(r.conn());
        BOOST_TEST((*r.conn())->get());
    }
    
    {
        auto conn = create();
        auto r = conn
            | query("invalid");

        BOOST_TEST(!r.get_stmt());
        BOOST_TEST(r.get_stmt().error() == error::error);
        BOOST_TEST(r.conn());
        BOOST_TEST((*r.conn())->get());
    }

    {
        auto conn = create();
        auto lv = conn | exec("insert into person values('jkl')");
        auto r = lv
            | query("select name from person");

        BOOST_TEST(r.get_stmt());
        BOOST_TEST(conn->get());
        BOOST_TEST(r.conn());
        BOOST_TEST((*r.conn())->get());
    }
    
    {
        auto conn = create();
        auto r = conn
            | exec("insert into person values('jkl')")
            | query("select name from person");

        BOOST_TEST(r.get_stmt());
        BOOST_TEST(conn->get());
        BOOST_TEST(r.conn());
        BOOST_TEST((*r.conn())->get());
    }
    
    return boost::report_errors();
}
