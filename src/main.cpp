/**
 * @file   main.cpp
 * @brief  Bog Jam Summer 2025: Entry Point.
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   July 27, 2025
*/
#include "bog/prelude.h" // IWYU pragma: keep
#include "bog/entry.h"
#include <stdlib.h>
#include <stdio.h>

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

bool is_first_frame = true;
bool should_quit    = false;

usize memory_size = 0;
void* memory      = NULL;

void Update(void);

int main( int argc, char** argv ) {
    (void)argc, (void)argv;

    InitWindow( 1280, 720, "Protocol Smile - Bog Jam Summer 2025" );
    InitAudioDevice();

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
        if( should_quit ) {
            break;
        }
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

    if( !on_update( memory ) ) {
        should_quit = true;
    }
}

