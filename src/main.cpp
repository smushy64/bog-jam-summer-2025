/**
 * @file   main.cpp
 * @brief  GTMK Game Jam 2025: Entry Point.
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   July 27, 2025
*/
#include "entry.h"
#include "common.h" // IWYU pragma: keep
#include <stdlib.h>
#include <stdio.h>

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

bool is_first_frame = true;

usize memory_size = 0;
void* memory      = NULL;

void Update(void);

int main( int argc, char** argv ) {
    (void)argc, (void)argv;

    InitWindow( 800, 600, "hello, world!" );

    memory_size = query_memory_requirement();
    memory      = calloc( 1, memory_size );

    if( !memory ) {
        fprintf( stderr, "ERROR: Failed to allocate memory of size %zu!\n", memory_size );
        return 1;
    }

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop( Update, 0, 1 );
#else
    SetTargetFPS(60);

    while( !WindowShouldClose() ) {
        Update();
    }
#endif

    on_close( memory );
    CloseWindow();
    return 0;
}

void Update(void) {
    if( is_first_frame ) {
        if( !on_init( memory ) ) {
            abort();
        }
        is_first_frame = false;
    }

    on_update( memory );
}

