#include <benchmark/benchmark.h>
#include <iostream>
#include <msqlite/open.hpp>
#include <msqlite/exec.hpp>
#include <msqlite/prepare.hpp>

namespace sql = msqlite;
using namespace std;

static void wmsqlite(benchmark::State& state) {
    auto conn = *sql::open("/tmp/msqlite_exec.db");
    sql::exec(conn,
              "create table person(name TEXT);"
              "insert into person values('abc');"
              "insert into person values('def');");
    for(auto _ : state) {
        auto r = sql::prepare(conn, "select name from person");
        if(!r) cout << r.error().value() << endl;
    }
}

inline int prepare(sqlite3* db, const char* s, sqlite3_stmt*& stmt) {
    if(auto rc = sqlite3_prepare_v2(
           db,
           s,
           -1,
           &stmt,
           nullptr))
    {
        sqlite3_finalize(stmt);
        return rc;
    }
    return 0;    
}

static void wsqlite3(benchmark::State& state) {
    auto conn = *sql::open("/tmp/c_exec.db");
    sql::exec(conn,
              "create table person(name TEXT);"
              "insert into person values('abc');"
              "insert into person values('def');");
    for(auto _ : state) {
        sqlite3_stmt* stmt;
        auto r = prepare(conn.get(), "select name from person", stmt);
        if(r) cout << r << endl;
        sqlite3_finalize(stmt);
    }
}

BENCHMARK(wsqlite3);
BENCHMARK(wmsqlite);

BENCHMARK_MAIN();
