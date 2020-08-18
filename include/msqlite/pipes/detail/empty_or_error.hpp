#pragma once

namespace msqlite::detail {

template<typename F>
struct empty_or_error_wrapper : F {
    using base = F;
    template<typename F_>
    empty_or_error_wrapper(F_ f) : base(std::forward<F_>(f)) {}
};

}
