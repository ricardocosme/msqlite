#include <benchmark/benchmark.h>
#include <iostream>
#include <sqlite_orm/sqlite_orm.h>
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
              "create table person(name TEXT, salary REAL);"
              "insert into person values('abc', 10000.00);"
              "insert into person values('def', 11000.00);");
    for(auto _ : state) {
        auto r = sql::query(conn, "select name, salary from person");
        if(r) {
            auto r2 = sql::for_each(*r, [](string_view s, float salary){ ++i0; });
            if(!r2) cout << r2.error().value() << endl;
        }
    }
}

std::size_t i{};

struct person {
    std::string name;
    float salary;
};

using namespace sqlite_orm;

static void wsqlite_orm(benchmark::State& state) {
    auto storage = make_storage("/tmp/sqlite_orm.db",
                                make_table("person",
                                           make_column("NAME", &person::name),
                                           make_column("SALARY", &person::salary)));

    storage.sync_schema();
    storage.remove_all<person>();  //  remove all old employees in case they exist in db..

    person jimmy{"jimmy", 10000.00};
    person john{"john", 9500.00};
    person robert{"robert", 11000.00};

    //  insert employees. `insert` function returns id of inserted object..
    storage.insert(jimmy);
    storage.insert(john);
    storage.insert(robert);

    for(auto _ : state) {
        try {
            auto rows = storage.select(columns(&person::name, &person::salary));
            for(auto& row : rows)
                ++i;
        } catch(const std::system_error& e)
        { std::cout << e.code() << std::endl; }
    }
}

BENCHMARK(wsqlite_orm);
BENCHMARK(wmsqlite);

BENCHMARK_MAIN();
