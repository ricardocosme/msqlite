#pragma once

#include "msqlite/detail/prepare.hpp"

#include <tuple>
#include <utility>

namespace msqlite::detail {

template<typename... Binds>
struct prepare_wrapper {
    std::string_view stmt;
    std::tuple<Binds...> binds;
};

template<typename ResultDb, typename... Binds>
auto pipe(ResultDb&& res, detail::prepare_wrapper<Binds...> o) {
    using Db = decltype(*std::forward<ResultDb>(res));
    using ForwardDb = decltype(forward(std::declval<Db>()));
    using Stmt = std::conditional_t<
        std::is_same_v<std::remove_pointer_t<std::remove_reference_t<Db>>, db>,
        stmt,
        stmt*>;
    if(res) {
        auto r = std::forward<ResultDb>(res).and_then(
            [&](auto&& conn) {
                return ::msqlite::detail::prepare(lvref(conn), o.stmt, std::move(o.binds));
            });
        return
            context<ForwardDb, void, Stmt>
            (forward(std::forward<Db>(*res)), std::move(r));
    } else
        return context<ForwardDb, void, Stmt>{res.error()};
}

}
