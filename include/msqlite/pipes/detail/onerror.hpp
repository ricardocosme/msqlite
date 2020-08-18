#pragma once

#include <utility>

namespace msqlite::detail {

template<typename F>
struct onerror_wrapper : F {
    using base = F;
    template<typename F_>
    onerror_wrapper(F_ f) : base(std::forward<F_>(f)) {}
};

}
