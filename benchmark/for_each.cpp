#include <benchmark/benchmark.h>
#include <iostream>
#include <msqlite/open.hpp>
#include <msqlite/exec.hpp>
#include <msqlite/for_each.hpp>
#include <msqlite/query.hpp>

namespace sql = msqlite;
using namespace std;

std::size_t i0{};

static void wmsqlite(benchmark::State& state) {
    auto conn = *sql::open("/tmp/msqlite_exec.db");
    sql::exec(conn,
              "create table person(name TEXT);"
              "insert into person values('abc');"
              "insert into person values('def');");
    for(auto _ : state) {
        auto r = sql::query(conn, "select name from person");
        if(r) {
            auto r2 = sql::for_each(*r, [](string_view s){ ++i0; });
            if(!r2) cout << r2.error().value() << endl;
        }
    }
}

inline int query(sqlite3* db, const char* s, sqlite3_stmt*& stmt) {
    if(auto ec = sqlite3_prepare_v2(
           db,
           s,
           -1,
           &stmt,
           nullptr))
    {
        sqlite3_finalize(stmt);
        return ec;
    }
    return 0;    
}

std::size_t i{};

inline void f(const char* s)
{ ++i; }
    
static void wsqlite3(benchmark::State& state) {
    auto conn = *sql::open("/tmp/c_exec.db");
    sql::exec(conn,
              "create table person(name TEXT);"
              "insert into person values('abc');"
              "insert into person values('def');");
    for(auto _ : state) {
        sqlite3_stmt* stmt;
        auto err = query(conn.get(), "select name from person", stmt);
        if(!err) {
            int res;
            do {
                res = sqlite3_step(stmt);
                if(res == SQLITE_ROW) {
                    auto p = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
                    f(p ? p : "");
                } else break;
            } while (true);
            sqlite3_finalize(stmt);
            if(res == SQLITE_DONE) continue;
            cout << res << endl;
        } else sqlite3_finalize(stmt);
    }
}

BENCHMARK(wsqlite3);
BENCHMARK(wmsqlite);

BENCHMARK_MAIN();
