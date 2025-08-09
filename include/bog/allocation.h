#if !defined(BOG_ALLOCATION_H)
#define BOG_ALLOCATION_H
/**
 * @file   allocation.h
 * @brief  Memory allocation.
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   August 08, 2025
*/
#include "bog/prelude.h" // IWYU pragma: keep

extern "C" void* _mem_reallocate( void* ptr, int size, int old_count, int new_count );
extern "C" void  _mem_free( void* ptr, int size, int count );

template<typename T>
T* mem_alloc( int count ) {
    return (T*)_mem_reallocate( nullptr, sizeof(T), 0, count );
}

template<typename T>
T* mem_realloc( T* ptr, int old_count, int new_count ) {
    return (T*)_mem_reallocate( ptr, sizeof(T), old_count, new_count );
}

template<typename T>
void mem_free( T* ptr, int count ) {
    _mem_free( ptr, sizeof(T), count );
}

#endif /* header guard */
