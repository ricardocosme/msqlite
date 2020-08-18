#include <boost/core/lightweight_test.hpp>
#include <msqlite/db.hpp>

#include <iostream>
using namespace std;
using namespace msqlite;
int main(){
    //default ctor
    {
        db c;
        BOOST_TEST(c.get() == nullptr);
        BOOST_TEST(c == db{});
    }
    
    //close the db when 'db' is destroyed
    {
        sqlite3* pdb;
        sqlite3_open("", &pdb);
        {
            db c{pdb};
            BOOST_TEST(c.get() != nullptr);
            BOOST_TEST(c.get() == pdb);
            BOOST_TEST(c != db{});
        }
    }

    //should be ok if the dbs had been closed before the
    //destruction of 'db'
    {
        sqlite3* pdb;
        sqlite3_open("", &pdb);
        {
            db c{pdb};
            BOOST_TEST(c.get() != nullptr);
            BOOST_TEST(c.get() == pdb);
        }
    }

    //move ctor
    {
        sqlite3* pdb;
        sqlite3_open("", &pdb);
        {
            db c{pdb};
            auto c2 = std::move(c);
            BOOST_TEST(c.get() == nullptr);
            BOOST_TEST(c2.get() == pdb);
        }
    }
    
    //move assignment op
    {
        sqlite3 *pdb, *pdb2;
        sqlite3_open("", &pdb);
        sqlite3_open("", &pdb2);
        {
            db c{pdb};
            db c2{pdb2};
            BOOST_TEST(c != c2);
            c2 = std::move(c);
            BOOST_TEST(c.get() == nullptr);
            BOOST_TEST(c2.get() == pdb);
        }
    }

    //release
    {
        sqlite3* pdb;
        sqlite3_open("", &pdb);
        {
            db c{pdb};
            sqlite3_close(c.release());
            BOOST_TEST(c.get() == nullptr);
        }
    }

    //explicit close
    {
        sqlite3* pdb;
        sqlite3_open("", &pdb);
        {
            db c{pdb};
            c.close();
            BOOST_TEST(c.get() == nullptr);
        }
    }

    //reset with nullptr
    {
        sqlite3* pdb;
        sqlite3_open("", &pdb);
        {
            db c{pdb};
            c.reset();
            BOOST_TEST(c.get() == nullptr);
        }
    }

    //reset with othe db
    {
        sqlite3* pdb;
        sqlite3_open("", &pdb);
        {
            db c{pdb};
            sqlite3* pdb2;
            sqlite3_open("", &pdb2);
            c.reset(pdb2);
            BOOST_TEST(c.get() == pdb2);
        }
    }
    
    return boost::report_errors();
}
