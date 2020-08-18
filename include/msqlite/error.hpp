#pragma once

#include <system_error>

namespace msqlite {

enum class error {
    error = 1,
    internal = 2,
    perm = 3,
    abort = 4,
    busy = 5,
    locked = 6,
    nomem = 7,
    readonly = 8,
    interrupt = 9,
    ioerr = 10,
    corrupt = 11,
    notfound = 12,
    full = 13,
    cantopen = 14,
    protocol = 15,
    empty = 16,
    schema = 17,
    toobig = 18,
    constraint = 19,
    mismatch = 20,
    misuse = 21,
    nolfs = 22,
    auth = 23,
    format = 24,
    range = 25,
    notadb = 26,
    notice = 27,
    warning = 28,
};

struct error_category_t : std::error_category {
    constexpr error_category_t() noexcept = default;
    const char* name() const noexcept override
    { return "msqlite.error"; }

    std::string message(int v) const override {
        switch(error{v}) {
        case error::error:
            return "generic error code";
        case error::internal:
            return "internal malfunction";
        case error::perm:
            return "requested access mode for a newly created database could not be provided";
        case error::abort:
            return "operation was aborted prior to completion";
        case error::busy:
            return "database file could not be written (or in some cases read) because of concurrent activity";
        case error::locked:
            return "write operation could not continue because of a conflict";
        case error::nomem:
            return "unable to allocate the memory needed";
        case error::readonly:
            return "database db does not have write permission";
        case error::interrupt:
            return "operation was interrupted";
        case error::ioerr:
            return "operating system reported an I/O error";
        case error::corrupt:
            return "database file has been corrupted";
        case error::notfound:
            return "not found";
        case error::full:
            return "disk is full";
        case error::cantopen:
            return "unable to open a file";
        case error::protocol:
            return "problem with the file locking protocol";
        case error::empty:
            return "empty";
        case error::schema:
            return "database schema has changed";
        case error::toobig:
            return "string or BLOB was too large";
        case error::constraint:
            return "SQL constraint violation occurred";
        case error::mismatch:
            return "datatype mismatch";
        case error::misuse:
            return "usage of interface in a way that is undefined or unsupported";
        case error::nolfs:
            return "system doesn't support large files";
        case error::auth:
            return "SQL stmt being prepared is not authorized";
        case error::format:
            return "format";
        case error::range:
            return "out of rang";
        case error::notadb:
            return "database file doesn't appear to be a SQLite file";
        case error::notice:
            return "an unusual operation is taking place";
        case error::warning:
            return "an unusual and possibly ill-advised operation is taking place";
        default:
            return "unknown error";
        }
    }
};

inline const error_category_t& error_category() noexcept {
    static const error_category_t instance;
    return instance;
}

inline auto make_error_code(error e)
{ return std::error_code{static_cast<int>(e), error_category()}; }

}//namespace msqlite

namespace std {
template<> struct is_error_code_enum<msqlite::error> : std::true_type {};
}//namespace std
