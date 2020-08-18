#include <boost/core/lightweight_test.hpp>
#include <msqlite/open.hpp>
#include <msqlite/exec.hpp>

using namespace std;
using namespace msqlite;

int main(){
    {
        auto r = open()
            | exec("create table person(name TEXT)");

        BOOST_TEST(r);
        BOOST_TEST(r->get());
    }
    
    {
        auto r = open()
            | exec("invalid statement");

        BOOST_TEST(!r);
        BOOST_TEST(r.error() == error::error);
    }
    
    {
        auto r = open()
            | exec("create table person(name TEXT)")
            | exec("insert into person values('abc')");

        BOOST_TEST(r);
        BOOST_TEST(r->get());
    }
    
    {
        auto r = open()
            | exec("invalid statement")
            | exec("insert into person values('abc')");

        BOOST_TEST(!r);
        BOOST_TEST(r.error() == error::error);
    }
    
    {
        auto r = open()
            | exec("insert into person values('abc')")
            | exec("invalid statement");

        BOOST_TEST(!r);
        BOOST_TEST(r.error() == error::error);
    }

    //lvalue db

    {
        auto conn = open();
        auto r = conn
            | exec("create table person(name TEXT)");

        BOOST_TEST(r);
        BOOST_TEST(conn->get());
        BOOST_TEST((*r)->get());
    }
    
    {
        auto conn = open();
        auto r = conn
            | exec("invalid statement");

        BOOST_TEST(!r);
        BOOST_TEST(conn->get());
        BOOST_TEST(r.error() == error::error);
    }
    
    {
        auto conn = open();
        auto lv = conn
            | exec("create table person(name TEXT)");
        auto r = lv | exec("insert into person values('abc')");
        
        BOOST_TEST(r);
        BOOST_TEST(conn->get());
        BOOST_TEST((*r)->get());
    }
    
    {
        auto conn = open();
        auto r = conn
            | exec("create table person(name TEXT)")
            | exec("insert into person values('abc')");
        
        BOOST_TEST(r);
        BOOST_TEST(conn->get());
        BOOST_TEST((*r)->get());
    }
    
    {
        auto conn = open();
        auto r = conn
            | exec("create table person(name TEXT)")
            | exec("insert into person values('abc')");

        BOOST_TEST(r);
        BOOST_TEST(conn->get());
        BOOST_TEST((*r)->get());
    }
    
    {
        auto conn = open();
        auto r = conn
            | exec("invalid statement")
            | exec("insert into person values('abc')");

        BOOST_TEST(!r);
        BOOST_TEST(conn->get());
        BOOST_TEST(r.error() == error::error);
    }
    
    {
        auto conn = open();
        auto r = conn
            | exec("insert into person values('abc')")
            | exec("invalid statement");

        BOOST_TEST(!r);
        BOOST_TEST(conn->get());
        BOOST_TEST(r.error() == error::error);
    }
    
    return boost::report_errors();
}
