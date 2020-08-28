#pragma once

#include "msqlite/concepts.hpp"
#include "msqlite/db.hpp"
#include "msqlite/result.hpp"
#include "msqlite/stmt.hpp"

#include <system_error>
#include <tl/expected.hpp>

namespace msqlite {

auto& lvref(auto& o) { return o; }
auto& lvref(auto* o) { return *o; }

auto forward(auto&& o) { return std::move(o); }
auto forward(auto& o) { return std::addressof(o); }
auto forward(auto* o) { return o; }

template<typename Db, typename T = void, typename Stmt = stmt>
class context;

template<typename Exp>
class context<db, Exp, stmt> : public Exp {
    result<db> _conn;
    result<stmt> _stmt;
public:
    using base = Exp;
    using base::base;
    
    using db_t = db;
    using stmt_t = stmt;
    
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

template<typename Exp>
class context<db, Exp, stmt*> : public Exp {
    result<db> _conn;
    result<stmt*> _stmt;
public:
    using base = Exp;
    using base::base;
    
    using db_t = db;
    using stmt_t = stmt*;
    
    explicit context(result<db> c) : _conn(std::move(c)) {}
    explicit context(result<db> c,
                     result<stmt*> s,
                     Exp v = Exp{})
        : base(std::move(v))
        , _conn(std::move(c))
        , _stmt(std::move(s))
    {}
    auto& get_stmt()
    { return _stmt; }
    
    auto conn() &&
    { return std::move(_conn); }
    
    auto& conn() &
    { return _conn; }
};

template<>
class context<db, void, stmt> {
    result<db> _conn;
    result<stmt> _stmt;
public:
    using db_t = db;
    using stmt_t = stmt;
    using base = void;
    
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
class context<db, void, stmt*> {
    result<db> _conn;
    result<stmt*> _stmt;
public:
    using db_t = db;
    using stmt_t = stmt*;
    using base = void;
    
    explicit context(result<db> c) : _conn(std::move(c)) {}
    explicit context(result<db> c,
                     result<stmt*> s)
        : _conn(std::move(c))
        , _stmt(std::move(s))
    {}
    auto& get_stmt()
    { return _stmt; }
    
    auto conn() &&
    { return std::move(_conn); }
    
    auto& conn() &
    { return _conn; }
};

template<>
class context<db*, void, stmt> {
    result<db*> _conn;
    result<stmt> _stmt;
public:
    using db_t = db*;
    using stmt_t = stmt;
    using base = void;
    
    explicit context(result<db*> c) : _conn(std::move(c)) {}
    explicit context(result<db*> c,
                     result<stmt> s)
        : _conn(std::move(c))
        , _stmt(std::move(s))
    {}
    auto get_stmt() &&
    { return std::move(_stmt); }
    
    auto& get_stmt() &
    { return _stmt; }
    
    auto& conn()
    { return _conn; }
};

template<>
class context<cache::db*, void, stmt> {
    result<cache::db*> _conn;
    result<stmt> _stmt;
public:
    using db_t = cache::db*;
    using stmt_t = stmt;
    using base = void;
    
    explicit context(result<cache::db*> c) : _conn(std::move(c)) {}
    explicit context(result<cache::db*> c,
                     result<stmt> s)
        : _conn(std::move(c))
        , _stmt(std::move(s))
    {}
    auto get_stmt() &&
    { return std::move(_stmt); }
    
    auto& get_stmt() &
    { return _stmt; }
    
    auto& conn()
    { return _conn; }
};

template<>
class context<db*, void, stmt*> {
    result<db*> _conn;
    result<stmt*> _stmt;
public:
    using db_t = db*;
    using stmt_t = stmt*;
    using base = void;
    
    explicit context(result<db*> c) : _conn(std::move(c)) {}
    explicit context(result<db*> c,
                     result<stmt*> s)
        : _conn(std::move(c))
        , _stmt(std::move(s))
    {}
    auto& get_stmt()
    { return _stmt; }
    
    auto& conn()
    { return _conn; }
};

template<>
class context<cache::db*, void, stmt*> {
    result<cache::db*> _conn;
    result<stmt*> _stmt;
public:
    using db_t = cache::db*;
    using stmt_t = stmt*;
    using base = void;
    
    explicit context(result<cache::db*> c) : _conn(std::move(c)) {}
    explicit context(result<cache::db*> c,
                     result<stmt*> s)
        : _conn(std::move(c))
        , _stmt(std::move(s))
    {}
    auto& get_stmt()
    { return _stmt; }
    
    auto& conn()
    { return _conn; }
};

template<typename Exp>
class context<db*, Exp, stmt> : public Exp {
    result<db*> _conn;
    result<stmt> _stmt;
public:
    using base = Exp;
    using base::base;
    
    using db_t = db*;
    using stmt_t = stmt;
    
    explicit context(result<db*> c) : _conn(std::move(c)) {}
    explicit context(result<db*> c,
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
    
    auto& conn()
    { return _conn; }
};

template<typename Exp>
class context<cache::db*, Exp, stmt> : public Exp {
    result<cache::db*> _conn;
    result<stmt> _stmt;
public:
    using base = Exp;
    using base::base;
    
    using db_t = cache::db*;
    using stmt_t = stmt;
    
    explicit context(result<cache::db*> c) : _conn(std::move(c)) {}
    explicit context(result<cache::db*> c,
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
    
    auto& conn()
    { return _conn; }
};

template<typename Exp>
class context<db*, Exp, stmt*> : public Exp {
    result<db*> _conn;
    result<stmt*> _stmt;
public:
    using base = Exp;
    using base::base;
    
    using db_t = db*;
    using stmt_t = stmt*;
    
    explicit context(result<db*> c) : _conn(std::move(c)) {}
    explicit context(result<db*> c,
                     result<stmt*> s,
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

template<typename Exp>
class context<cache::db*, Exp, stmt*> : public Exp {
    result<cache::db*> _conn;
    result<stmt*> _stmt;
public:
    using base = Exp;
    using base::base;
    
    using db_t = cache::db*;
    using stmt_t = stmt*;
    
    explicit context(result<cache::db*> c) : _conn(std::move(c)) {}
    explicit context(result<cache::db*> c,
                     result<stmt*> s,
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

template<typename Db>
auto make_context(Db&& db) {
    return context<Db>(std::forward<Db>(db));
}

template<typename Db, typename Stmt>
auto make_context(Db&& db, Stmt&& stmt) {
    return context<Db, Stmt>(std::forward<Db>(db), std::forward<Stmt>(stmt));
}

template<typename Context>
struct db_of {
    using type = decltype(
        forward(
            *std::declval<decltype(std::declval<Context>().conn())>()));
};

template<typename Context>
struct stmt_of {
    using type = decltype(
        forward(
            *std::declval<decltype(std::declval<Context>().get_stmt())>()));
};

}

namespace tl::detail {

template<typename Db, typename Exp>
struct is_expected_impl<msqlite::context<Db, Exp>>
    : std::true_type {};

}
