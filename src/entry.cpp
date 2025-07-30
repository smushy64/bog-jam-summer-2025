/**
 * @file   entry.cpp
 * @brief  Entry Points.
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   July 29, 2025
*/
#include "common.h"
#include "entry.h"
#include <string.h>

struct Memory {
    union {
        struct {
            char string[1024];
        };
        u8 raw[1024];
    };
};

usize query_memory_requirement(void) {
    return sizeof(Memory);
}

bool on_init( void* memory ) {
    auto* mem = (Memory*)memory;

    char* text = LoadFileText( "resources/keep.me" );

    int length = strlen( text );
    memcpy( mem->string, text, length );

    UnloadFileText( text );

    return true;
}

void on_update( void* memory ) {
    auto* mem = (Memory*)memory;

    BeginDrawing();
    ClearBackground(RAYWHITE);

    DrawText( mem->string, 200, 200, 24, RED );

    EndDrawing();
}

void on_close( void* memory ) {
    (void)memory;
}


