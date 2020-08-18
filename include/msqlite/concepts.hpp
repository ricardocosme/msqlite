#pragma once

#include <concepts>
#include <type_traits>

namespace msqlite {

template<typename T>
concept ValueOrError = requires (T o) {
    typename std::remove_reference_t<T>::value_type;
    typename std::remove_reference_t<T>::error_type;
    {o.has_value()} -> std::same_as<bool>;
    {o.value()};
    {o.error()};
};

template<typename T>
concept ExpectedDb = ValueOrError<T>
    && std::same_as<
           std::remove_pointer_t<
               typename std::remove_reference_t<T>::value_type>,
           db>
    && std::same_as<
           typename std::remove_reference_t<T>::error_type,
           std::error_code>;

template<typename T>
concept Context = requires (T o) {
    requires std::same_as<typename std::remove_reference_t<T>::db_t, db>
        || std::same_as<typename std::remove_reference_t<T>::db_t, db*>;
    {o.get_stmt()};
    {o.conn()};
};

}
