#pragma once

#include "msqlite/detail/query.hpp"

#include <utility>

namespace msqlite::detail {

struct query_wrapper { std::string_view stmt; };

template<typename ResultDb>
auto pipe(ResultDb&& res, detail::query_wrapper o) {
    using Db = decltype(*std::forward<ResultDb>(res));
    using ForwardDb = decltype(forward(std::declval<Db>()));
    if(res) {
        auto r = std::forward<ResultDb>(res).and_then(
            [&](auto&& conn) { return ::msqlite::detail::query(lvref(conn), o.stmt); });
        return
            context<ForwardDb>
            {forward(std::forward<Db>(*res)), std::move(r)};
    } else
        return context<ForwardDb>{res.error()};
}

}
