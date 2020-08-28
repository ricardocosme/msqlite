#include <boost/core/lightweight_test.hpp>
#include <msqlite.hpp>

using namespace std;
using namespace msqlite;

int main(){
    {
        auto c = open(cache_stmts);
        BOOST_TEST(c);
        BOOST_TEST(exec(*c, "create table person(name TEXT)"));
    }
    {
        auto c = open(cache_stmts);
        BOOST_TEST(c);
        BOOST_TEST(exec(*c, "create table person(name TEXT);"
                        "insert into person values('abc')"));
    }
    {
        auto c = open(cache_stmts);
        BOOST_TEST(c);
        BOOST_TEST(!exec(*c, "invalid"));
    }

    //Note: There isn't support to multiple statements using only one
    //call to exec(cache::db, ....).
    //
    // {
    //     auto c = open(cache_stmts);
    //     BOOST_TEST(c);
    //     BOOST_TEST(!exec(*c, "create table person(name TEXT);"
    //                     "snd_stmt_invalid"));
    // }
    
    {
        auto r = open(cache_stmts)
            | exec("create table person(name TEXT)");

        BOOST_TEST(r);
        BOOST_TEST(r->get());
    }
    
    {
        auto r = open(cache_stmts)
            | exec("invalid statement");

        BOOST_TEST(!r);
        BOOST_TEST(r.error() == error::error);
    }
    
    {
        auto r = open(cache_stmts)
            | exec("create table person(name TEXT)")
            | exec("insert into person values('abc')");

        BOOST_TEST(r);
        BOOST_TEST(r->get());
    }
    
    {
        auto r = open(cache_stmts)
            | exec("invalid statement")
            | exec("insert into person values('abc')");

        BOOST_TEST(!r);
        BOOST_TEST(r.error() == error::error);
    }
    
    {
        auto r = open(cache_stmts)
            | exec("insert into person values('abc')")
            | exec("invalid statement");

        BOOST_TEST(!r);
        BOOST_TEST(r.error() == error::error);
    }

    //lvalue db

    {
        auto conn = open(cache_stmts);
        auto r = conn
            | exec("create table person(name TEXT)");

        BOOST_TEST(r);
        BOOST_TEST(conn->get());
        BOOST_TEST((*r)->get());
    }
    
    {
        auto conn = open(cache_stmts);
        auto r = conn
            | exec("invalid statement");

        BOOST_TEST(!r);
        BOOST_TEST(conn->get());
        BOOST_TEST(r.error() == error::error);
    }
    
    {
        auto conn = open(cache_stmts);
        auto lv = conn
            | exec("create table person(name TEXT)");
        auto r = lv | exec("insert into person values('abc')");
        
        BOOST_TEST(r);
        BOOST_TEST(conn->get());
        BOOST_TEST((*r)->get());
    }
    
    {
        auto conn = open(cache_stmts);
        auto r = conn
            | exec("create table person(name TEXT)")
            | exec("insert into person values('abc')");
        
        BOOST_TEST(r);
        BOOST_TEST(conn->get());
        BOOST_TEST((*r)->get());
    }
    
    {
        auto conn = open(cache_stmts);
        auto r = conn
            | exec("create table person(name TEXT)")
            | exec("insert into person values('abc')");

        BOOST_TEST(r);
        BOOST_TEST(conn->get());
        BOOST_TEST((*r)->get());
    }
    
    {
        auto conn = open(cache_stmts);
        auto r = conn
            | exec("invalid statement")
            | exec("insert into person values('abc')");

        BOOST_TEST(!r);
        BOOST_TEST(conn->get());
        BOOST_TEST(r.error() == error::error);
    }
    
    {
        auto conn = open(cache_stmts);
        auto r = conn
            | exec("insert into person values('abc')")
            | exec("invalid statement");

        BOOST_TEST(!r);
        BOOST_TEST(conn->get());
        BOOST_TEST(r.error() == error::error);
    }

    //bind of one string_view
    {
        std::string_view s("abc");
        auto db = open(cache_stmts)
            | exec("create table t(col TEXT UNIQUE)")
            | exec("insert or fail into t values(?)", s);
        BOOST_TEST(db);
        auto r = db | exec("insert or fail into t values(?)", s);
        BOOST_TEST(r.error() == error::constraint);
    }
    //bind of one rvalue std::string
    {
        std::cout << "here" << std::endl;
        auto db = open(cache_stmts)
            | exec("create table t(col TEXT UNIQUE)")
            | exec("insert or fail into t values(?)", std::string("abc"));
        BOOST_TEST(db);
        auto r = db | exec("insert or fail into t values(?)", std::string("abc"));
        BOOST_TEST(r.error() == error::constraint);
    }
    //bind of one lvalue std::string
    {
        std::string s("abc");
        auto db = open(cache_stmts)
            | exec("create table t(col TEXT UNIQUE)")
            | exec("insert or fail into t values(?)", s);
        BOOST_TEST(db);
        auto r = db | exec("insert or fail into t values(?)", s);
        BOOST_TEST(r.error() == error::constraint);
    }
    //bind of one float
    {
        float v = 5.6;
        auto db = open(cache_stmts)
            | exec("create table t(col REAL UNIQUE)")
            | exec("insert or fail into t values(?)", v);
        BOOST_TEST(db);
        auto r = db | exec("insert or fail into t values(?)", v);
        BOOST_TEST(r.error() == error::constraint);
    }
    //bind of one double
    {
        double v = 5.6;
        auto db = open(cache_stmts)
            | exec("create table t(col REAL UNIQUE)")
            | exec("insert or fail into t values(?)", v);
        BOOST_TEST(db);
        auto r = db | exec("insert or fail into t values(?)", v);
        BOOST_TEST(r.error() == error::constraint);
    }
    //bind of one string literal
    {
        auto db = open(cache_stmts)
            | exec("create table t(col TEXT UNIQUE)")
            | exec("insert or fail into t values(?)", "abc");
        BOOST_TEST(db);
        auto r = db | exec("insert or fail into t values(?)", "abc");
        BOOST_TEST(r.error() == error::constraint);
    }
    //bind of a triple
    {
        std::string_view s("abc");
        float f(5.6);
        double d(7.4);
        auto db = open(cache_stmts)
            | exec("create table t(col1 TEXT, col2 REAL, col3 REAL)")
            | exec("insert or fail into t values(?,?,?)", s, f, d);
        BOOST_TEST(db);
        auto r = db
            | prepare("select * from t")
            | step([&](std::string_view c1, float c2, double c3){
                       BOOST_TEST(c1 == s);
                       BOOST_TEST(std::abs(c2 - f) < 1e-6);
                       BOOST_TEST(std::abs(c3 - d) < 1e-6);
                   });
        BOOST_TEST(r);
    }
    //change the value to be binded
    {
        auto db = open(cache_stmts);
        db | exec("create table person(name TEXT UNIQUE)");
        std::string s{"abc"};
        auto e1 = db | exec("insert or fail into person values(?)", s);
        BOOST_TEST(e1);
        s = "def";
        auto e2 = db | exec("insert or fail into person values(?)", s);
        BOOST_TEST(e2);
    }
    return boost::report_errors();
}
