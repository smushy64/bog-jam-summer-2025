/**
 * @file   game.cpp
 * @brief  State/Game
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   August 08, 2025
*/
#include "bog/state.h"
#include "bog/collections.h" // IWYU pragma: keep
#include "bog/ui.h"

#define MIN_HEIGHT (100.0f)

float offset_y = MIN_HEIGHT;
void _game_update( State* state ) {
    BeginDrawing();
    ClearBackground( RAYWHITE );

    auto& tex = state->game.textures[TEX_MENU];

    Rectangle src, dst;

    src.x      = 0;
    src.y      = 0;
    src.width  = tex.width;
    src.height = tex.height;

    src.y      = 220.0f;
    src.height = 16.0f;
    src.width  = 350.0f;

    dst.x      = dst.y       = 0.0f;
    dst.width  = src.width;
    dst.height = src.height;

    dst.y += 20.0f;

    *(Vector2*)&dst.width = fit_to_dst( { (float)GetScreenWidth(), (float)GetScreenHeight() }, *(Vector2*)&dst.width );

    Rectangle src2, dst2;
    src2 = src;
    dst2 = dst;

    src2.y += 59.0f;

    if( IsKeyDown( KEY_DOWN ) ) {
        offset_y += GetFrameTime() * 150.0f;
    } else if( IsKeyDown( KEY_UP ) ) {
        offset_y -= GetFrameTime() * 150.0f;
    }

    if( offset_y < MIN_HEIGHT ) {
        offset_y = MIN_HEIGHT;
    } else if( offset_y >= 600.0f ) {
        offset_y         = 600.0f;
    }

    dst2.y += offset_y;

    Rectangle bg = {};
    bg.x      = 0;
    bg.y      = (dst.y + (dst.height / 2.0f)) + 8.0f;
    bg.width  = dst.width;
    bg.height = dst2.y - bg.y;

    DrawRectangleRec( bg, Color{ 40, 4, 16, 176 } );

    DrawTexturePro( tex, src, dst, {}, 0.0f, WHITE );
    DrawTexturePro( tex, src2, dst2, {}, 0.0f, WHITE );

    Rectangle text_area = bg;

    text_area = padding( text_area, 30.0f, 60.0f, 10.0f );

    String test_string = "Dapibus urna id, <rgba:ff,0,0,ff>nullam aenean<rgba:ff,ff,ff,ff>, dapibus metus suscipit maximus erat. Sagittis imperdiet vivamus habitasse, integer sit non, nullam ex faucibus. Ante quisque lorem dignissim metus suscipit integer facilisis, laoreet quis. Varius vitae faucibus sit pellentesque, convallis tristique consequat consectetur ridiculus. Pellentesque faucibus curabitur odio nullam, nam, rhoncus vitae fusce lacus. Eget ante est, sit suspendisse, nam euismod quisque pretium ultricies. Phasellus faucibus eu, consectetur tristique ante aliquam in nisl nam. Magna eget nullam sem pellentesque ullamcorper est feugiat ipsum lacus. Elit, ipsum, tincidunt sem montes tristique tellus sed nisi ultricies. Etiam pellentesque, velit aenean et eu in dolor sed, ullamcorper. Eget viverra tempus rutrum dignissim fermentum quis tempor, cursus massa. Suscipit morbi eget, dapibus, eleifend platea lacus praesent metus lacus. Tempor, vitae dolor suspendisse facilisis curabitur, euismod diam turpis dolor. Sed purus, maximus, nisl nulla ridiculus quis sagittis euismod aliquam. Vulputate sapien, dolor habitasse finibus metus massa tellus dui penatibus. Nullam nibh dui tristique vulputate, nulla eu consequat, tempor vestibulum. Tristique ante faucibus, sem, ultrices etiam at odio dui lorem. Magna varius convallis lectus tincidunt, leo pharetra in consectetur nibh. Posuere dis semper consequat phasellus nunc arcu faucibus nunc erat. Neque consequat nullam eros, a vestibulum, arcu imperdiet felis dignissim. Lorem cras neque ornare ultrices quisque ac, ultrices diam luctus. Libero eget praesent dapibus cursus amet eget ipsum vel, et. Nascetur sed convallis dui ipsum efficitur elit ut, quisque a. Pulvinar, purus, aenean orci consequat quisque nunc sit pharetra quis. Erat, in lorem lacus ac, libero ipsum volutpat ante aliquet. Nam, luctus dapibus tempus nulla pulvinar ac, tristique nulla posuere.";

    text_draw(
        state->common.font,
        test_string,
        *(Vector2*)&text_area.x,
        &text_area,
        &state->game.display_text );

    if( state->game.display_text.len >= test_string.len ) {
        state->game.display_text = {};
    }

    EndDrawing();
}

void _game_load( State* state ) {
    for( int i = 0; i < TEX_COUNT; ++i ) {
        auto* texture = state->game.textures + i;

        *texture = LoadTexture( TEXTURE_LOAD_PARAMS[i].path );
        SetTextureFilter( *texture, TEXTURE_LOAD_PARAMS[i].filter );
    }
}
void _game_unload( State* state ) {
    for( int i = 0; i < TEX_COUNT; ++i ) {
        UnloadTexture( state->game.textures[i] );
    }
}

