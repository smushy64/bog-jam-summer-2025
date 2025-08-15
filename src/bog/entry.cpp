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

    mem->state.type = StateType::DEFAULT;

    state_set( &mem->state, StateType::INVALID );

    return true;
}

bool on_update( void* memory ) {
    auto* mem = (Memory*)memory;

    auto start_state = mem->state.type;

    state_update( &mem->state );

    if( start_state != mem->state.type ) {
        state_set( &mem->state, start_state );
    }

    if( mem->state.should_quit ) {
        return false;
    }
    return true;
}

void on_close( void* memory ) {
    (void)memory;
}


