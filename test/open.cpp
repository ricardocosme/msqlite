#include <boost/core/lightweight_test.hpp>
#include <msqlite/open.hpp>

using namespace std;
namespace sql = msqlite;

int main(){
    {
        auto conn = sql::open();
        BOOST_TEST(conn);
        BOOST_TEST(conn->get());
    }
    
    {
        auto conn = sql::open("");
        BOOST_TEST(conn);
        BOOST_TEST(conn->get());
    }

    {
        auto conn = sql::open("///");
        BOOST_TEST(!conn);
        BOOST_TEST(conn.error() == sql::error::cantopen);
    }
    
    return boost::report_errors();
}
