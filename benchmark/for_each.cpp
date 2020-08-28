#include <benchmark/benchmark.h>
#include <filesystem>
#include <iostream>
#include <msqlite.hpp>
#include <string_view>

namespace sql = msqlite;
using namespace std;

auto select(std::string_view db) {
    return sql::open(db.data())
    | sql::exec(
        "create table person(name TEXT);"
        "insert into person values('abc');"
        "insert into person values('def');")
    | sql::prepare("select name from person");
}

std::string_view s;

static void wmsqlite(benchmark::State& state) {
    auto filename = "/tmp/msqlite_for_each.db";
    auto ctx = select(filename);
    auto& stmt = ctx.get_stmt();
    for(auto _ : state) {
        auto r2 = sql::for_each(*stmt, [](string_view p){ s = p; });
        if(!r2) cout << r2.error().value() << endl;
    }
    std::filesystem::remove(filename);
}

BENCHMARK(wmsqlite);

std::string_view s2;
inline void f(const char* s)
{ s2 = s; }
    
static void wsqlite(benchmark::State& state) {
    auto filename = "/tmp/sqlite_for_each.db";
    auto ctx = select(filename);
    auto stmt = ctx.get_stmt()->get();
    for(auto _ : state) {
        int rc;
        do {
            rc = sqlite3_step(stmt);
            if(rc == SQLITE_ROW) {
                auto p = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
                f(p ? p : "");
            } else break;
        } while (true);
        if(rc == SQLITE_DONE) {
            sqlite3_reset(stmt);
            continue;
        }
        cout << rc << endl;
    }
    std::filesystem::remove(filename);
}

BENCHMARK(wsqlite);

BENCHMARK_MAIN();
