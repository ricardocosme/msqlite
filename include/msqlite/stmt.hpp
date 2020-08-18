#pragma once

#include <sqlite3.h>

namespace msqlite {

//Represents a prepared stmt
//
//RAII is used to guarantee that the stmt will be finalized after
//the destruction of an object of this type.
//
//The sqlite3_stmt object can be obtained using the method get().
//
//An object of this type is not copyable but it is movable.
class stmt {
    sqlite3_stmt* _stmt{nullptr};
    void try_finalize() noexcept
    { if(_stmt) sqlite3_finalize(_stmt); }
public:
    stmt() = default;
    
    explicit stmt(sqlite3_stmt* stmt) : _stmt(stmt) {}
    
    stmt(stmt&& rhs) noexcept
        : _stmt(rhs._stmt)
    { rhs._stmt = nullptr; }
    
    stmt& operator=(stmt&& rhs) noexcept {
        reset(rhs._stmt);
        rhs._stmt = nullptr;
        return *this;
    }

    ~stmt() noexcept
    { try_finalize(); }
    
    void finalize() noexcept {
        sqlite3_finalize(_stmt);
        _stmt = nullptr;
    }

    sqlite3_stmt* release() noexcept {
        auto p = _stmt;
        _stmt = nullptr;
        return p;
    }
    
    void reset(sqlite3_stmt* p = nullptr) noexcept {
        try_finalize();
        _stmt = p;
    }
    
    sqlite3_stmt* get() const noexcept
    { return _stmt; }
    
    auto operator<=>(const stmt&) const = default;
};

}
