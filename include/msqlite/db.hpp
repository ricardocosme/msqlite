#pragma once

#include <sqlite3.h>

namespace msqlite {

//Represents a database connection
//
//RAII is used to guarantee that the connection will be closed after
//the destruction of an object of this type.
//
//The sqlite3 object can be obtained using the method get().
//
//An object of this type is not copyable but it is movable.
class db {
    sqlite3* _db{nullptr};
    void try_close() noexcept
    { if(_db) sqlite3_close(_db); }
public:    
    db() = default;
    
    explicit db(sqlite3* db) : _db(db) {}

    db(db&& rhs) noexcept
        : _db(rhs._db)
    { rhs._db = nullptr; }
    
    db& operator=(db&& rhs) noexcept {
        reset(rhs._db);
        rhs._db = nullptr;
        return *this;
    }
    
    ~db() noexcept
    { try_close(); }

    void close() noexcept {
        sqlite3_close(_db);
        _db = nullptr;
    }
    
    sqlite3* release() noexcept {
        auto p = _db;
        _db = nullptr;
        return p;
    }

    void reset(sqlite3* p = nullptr) noexcept {
        try_close();
        _db = p;
    }
    
    sqlite3* get() const noexcept
    { return _db; }

    auto operator<=>(const db&) const = default;
};

}
