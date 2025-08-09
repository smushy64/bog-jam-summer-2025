/**
 * @file   allocation.cpp
 * @brief  Memory allocation.
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   August 08, 2025
*/
#include "bog/allocation.h"
#include <stdlib.h>
#include <string.h>

extern "C" void* _mem_reallocate( void* ptr, int size, int old_count, int new_count ) {
    if( ptr ) {
        void* new_ptr = realloc( ptr, size * new_count );
        if( !new_ptr ) {
            return nullptr;
        }

        memset( (u8*)new_ptr + (size * old_count), 0, size * (new_count - old_count) );

        return new_ptr;
    } else {
        return calloc( new_count, size );
    }
}
extern "C" void _mem_free( void* ptr, int size, int count ) {
    if( !ptr ) {
        return;
    }

    (void)size, (void)count;
    free( ptr );
}


