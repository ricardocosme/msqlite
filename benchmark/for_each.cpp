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
    | sql::query("select name from person");
}

static void wmsqlite(benchmark::State& state) {
    auto filename = "/tmp/msqlite_for_each.db";
    auto ctx = select(filename);
    auto& stmt = ctx.get_stmt();
    for(auto _ : state) {
        auto r2 = sql::for_each(*stmt, [](string_view s)
        {
            benchmark::DoNotOptimize(s);
        });
        if(!r2) cout << r2.error().value() << endl;
    }
    std::filesystem::remove(filename);
}

BENCHMARK(wmsqlite);

inline void f(const char* s)
{ benchmark::DoNotOptimize(s); }
    
static void wsqlite3(benchmark::State& state) {
    auto filename = "/tmp/sqlite3_for_each.db";
    auto ctx = select(filename);
    auto stmt = ctx.get_stmt()->get();
    for(auto _ : state) {
        int res;
        do {
            res = sqlite3_step(stmt);
            if(res == SQLITE_ROW) {
                auto p = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
                f(p ? p : "");
            } else break;
        } while (true);
        if(res == SQLITE_DONE) {
            sqlite3_reset(stmt);
            continue;
        }
        cout << res << endl;
    }
    std::filesystem::remove(filename);
}

BENCHMARK(wsqlite3);

BENCHMARK_MAIN();
