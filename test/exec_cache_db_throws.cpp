#include <boost/core/lightweight_test.hpp>
#include <msqlite.hpp>

using namespace std;
namespace sql = msqlite::throws;

int main(){
    {
        auto c = sql::open(sql::cache_stmts);
        sql::exec(c, "create table person(name TEXT)");
    }
    {
        auto c = sql::open(sql::cache_stmts);
        sql::exec(c, "create table person(name TEXT);"
                  "insert into person values('abc')");
    }
    {
        auto c = sql::open(sql::cache_stmts);
        std::error_code ec;
        try {
            sql::exec(c, "invalid");
        } catch(std::system_error& e)
        { ec = e.code(); }
        BOOST_TEST(ec == sql::error::error);
    }
    //Note: There isn't support to multiple statements using only one
    //call to exec(cache::db, ....).
    return boost::report_errors();
}
