#include <benchmark/benchmark.h>
#include <iostream>
#include <filesystem>
#include <msqlite.hpp>

namespace sql = msqlite;
using namespace std;

auto msqlite_path = "/tmp/msqlite_exec.db";

static void wmsqlite(benchmark::State& state) {
    auto conn = *sql::open(msqlite_path);
    sql::exec(conn, "create table person(name TEXT)");
    for(auto _ : state) {
        auto r = sql::exec(conn, "insert into person values(?)", "abc");
        if(!r) cout << r.error().value() << endl;
    }
    std::filesystem::remove(msqlite_path);
}

BENCHMARK(wmsqlite);

int exec(sqlite3* db, std::string_view s, std::string_view val) {
    sqlite3_stmt* pstmt;
    if(auto rc = sqlite3_prepare_v2(
           db,
           s.data(),
           -1,
           &pstmt,
           nullptr))
    {
        sqlite3_finalize(pstmt);
        return rc;
    }
    sqlite3_bind_text(pstmt, 1, val.data(), val.size(), SQLITE_STATIC);
    if(auto rc = sqlite3_step(pstmt); rc != SQLITE_DONE) {
        sqlite3_finalize(pstmt);
        return rc;
    }
    sqlite3_finalize(pstmt);
    return 0;    
}

auto sqlite_path = "/tmp/sqlite_exec.db";

static void wsqlite(benchmark::State& state) {
    auto conn = *sql::open(sqlite_path);
    sql::exec(conn, "create table person(name TEXT)");
    for(auto _ : state) {
        auto r = exec(conn.get(), "insert into person values(?)", "abc");
        if(r) cout << r << endl;
    }
    std::filesystem::remove(sqlite_path);
}

BENCHMARK(wsqlite);

BENCHMARK_MAIN();
