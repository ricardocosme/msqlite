#include <boost/core/lightweight_test.hpp>
#include <msqlite.hpp>

using namespace std;
namespace sql = msqlite;

int main(){
    {
        auto c = sql::open();
        BOOST_TEST(c);
        BOOST_TEST(sql::exec(*c, "create table person(name TEXT)"));
    }
    {
        auto c = sql::open();
        BOOST_TEST(c);
        BOOST_TEST(sql::exec(*c, "create table person(name TEXT);"
                        "insert into person values('abc')"));
    }
    {
        auto c = sql::open();
        BOOST_TEST(c);
        BOOST_TEST(!sql::exec(*c, "invalid"));
    }
    {
        auto c = sql::open();
        BOOST_TEST(c);
        BOOST_TEST(!sql::exec(*c, "create table person(name TEXT);"
                        "snd_stmt_invalid"));
    }
    {
        auto c = sql::open();
        BOOST_TEST(c);
        BOOST_TEST(sql::exec(*c, "create table person(name TEXT UNIQUE)"));
        auto s = sql::prepare(*c, "insert or fail into person values(?)");
        BOOST_TEST(s);
        BOOST_TEST(sql::exec(*s, "abc"));
        BOOST_TEST(sql::exec(*s, "def"));
        BOOST_TEST(!sql::exec(*s, "abc"));
    }
    {
        auto r = sql::open()
            | sql::exec("create table person(name TEXT)");

        BOOST_TEST(r);
        BOOST_TEST(r->get());
    }
    {
        auto r = sql::open()
            | sql::exec("create table person(name TEXT);"
                   "insert into person values('abc')");

        BOOST_TEST(r);
        BOOST_TEST(r->get());
    }
    {
        auto r = sql::open()
            | sql::exec("invalid statement");

        BOOST_TEST(!r);
        BOOST_TEST(r.error() == sql::error::error);
    }
    {
        auto r = sql::open()
            | sql::exec("create table person(name TEXT);"
                   "invalid");

        BOOST_TEST(!r);
        BOOST_TEST(r.error() == sql::error::error);
    }
    {
        auto r = sql::open()
            | sql::exec("create table person(name TEXT)")
            | sql::exec("insert into person values('abc')");

        BOOST_TEST(r);
        BOOST_TEST(r->get());
    }
    {
        auto r = sql::open()
            | sql::exec("invalid statement")
            | sql::exec("insert into person values('abc')");

        BOOST_TEST(!r);
        BOOST_TEST(r.error() == sql::error::error);
    }
    {
        auto r = sql::open()
            | sql::exec("insert into person values('abc')")
            | sql::exec("invalid statement");

        BOOST_TEST(!r);
        BOOST_TEST(r.error() == sql::error::error);
    }

    //lvalue db
    {
        auto conn = sql::open();
        auto r = conn
            | sql::exec("create table person(name TEXT)");

        BOOST_TEST(r);
        BOOST_TEST(conn->get());
        BOOST_TEST((*r)->get());
    }
    {
        auto conn = sql::open();
        auto r = conn
            | sql::exec("invalid statement");

        BOOST_TEST(!r);
        BOOST_TEST(conn->get());
        BOOST_TEST(r.error() == sql::error::error);
    }
    {
        auto conn = sql::open();
        auto lv = conn
            | sql::exec("create table person(name TEXT)");
        auto r = lv | sql::exec("insert into person values('abc')");
        
        BOOST_TEST(r);
        BOOST_TEST(conn->get());
        BOOST_TEST((*r)->get());
    }
    {
        auto conn = sql::open();
        auto r = conn
            | sql::exec("create table person(name TEXT)")
            | sql::exec("insert into person values('abc')");
        
        BOOST_TEST(r);
        BOOST_TEST(conn->get());
        BOOST_TEST((*r)->get());
    }
    
    {
        auto conn = sql::open();
        auto r = conn
            | sql::exec("create table person(name TEXT)")
            | sql::exec("insert into person values('abc')");

        BOOST_TEST(r);
        BOOST_TEST(conn->get());
        BOOST_TEST((*r)->get());
    }
    
    {
        auto conn = sql::open();
        auto r = conn
            | sql::exec("invalid statement")
            | sql::exec("insert into person values('abc')");

        BOOST_TEST(!r);
        BOOST_TEST(conn->get());
        BOOST_TEST(r.error() == sql::error::error);
    }
    
    {
        auto conn = sql::open();
        auto r = conn
            | sql::exec("insert into person values('abc')")
            | sql::exec("invalid statement");

        BOOST_TEST(!r);
        BOOST_TEST(conn->get());
        BOOST_TEST(r.error() == sql::error::error);
    }
    //bind of one string_view
    {
        std::string_view s("abc");
        auto db = sql::open()
            | sql::exec("create table t(col TEXT UNIQUE)")
            | sql::exec("insert or fail into t values(?)", s);
        BOOST_TEST(db);
        auto r = db | sql::exec("insert or fail into t values(?)", s);
        BOOST_TEST(r.error() == sql::error::constraint);
    }
    //bind of one rvalue std::string
    {
        std::cout << "here" << std::endl;
        auto db = sql::open()
            | sql::exec("create table t(col TEXT UNIQUE)")
            | sql::exec("insert or fail into t values(?)", std::string("abc"));
        BOOST_TEST(db);
        auto r = db | sql::exec("insert or fail into t values(?)", std::string("abc"));
        BOOST_TEST(r.error() == sql::error::constraint);
    }
    //bind of one lvalue std::string
    {
        std::string s("abc");
        auto db = sql::open()
            | sql::exec("create table t(col TEXT UNIQUE)")
            | sql::exec("insert or fail into t values(?)", s);
        BOOST_TEST(db);
        auto r = db | sql::exec("insert or fail into t values(?)", s);
        BOOST_TEST(r.error() == sql::error::constraint);
    }
    //bind of one float
    {
        float v = 5.6;
        auto db = sql::open()
            | sql::exec("create table t(col REAL UNIQUE)")
            | sql::exec("insert or fail into t values(?)", v);
        BOOST_TEST(db);
        auto r = db | sql::exec("insert or fail into t values(?)", v);
        BOOST_TEST(r.error() == sql::error::constraint);
    }
    //bind of one double
    {
        double v = 5.6;
        auto db = sql::open()
            | sql::exec("create table t(col REAL UNIQUE)")
            | sql::exec("insert or fail into t values(?)", v);
        BOOST_TEST(db);
        auto r = db | sql::exec("insert or fail into t values(?)", v);
        BOOST_TEST(r.error() == sql::error::constraint);
    }
    //bind of one string literal
    {
        auto db = sql::open()
            | sql::exec("create table t(col TEXT UNIQUE)")
            | sql::exec("insert or fail into t values(?)", "abc");
        BOOST_TEST(db);
        auto r = db | sql::exec("insert or fail into t values(?)", "abc");
        BOOST_TEST(r.error() == sql::error::constraint);
    }
    //bind of a triple
    {
        std::string_view s("abc");
        float f(5.6);
        double d(7.4);
        auto db = sql::open()
            | sql::exec("create table t(col1 TEXT, col2 REAL, col3 REAL)")
            | sql::exec("insert or fail into t values(?,?,?)", s, f, d);
        BOOST_TEST(db);
        auto r = db
            | sql::prepare("select * from t")
            | sql::step([&](std::string_view c1, float c2, double c3){
                       BOOST_TEST(c1 == s);
                       BOOST_TEST(std::abs(c2 - f) < 1e-6);
                       BOOST_TEST(std::abs(c3 - d) < 1e-6);
                   });
        BOOST_TEST(r);
    }
    {
        
        auto db = sql::open("", sql::cache_stmts);
        db | sql::exec("create table person(name TEXT UNIQUE)");
        std::string s{"abc"};
        auto e1 = db | sql::exec("insert or fail into person values(?)", s);
        BOOST_TEST(e1);
        auto e2 = db | sql::exec("insert or fail into person values(?)", s);
        BOOST_TEST(e2.error() == sql::error::constraint);
    }
    
    {
        auto db = sql::open("", sql::cache_stmts);
        db | sql::exec("create table person(name TEXT UNIQUE)");
        std::string s{"abc"};
        auto e1 = db | sql::exec("insert or fail into person values(?)", s);
        BOOST_TEST(e1);
        s = "def";
        auto e2 = db | sql::exec("insert or fail into person values(?)", s);
        BOOST_TEST(e2);
    }
    return boost::report_errors();
}
