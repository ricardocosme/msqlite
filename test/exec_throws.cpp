#include <boost/core/lightweight_test.hpp>
#include <msqlite.hpp>

using namespace std;
namespace sql = msqlite::throws;

int main(){
    {
        auto c = sql::open();
        sql::exec(c, "create table person(name TEXT)");
    }
    {
        auto c = sql::open();
        sql::exec(c, "create table person(name TEXT);"
                  "insert into person values('abc')");
    }
    {
        auto c = sql::open();
        bool error{};
        try {
            sql::exec(c, "invalid");
        } catch(std::system_error &e)
        { error = true; }
        BOOST_TEST(error);
    }
    {
        auto c = sql::open();
        bool error{};
        try {
            sql::exec(c, "create table person(name TEXT);"
                      "snd_stmt_invalid");
        } catch(std::system_error &e)
        { error = true; }
        BOOST_TEST(error);
    }
    {
        std::error_code ec;
        auto c = sql::open();
        sql::exec(c, "create table person(name TEXT UNIQUE)");
        auto s = sql::prepare(c, "insert or fail into person values(?)");
        sql::exec(s, "abc");
        sql::exec(s, "def");
        try {
            sql::exec(s, "abc");
        } catch(std::system_error &e)
        { ec = e.code(); }
        BOOST_TEST(ec == sql::error::constraint);
    }
    return boost::report_errors();
}
