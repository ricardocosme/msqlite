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

#include <SQLiteWrapper.h>

std::string prepare() {
    return std::string{"select name from person"};
}

static void wsqlite_wrapper(benchmark::State& state) {
    static const char filename[] = "/tmp/sqlite_wrapper_for_each.db";
    select(filename);
    
    using db = sqlite::Database<filename>;

    for(auto _ : state) {
        db::PrepareResult fetch_row = db::prepare<prepare>();
        std::string_view name;
        while(fetch_row(name)) {
            benchmark::DoNotOptimize(name);
        }
    }
    std::filesystem::remove(filename);
}

BENCHMARK(wsqlite_wrapper);

struct person {
    std::string name;
};

#include <sqlite_orm/sqlite_orm.h>

using namespace sqlite_orm;

static void wsqlite_orm(benchmark::State& state) {
    auto filename = "/tmp/sqlite_orm_for_each.db";
    auto storage = make_storage(
        filename,
        make_table("person",
                   make_column("name", &person::name)));

    storage.sync_schema();
    storage.remove_all<person>();  //  remove all old employees in case they exist in db..
    storage.open_forever();
    
    person jimmy{"jimmy"};
    person john{"john"};
    person robert{"robert"};

    storage.insert(jimmy);
    storage.insert(john);
    storage.insert(robert);

    auto stmt = storage.prepare(select(columns(&person::name)));
    for(auto _ : state) {
        try {
            auto rows = storage.execute(stmt);
            for(auto& row : rows) { benchmark::DoNotOptimize(row); }
        } catch(const std::system_error& e)
        { std::cout << e.code() << std::endl; }
    }
    std::filesystem::remove(filename);
}

BENCHMARK(wsqlite_orm);

BENCHMARK_MAIN();
