#pragma once

#include "msqlite/detail/exec.hpp"

#include <utility>

namespace msqlite::detail {

struct exec_wrapper { std::string_view stmt; };

template<typename ResultDb>
auto pipe(ResultDb&& res, detail::exec_wrapper o) {
    return std::forward<ResultDb>(res).and_then(
        [&](auto&& db) {
            return detail::exec(lvref(db), o.stmt).map(
                [&]{ return forward(std::forward<decltype(db)>(db)); });
        });
}

}
