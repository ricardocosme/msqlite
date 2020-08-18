#include <iostream>
#include <boost/core/lightweight_test.hpp>
#include <msqlite/open.hpp>
#include <msqlite/query.hpp>
#include <msqlite/exec.hpp>

using namespace std;
namespace sql = msqlite;
using namespace sql;

inline auto db() {
    return open()
        .exec("create table person(name TEXT, salary REAL);"
              "insert into person values ('joao', 10000.00);"
              "insert into person values ('maria', 13000.00)");
}

struct fp { float v; };

bool operator==(fp lhs, fp rhs)
{ return std::abs(lhs.v - rhs.v) < 10e-3; }

bool operator==(fp lhs, float rhs)
{ return std::abs(lhs.v - rhs) < 10e-3; }

bool operator==(float lhs, fp rhs)
{ return std::abs(lhs - rhs.v) < 10e-3; }

bool operator!=(fp lhs, float rhs)
{ return !(lhs == rhs); }

bool operator!=(float lhs, fp rhs)
{ return !(lhs == rhs); }

using rows = std::vector<std::pair<std::string, float>>;

int main(){
    //db as rvalue
    {
        rows v;
        bool error{};
        auto conn = db();
        std::move(conn)
        .query("select name, salary from person")
        .for_each([&](string name, float salary) {
                      v.emplace_back(name, salary);
                  })
        .onerror([&](auto e){ error = true; });
        BOOST_TEST(conn);
        BOOST_TEST(conn->get() == nullptr);
        BOOST_TEST((v == rows{{"joao", 10000.00}, {"maria", 13000.00}}));
        BOOST_TEST(!error);
    }
    
    //db as lvalue
    {
        rows v;
        bool error{};
        auto conn = db();
        
        conn
        .query("select name, salary from person")
        .for_each([&](string name, float salary) {
                      v.emplace_back(name, salary);
                  })
        .onerror([&](auto e){ error = true; });

        BOOST_TEST(conn);
        BOOST_TEST(conn->get() != nullptr);
        BOOST_TEST((v == rows{{"joao", 10000.00}, {"maria", 13000.00}}));
        BOOST_TEST(!error);
    }

    //onerror after for_each
    {
        rows v;
        bool called{};
        db()
        .query("INVALID_STMT name, salary from person")
        .for_each([&](string name, float salary) { v.emplace_back(name, salary); })
        .onerror([&](auto e){ BOOST_TEST(e == error::error); called = true; });
        BOOST_TEST(v.empty());
        BOOST_TEST(called);
    }

    //onerror after rv exec
    {
        bool called{};
        db()
        .exec("INVALID_STMT name, salary from person")
        .onerror([&](auto e){ BOOST_TEST(e == error::error); called = true; });
        BOOST_TEST(called);
    }
    
    //onerror after lv exec
    {
        bool called{};
        auto ex = db()
        .exec("INVALID_STMT");
        ex.onerror([&](auto e){ BOOST_TEST(e == error::error); called = true; });
        BOOST_TEST(called);
    }
    
    //exec(rv) -> exec
    {
        rows v;
        bool error{};
        auto conn = db();
        
        conn
        .exec("delete from person where name = 'joao'")
        .query("select name, salary from person")
        .for_each([&](string name, float salary) { v.emplace_back(name, salary); })
        .onerror([&](auto e){ error = true; });

        BOOST_TEST(conn);
        BOOST_TEST(conn->get() != nullptr);
        BOOST_TEST((v == rows{{"maria", 13000.00}}));
        BOOST_TEST(!error);
    }
    
    //exec(lv) -> exec
    {
        rows v;
        bool error{};
        auto conn = db();
        
        auto ex = conn
        .exec("delete from person where name = 'joao'");
        
        ex.query("select name, salary from person")
        .for_each([&](string name, float salary) { v.emplace_back(name, salary); })
        .onerror([&](auto e){ error = true; });

        BOOST_TEST(conn);
        BOOST_TEST(conn->get() != nullptr);
        BOOST_TEST((v == rows{{"maria", 13000.00}}));
        BOOST_TEST(!error);
    }
    
    //for_each as lvalue
    {
        rows v;
        bool called{};
        auto res = db()
        .query("INVALID_STMT name, salary from person");
        
        res.for_each([&](string name, float salary) { v.emplace_back(name, salary); })
        .onerror([&](auto e){ called = true; });

        BOOST_TEST(res.conn.get() != nullptr);
        BOOST_TEST(v.empty());
        BOOST_TEST(called);
    }

    //rv first
    {
        rows v;
        bool error{};
        
        auto ret = db()
        .query("select name, salary from person")
        .first([&](string name, float salary) {
                   v.emplace_back(name, salary);
                   return std::string{"abc"};
               })
        .onerror([&](auto e){ error = true; return std::string{}; });
        
        BOOST_TEST(ret == std::string{"abc"});
        
        BOOST_TEST((v == rows{{"joao", 10000.00}}));
        BOOST_TEST(!error);
    }
    
    //lv first
    {
        rows v;
        bool error{};
        
        auto ex = db()
        .query("select name, salary from person");
            
        auto ret = ex.first([&](string name, float salary) {
                   v.emplace_back(name, salary);
                   return std::string{"abc"};
               })
        .onerror([&](auto e){ error = true; return std::string{}; });
        
        BOOST_TEST(ret == std::string{"abc"});
        
        BOOST_TEST((v == rows{{"joao", 10000.00}}));
        BOOST_TEST(!error);
    }
    
    //empty first
    {
        rows v;
        bool error{};
        
        auto ret = db()
        .query("select name, salary from person where name='NOT_EXIST'")
        .first([&](string name, float salary) {
                   v.emplace_back(name, salary);
                   return std::string{"abc"};
               })
        .onerror([&](auto e){
                     BOOST_TEST(e.value() == SQLITE_DONE);
                     error = true;
                     return std::string{};
                 });
        
        BOOST_TEST(ret == std::string{});
        
        BOOST_TEST((v == rows{}));
        BOOST_TEST(error);
    }
    
    //lv first with onerror handling a sql error 
    {
        rows v;
        bool error{};
        
        auto ex = db()
        .query("INVALID_STMT name, salary from person")
        .first([&](string name, float salary) {
                   v.emplace_back(name, salary);
                   return std::string{"abc"};
               });
            
        auto ret =
        ex.onerror([&](auto e){
                     error = true;
                     BOOST_TEST(e = error::error);
                     return std::string{"something"};
                   });
        
        BOOST_TEST(ret == std::string{"something"});
        
        BOOST_TEST((v == rows{}));
        BOOST_TEST(error);
    }
    
    //lv first with onerror 
    {
        rows v;
        bool error{};
        
        auto ex = db()
        .query("select name, salary from person")
        .first([&](string name, float salary) {
                   v.emplace_back(name, salary);
                   return std::string{"abc"};
               });
            
        auto ret =
        ex.onerror([&](auto e){
                     error = true;
                     BOOST_TEST(e == error::error);
                     return std::string{"something"};
                   });
        
        BOOST_TEST(ex == std::string{"abc"});
        BOOST_TEST(ret == std::string{"abc"});
        
        BOOST_TEST((v == rows{{"joao", 10000.00}}));
        BOOST_TEST(!error);
    }

    //first returns an error
    {
        auto ex = db()
        .query("select name, salary from person")
        .first([&](string name, float salary) {
                   return std::error_code{1, std::generic_category()};
               });

        BOOST_TEST(!ex);
        BOOST_TEST(ex.error().value() == 1);
    }
    
    //query followed by exec
    {
        rows v;
        bool error{};
        
        db()
        .query("select name, salary from person")
        .for_each([&](string name, float salary) {
                   v.emplace_back(name, salary);
               })
        .exec("delete from person")
        .query("select name, salary from person")
        .for_each([&](string name, float salary) {
                   v.emplace_back(name, salary);
               })
        .onerror([&](auto e){ error = true; });
        
        BOOST_TEST((v == rows{{"joao", 10000.00}, {"maria", 13000.00}}));
        BOOST_TEST(!error);
    }

    // //exec inside for_each 
    // {
    //     rows v;
    //     bool error{};
    //     auto conn = sql::open("/tmp/msqlite_test.db")
    //         .exec("create table person(name TEXT, salary REAL)")
    //         .exec("insert into person values ('joao', 10000.00)");

    //     BOOST_TEST(conn);
    //     conn.query("select name, salary from person")
    //     .first([&](string name, float salary) {
    //                auto conn2 = sql::open("/tmp/msqlite_test.db");
    //                BOOST_TEST(conn2);
    //                BOOST_TEST(exec(*conn2, "update person set name = 'something'"));
    //         v.emplace_back(name, salary);
    //         return 0;
    //     })
    //         .onerror([&](auto e){ error = true; return 1;});
    //     BOOST_TEST((v == rows{{"something", 10000.00}}));
    //     BOOST_TEST(!error);
    // }
    
    {
        db()
            .exec("create table person(id TEXT, name TEXT)")
            .exec("insert into person values ('1', 'joao')")
            .query("select id, name from person")
            .for_each([](string id, string name) {
                          cout << "id=" << id << endl
                               << "name=" << name << endl;
                      })
            .exec("select name from person where name='maria'")
            .onerror([](auto e){ cout << e << endl; });
    }

    return boost::report_errors();
}
