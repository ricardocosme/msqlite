#include <benchmark/benchmark.h>
#include <iostream>
#include <msqlite/open.hpp>

namespace sql = msqlite;
using namespace std;

void msqlite_conn() {
    auto conn = sql::open("/tmp/msqlite_conn.db");
    if(!conn) cout << conn.error().value() << endl;
}

static void wmsqlite(benchmark::State& state) {
    for(auto _ : state)
        msqlite_conn();
}

void c_conn() {
    sqlite3* db;
    auto ec = sqlite3_open("/tmp/c_conn.db", &db);
    if(ec) {
        sqlite3_close(db);
        cout << ec << endl;
        return;
    }
    sqlite3_close(db);
}

static void wsqlite3(benchmark::State& state) {
    for(auto _ : state)
        c_conn();
}

BENCHMARK(wmsqlite);
BENCHMARK(wsqlite3);

BENCHMARK_MAIN();
