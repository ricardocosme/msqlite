#include <boost/core/lightweight_test.hpp>
#include <msqlite/open.hpp>
#include <msqlite/query.hpp>
#include <msqlite/stmt.hpp>

#include <iostream>
using namespace std;
using namespace msqlite;
int main(){
    //default ctor
    {
        stmt s;
        BOOST_TEST(s.get() == nullptr);
        BOOST_TEST(s == stmt{});
    }
    
    //finalize the stmt when the object is destroyed
    {
        auto c = open();
        BOOST_TEST(c);
        auto s = query(*c, "create table t(col TEXT)");
        BOOST_TEST(s);
        auto ps = s->release();
        {
            stmt s{ps};
            BOOST_TEST(s.get() != nullptr);
            BOOST_TEST(s.get() == ps);
            BOOST_TEST(s != stmt{});
        }
    }

    //should be ok if the stmts had been finalized before the
    //destruction the object
    {
        auto c = open();
        BOOST_TEST(c);
        auto s = query(*c, "create table t(col TEXT)");
        BOOST_TEST(s);
        auto ps = s->release();
        {
            stmt s{ps};
            BOOST_TEST(s.get() != nullptr);
            BOOST_TEST(s.get() == ps);
        }
    }

    //move ctor
    {
        auto c = open();
        BOOST_TEST(c);
        auto s = query(*c, "create table t(col TEXT)");
        BOOST_TEST(s);
        auto ps = s->release();
        {
            stmt s{ps};
            auto s2 = std::move(s);
            BOOST_TEST(s.get() == nullptr);
            BOOST_TEST(s2.get() == ps);
        }
    }
    
    //move assignment op
    {
        auto c = open();
        BOOST_TEST(c);
        auto s = query(*c, "create table t(col TEXT)");
        BOOST_TEST(s);
        auto ps = s->release();
        auto c2 = open();
        BOOST_TEST(c2);
        auto s2 = query(*c2, "create table t(col TEXT)");
        BOOST_TEST(s2);
        auto ps2 = s2->release();
        {
            stmt s{ps};
            stmt s2{ps2};
            BOOST_TEST(s != s2);
            s2 = std::move(s);
            BOOST_TEST(s.get() == nullptr);
            BOOST_TEST(s2.get() == ps);
        }
    }

    //release
    {
        auto c = open();
        BOOST_TEST(c);
        auto s = query(*c, "create table t(col TEXT)");
        BOOST_TEST(s);
        auto ps = s->release();
        {
            stmt s{ps};
            sqlite3_finalize(s.release());
            BOOST_TEST(s.get() == nullptr);
        }
    }
    
    //explicit finalize
    {
        auto c = open();
        BOOST_TEST(c);
        auto s = query(*c, "create table t(col TEXT)");
        BOOST_TEST(s);
        auto ps = s->release();
        {
            stmt s{ps};
            s.finalize();
            BOOST_TEST(s.get() == nullptr);
        }
    }
    
    //reset with nullptr
    {
        auto c = open();
        BOOST_TEST(c);
        auto s = query(*c, "create table t(col TEXT)");
        BOOST_TEST(s);
        auto ps = s->release();
        {
            stmt s{ps};
            s.reset();
            BOOST_TEST(s.get() == nullptr);
        }
    }

    //reset with othe db
    {
        auto c = open();
        BOOST_TEST(c);
        auto s = query(*c, "create table t(col TEXT)");
        BOOST_TEST(s);
        auto ps = s->release();
        {
            stmt s{ps};
            auto c2 = open();
            BOOST_TEST(c2);
            auto s2 = query(*c2, "create table t(col TEXT)");
            BOOST_TEST(s2);
            auto ps2 = s2->release();
            s.reset(ps2);
            BOOST_TEST(s.get() == ps2);
        }
    }
    
    return boost::report_errors();
}
