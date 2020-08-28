#pragma once

#include "msqlite/db.hpp"
#include "msqlite/detail/prepare_impl.hpp"
#include "msqlite/stmt.hpp"

#include <memory>
#include <sqlite3.h>
#include <string>
#include <utility>
#include <vector>

namespace msqlite::cache {

class db : public ::msqlite::db {
    std::vector<std::pair<std::string, std::unique_ptr<stmt>>> _stmts;
public:    
    using base = ::msqlite::db;
    using base::base;

    base& as_base() noexcept
    { return static_cast<base&>(*this); }

    const base& as_base() const noexcept
    { return static_cast<const base&>(*this); }
    
    db(::msqlite::db o) : base(std::move(o)) {}

    template<typename Binds>
    result<stmt*> prepare(std::string_view s, Binds binds) {
        if(auto it = std::find_if(_stmts.begin(), _stmts.end(),
                                  [&](auto& p){ return p.first == s; });
           it != _stmts.end())
        {
            return it->second.get();
        } else {
            return static_cast<result<stmt*>&&>(
                detail::prepare_impl(as_base(), s, std::make_tuple()).map(
                [&](auto&& stmt_) {
                    detail::bind_params(stmt_.get(), std::move(binds));
                    return _stmts.emplace_back(
                        std::string{s},
                        std::make_unique<stmt>(std::move(stmt_))).second.get();
                }));
        }
    }
};

}

