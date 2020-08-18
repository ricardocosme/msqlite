#pragma once

#include "msqlite/concepts.hpp"
#include "msqlite/db.hpp"
#include "msqlite/stmt.hpp"

#include <system_error>
#include <tl/expected.hpp>

namespace msqlite {

auto& lvref(auto& o) { return o; }
auto& lvref(auto* o) { return *o; }

auto forward(auto&& o) { return std::move(o); }
auto forward(auto& o) { return std::addressof(o); }
auto forward(auto* o) { return o; }

template<typename Db, typename T = void>
class context;

template<typename Exp>
class context<db, Exp> : public Exp {
    result<db> _conn;
    result<stmt> _stmt;
public:
    using base = Exp;
    using base::base;
    
    using db_t = db;
    
    explicit context(result<db> c) : _conn(std::move(c)) {}
    explicit context(result<db> c,
                     result<stmt> s,
                     Exp v = Exp{})
        : base(std::move(v))
        , _conn(std::move(c))
        , _stmt(std::move(s))
    {}
    auto get_stmt() &&
    { return std::move(_stmt); }
    
    auto& get_stmt() &
    { return _stmt; }
    
    auto conn() &&
    { return std::move(_conn); }
    
    auto& conn() &
    { return _conn; }
};

template<>
class context<db, void> {
    result<db> _conn;
    result<stmt> _stmt;
public:
    using db_t = db;
    
    explicit context(result<db> c) : _conn(std::move(c)) {}
    explicit context(result<db> c,
                     result<stmt> s)
        : _conn(std::move(c))
        , _stmt(std::move(s))
    {}
    auto get_stmt() &&
    { return std::move(_stmt); }
    
    auto& get_stmt() &
    { return _stmt; }
    
    auto conn() &&
    { return std::move(_conn); }
    
    auto& conn() &
    { return _conn; }
};

template<>
class context<db*, void> {
    result<db*> _conn;
    result<stmt> _stmt;
public:
    using db_t = db*;
    
    explicit context(result<db*> c) : _conn(std::move(c)) {}
    explicit context(result<db*> c,
                     result<stmt> s)
        : _conn(std::move(c))
        , _stmt(std::move(s))
    {}
    auto& get_stmt()
    { return _stmt; }
    
    auto& conn()
    { return _conn; }
};

template<typename Exp>
class context<db*, Exp> : public Exp {
    result<db*> _conn;
    result<stmt> _stmt;
public:
    using base = Exp;
    using base::base;
    
    using db_t = db*;
    
    explicit context(result<db*> c) : _conn(std::move(c)) {}
    explicit context(result<db*> c,
                     result<stmt> s,
                     Exp v = Exp{})
        : base(std::move(v))
        , _conn(std::move(c))
        , _stmt(std::move(s))
    {}
    auto& get_stmt()
    { return _stmt; }
    
    auto& conn()
    { return _conn; }
};

template<typename Context>
struct db_of {
    using type = decltype(
        forward(
            *std::declval<decltype(std::declval<Context>().conn())>()));
};

}

namespace tl::detail {

template<typename Db, typename Exp>
struct is_expected_impl<msqlite::context<Db, Exp>>
    : std::true_type {};

}
