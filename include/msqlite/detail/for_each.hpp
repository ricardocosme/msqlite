#pragma once

#include "msqlite/detail/call.hpp"
#include "msqlite/error.hpp"

namespace msqlite::detail {

template<typename F>
result<void> for_each(stmt& stmt, F&& f) {
    int res;
    do {
        res = sqlite3_step(stmt.get());
        if(res == SQLITE_ROW)
            detail::call(std::forward<F>(f),
                         stmt.get(),
                         std::make_index_sequence<
                             detail::arity<std::remove_reference_t<decltype(f)>>::value>{});
        else break;
    } while (true);
    
    if(res == SQLITE_DONE) {
        //postcondition: it always returns SQLITE_OK here. We don't
        //need to check the result.
        sqlite3_reset(stmt.get());
        return {};
    }
    
    return std::error_code{error{res}};
}

}
