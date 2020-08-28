#include <benchmark/benchmark.h>
#include <filesystem>
#include <msqlite.hpp>

namespace sql = msqlite;
using namespace std;

static void nocache(benchmark::State& state) {
    auto path = "/tmp/nocache.db";
    auto conn = sql::open(path)
        | sql::exec("create table person(name TEXT UNIQUE)");
    for(auto _ : state) {
        conn | sql::exec("insert into person values(?)", "abc");
        conn | sql::exec("update person set name = ? where name = ?", "ABC", "abc");
        conn | sql::exec("insert into person values(?)", "ghi");
        conn | sql::exec("select * from person");
        conn | sql::exec("delete from person where name = ?", "abc");
        conn | sql::exec("select * from person where name = ?", "ghi");
        conn | sql::exec("insert or fail into person values(?)", "jkl");
        conn | sql::exec("delete from person");
        conn | sql::exec("insert or fail into person values(?)", "JKL");
        conn | sql::exec("insert or fail into person values(?)", "QWE");
        conn | sql::exec("insert or fail into person values(?)", "TOO");
        conn | sql::exec("delete from person");
        conn | sql::exec("insert    into person values(?)", "foo");
        conn | sql::exec("insert or fail into person values(?)", "789");
        conn | sql::exec("insert or fail into person values(?)", "456");
        conn | sql::exec("insert or fail into person values(?)", "123");
        conn | sql::exec("insert  or fail into person values(?)", "000");
    }
    std::filesystem::remove(path);
}

BENCHMARK(nocache);

static void cache_using_db(benchmark::State& state) {
    auto path = "/tmp/cache_using_db.db";
    auto conn = sql::open(path, sql::cache_stmts)
        | sql::exec("create table person(name TEXT UNIQUE)");
    for(auto _ : state) {
        conn | sql::exec("insert into person values(?)", "abc");
        conn | sql::exec("update person set name = ? where name = ?", "ABC", "abc");
        conn | sql::exec("insert into person values(?)", "ghi");
        conn | sql::exec("select * from person");
        conn | sql::exec("delete from person where name = ?", "abc");
        conn | sql::exec("select * from person where name = ?", "ghi");
        conn | sql::exec("insert or fail into person values(?)", "jkl");
        conn | sql::exec("delete from person");
        conn | sql::exec("insert or fail into person values(?)", "JKL");
        conn | sql::exec("insert or fail into person values(?)", "QWE");
        conn | sql::exec("insert or fail into person values(?)", "TOO");
        conn | sql::exec("delete from person");
        conn | sql::exec("insert    into person values(?)", "foo");
        conn | sql::exec("insert or fail into person values(?)", "789");
        conn | sql::exec("insert or fail into person values(?)", "456");
        conn | sql::exec("insert or fail into person values(?)", "123");
        conn | sql::exec("insert  or fail into person values(?)", "000");
    }
    std::filesystem::remove(path);
}

BENCHMARK(cache_using_db);
    
static void cache_by_hand(benchmark::State& state) {
    auto path = "/tmp/cache_by_hand.db";
    auto conn = sql::open("/tmp/cache_by_hand.db")
        | sql::exec("create table person(name TEXT UNIQUE)");
    auto insert_person = conn
        | sql::prepare("insert into person values(?)");
    auto insert2_person = conn
        | sql::prepare("insert or fail into person values(?)");
    auto insert3_person = conn
        | sql::prepare("insert    into person values(?)", "foo");
    auto insert4_person = conn
        | sql::prepare("insert  or fail into person values(?)", "000");
    auto update_person = conn
        | sql::prepare("update person set name = ? where name = ?");
    auto delete_person = conn
        | sql::prepare("delete from person where name = ?");
    auto select_person = conn
        | sql::prepare("select * from person");
    auto select_person_where = conn
        | sql::prepare("select * from person where name = ?");
    auto delete_all_person = conn
        | sql::prepare("delete from person");
    for(auto _ : state) {
        insert_person | sql::exec_with("abc");
        update_person | sql::exec_with("ABC", "abc");
        insert_person | sql::exec_with("ghi");
        select_person | sql::exec();
        delete_person | sql::exec_with("abc");
        select_person_where | sql::exec_with("ghi");
        insert2_person | sql::exec_with("jkl");
        delete_all_person | sql::exec_with();
        insert2_person
            | sql::exec_with("JKL")
            | sql::exec_with("QWE")
            | sql::exec_with("TOO");
        delete_all_person | sql::exec_with();
        insert3_person | sql::exec_with("foo");
        insert2_person
            | sql::exec_with("789")
            | sql::exec_with("456")
            | sql::exec_with("123");
        insert4_person | sql::exec_with("000");
    }
    std::filesystem::remove(path);
}

BENCHMARK(cache_by_hand);

BENCHMARK_MAIN();
