#include <benchmark/benchmark.h>
#include <iostream>
#include <msqlite/open.hpp>
#include <msqlite/exec.hpp>

namespace sql = msqlite;
using namespace std;

static void wmsqlite(benchmark::State& state) {
    auto conn = *sql::open("/tmp/msqlite_exec.db");
    sql::exec(conn, "create table person(name TEXT)");
    for(auto _ : state) {
        auto r = sql::exec(conn, "insert into person values('abc')");
        if(!r) cout << r.error().value() << endl;
    }
}

int exec(sqlite3* db, const char* s) {
    char* emsg;
    if(auto ec = sqlite3_exec(
           db,
           "insert into person values('abc')",
           nullptr,
           nullptr,
           &emsg))
    {
        sqlite3_free(emsg);
        return ec;
    }
    return 0;    
}

static void wsqlite3(benchmark::State& state) {
    auto conn = *sql::open("/tmp/c_exec.db");
    sql::exec(conn, "create table person(name TEXT)");
    for(auto _ : state) {
        auto r = exec(conn.get(), "create table person(name TEXT)");
        if(r) cout << r << endl;
    }
}

BENCHMARK(wsqlite3);
BENCHMARK(wmsqlite);

BENCHMARK_MAIN();
