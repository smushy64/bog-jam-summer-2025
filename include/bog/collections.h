#if !defined(BOG_COLLECTIONS_H)
#define BOG_COLLECTIONS_H
/**
 * @file   collections.h
 * @brief  Collections.
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   August 08, 2025
*/
#include "bog/prelude.h" // IWYU pragma: keep
#include "bog/allocation.h"
#include <string.h>

#define MINIMUM_ALLOC_COUNT (16)

template<typename T>
struct List;

template<typename T>
struct Slice;

typedef Slice<char> String;

template<typename T>
Slice<T> advance( const Slice<T>& slice, int amount = 1 );
template<typename T>
Slice<T> truncate( const Slice<T>& slice, int max );

String advance( String string, String other );

bool find_char( String string, char c, int* opt_out_index = nullptr );
bool find_set( String string, String set, int* opt_out_index = nullptr );
bool find_string( String string, String substring, int* opt_out_index = nullptr );

Color parse_color( String string );

bool string_cmp( String a, String b );

// NOTE(alicia): implementation -----------------------------------------------

template<typename T>
struct List {
    int cap, len;
    T*  buf;

    void reserve( int amount = MINIMUM_ALLOC_COUNT ) {
        if( (cap - len) >= amount ) {
            return;
        }

        int alloc_count = amount - (cap - len);
        if( alloc_count < MINIMUM_ALLOC_COUNT ) {
            alloc_count = MINIMUM_ALLOC_COUNT;
        }

        buf = mem_realloc( buf, cap, cap + alloc_count );
        cap += alloc_count;
    }

    void reset() {
        len = 0;
    }

    void free() {
        mem_free( buf, cap );
        buf = len = cap = 0;
    }

    int push( const T& item ) {
        reserve();

        buf[len++] = item;

        return (len - 1);
    }

    bool pop( T* opt_out_item = nullptr ) {
        if( !len ) {
            return false;
        }

        len--;
        if( opt_out_item ) {
            *opt_out_item = buf[len];
        }

        return true;
    }

    const T& operator[]( int index ) const {
        return buf[index];
    }
    T& operator[]( int index ) {
        return buf[index];
    }

    operator const T*() const {
        return buf;
    }
    operator T*() {
        return buf;
    }

    const T& operator *() const {
        return buf[0];
    }
    T& operator *() {
        return buf[0];
    }

    const T* operator+( int amount ) const {
        return buf + amount;
    }
    T* operator+( int amount ) {
        return buf + amount;
    }
};

template<typename T>
struct Slice {
    int len;
    T*  buf;

    const T& operator[]( int index ) const {
        return buf[index];
    }
    T& operator[]( int index ) {
        return buf[index];
    }

    operator const T*() const {
        return buf;
    }
    operator T*() {
        return buf;
    }

    const T& operator *() const {
        return buf[0];
    }
    T& operator *() {
        return buf[0];
    }

    const T* operator+( int amount ) const {
        return buf + amount;
    }
    T* operator+( int amount ) {
        return buf + amount;
    }
};

template<>
struct Slice<char> {
    int   len;
    char* buf;

    constexpr
    Slice() : len(0), buf(0) {}
    constexpr
    Slice( int len, const char* buf ) : len(len), buf((char*)buf) {}
    template<usize ArrayLength> constexpr
    Slice( const char (&array)[ArrayLength] )
        : len(ArrayLength - 1), buf((char*)array) {}
    explicit
    Slice( const char* string )
        : len(strlen(string)), buf((char*)string) {}

    char operator[]( int index ) const {
        return buf[index];
    }
    char& operator[]( int index ) {
        return buf[index];
    }

    operator const char*() const {
        return buf;
    }
    operator char*() {
        return buf;
    }

    char operator *() const {
        return buf[0];
    }
    char& operator *() {
        return buf[0];
    }

    const char* operator+( int amount ) const {
        return buf + amount;
    }
    char* operator+( int amount ) {
        return buf + amount;
    }
};

template<typename T>
Slice<T> advance( const Slice<T>& slice, int amount ) {
    Slice<T> result = slice;
    if( amount >= slice.len ) {
        result.buf = result.buf + result.len;
        result.len = 0;
        return result;
    }

    result.buf = result.buf + amount;
    result.len = result.len - amount;

    return result;
}

template<typename T>
Slice<T> truncate( const Slice<T>& slice, int max ) {
    Slice<T> result = slice;

    if( slice.len < max ) {
        return result;
    }

    result.len = max;
    return result;
}

inline
String advance( String string, String other ) {
    return advance( string, other.len );
}

#endif /* header guard */
