#include <boost/core/lightweight_test.hpp>
#include <msqlite/throws/open.hpp>

using namespace std;
namespace sql = msqlite::throws;

int main(){
    {
        auto conn = sql::open();
        BOOST_TEST(conn.get());
    }
    
    {
        auto conn = sql::open(sql::cache_stmts);
        BOOST_TEST(conn.get());
    }
    
    {
        auto conn = sql::open("");
        BOOST_TEST(conn.get());
    }

    {
        auto conn = sql::open("", sql::cache_stmts);
        BOOST_TEST(conn.get());
    }
    
    {
        std::error_code ec;
        try {
            auto conn = sql::open("///");
        } catch (std::system_error &e)
        { ec = e.code(); }
        BOOST_TEST(ec == sql::error::cantopen);
    }
    
    return boost::report_errors();
}
