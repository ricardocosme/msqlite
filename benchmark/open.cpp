#include <benchmark/benchmark.h>
#include <filesystem>
#include <iostream>
#include <msqlite.hpp>

namespace sql = msqlite;
using namespace std;

auto msqlite_path = "/tmp/msqlite_conn.db";

void msqlite_conn() {
    auto conn = sql::open(msqlite_path);
    if(!conn) cout << conn.error().value() << endl;
}

static void wmsqlite(benchmark::State& state) {
    for(auto _ : state)
        msqlite_conn();
    std::filesystem::remove(msqlite_path);
}

BENCHMARK(wmsqlite);

auto sqlite_path = "/tmp/sqlite_conn.db";

void c_conn() {
    sqlite3* db;
    auto rc = sqlite3_open(sqlite_path, &db);
    if(rc) {
        sqlite3_close(db);
        cout << rc << endl;
        return;
    }
    sqlite3_close(db);
}

static void wsqlite(benchmark::State& state) {
    for(auto _ : state)
        c_conn();
    std::filesystem::remove(sqlite_path);
}

BENCHMARK(wsqlite);

BENCHMARK_MAIN();
