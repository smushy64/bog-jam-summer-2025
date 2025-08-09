/**
 * @file   entry.cpp
 * @brief  Entry Points.
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   July 29, 2025
*/
#include "bog/prelude.h"
#include "bog/entry.h"
#include "bog/state.h"

usize query_memory_requirement(void) {
    return sizeof(Memory);
}

bool on_init( void* memory ) {
    auto* mem = (Memory*)memory;

    state_set( &mem->state, StateType::DEFAULT );

    return true;
}

void on_update( void* memory ) {
    auto* mem = (Memory*)memory;

    state_update( &mem->state );
}

void on_close( void* memory ) {
    (void)memory;
}


