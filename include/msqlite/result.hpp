#pragma once

#include "msqlite/error.hpp"

#include <concepts>
#include <tl/expected.hpp>

namespace msqlite {

//This is used to represent any result that is returned by any public
//function that needs to report a success or an error. The exception
//is the interface(msqlite/throws) with support to C++ exceptions.
//
//The 'result' contains an object of type T in case of success or
//otherwise it contains a std::error_code.
template<typename T>
struct result : tl::expected<T, std::error_code> {
    using base = tl::expected<T, std::error_code>;
    using base::base;

    constexpr result(std::error_code ec)
        : base(tl::unexpected(std::move(ec)))
    {}
};

} //namespace msqlite

namespace tl::detail {
template<typename T>
struct is_expected_impl<msqlite::result<T>> : std::true_type {};
}
