/**
 * @file   menu.cpp
 * @brief  State/Menu
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   August 08, 2025
*/
#include "bog/state.h"

void _menu_update( State* state ) {
    auto* s = &state->menu;

    float dt = GetFrameTime();

    Vector2 mouse  = GetMousePosition();
    Vector2 screen = { (float)GetScreenWidth(), (float)GetScreenHeight() };

    BeginDrawing();
    ClearBackground(BLACK);

#if defined(PLATFORM_WEB)
    _readonly int MAX_BUTTONS = 3;
#else
    _readonly int MAX_BUTTONS = 4;
#endif

    if( state->common.game_finished_once ) {
        DrawTexturePro(
            s->second,
            { 0, 0, (float)s->second.width, (float)s->second.height },
            { 0, 0, screen.x, screen.y }, {}, 0.0f, WHITE );
    } else {
        DrawTexturePro(
            s->first,
            { 0, 0, (float)s->first.width, (float)s->first.height },
            { 0, 0, screen.x, screen.y }, {}, 0.0f, WHITE );
    }

    Texture tex = s->texture;

    Rectangle src, dst;
    src = dst = {};

    src = COORD_LOGO;
    *(Vector2*)&dst.width = *(Vector2*)&src.width * 4.0f;

    dst.x = 0.0f;
    dst.y = 160.0f;

    DrawTexturePro( tex, src, dst, {}, 0.0f, WHITE );

    bool left_pressed = IsMouseButtonPressed( MOUSE_BUTTON_LEFT );

    dst.y += dst.height + 40.0f;
    for( int i = 0; i < MAX_BUTTONS; ++i ) {
        auto* btn = s->buttons + i;

        src = btn->update( dt ).src;
        
        *(Vector2*)&dst.width = *(Vector2*)&src.width * 2.0f;

        if( CheckCollisionPointRec( mouse, dst ) && !s->is_settings_open && !s->is_credits_open ) {
            btn->set_once( ANIM_BUTTON_PLAY_BIG_SELECT + (i * 2) );

            if( left_pressed ) {
                switch( i ) {
                    // play 
                    case 0: {
                        state->type = StateType::GAME;
                    } break;
                    // settings
                    case 1: {
                        s->is_settings_open = true;
                        s->is_credits_open  = false;
                    } break;
                    // credits
                    case 2: {
                        s->is_settings_open = false;
                        s->is_credits_open  = true;
                    } break;
                    // quit
                    case 3: {
                        state->should_quit = true;
                    } break;
                }
            }

        } else {
            btn->set_once( ANIM_BUTTON_PLAY_BIG_DESELECT + (i * 2) );
        }

        DrawTexturePro( tex, src, dst, {}, 0.0f, WHITE );

        dst.y += dst.height;
    }

    if( s->is_settings_open ) {
        draw_settings( &state->common.settings, state->common.font, &s->is_settings_open );
    } else if( s->is_credits_open ) {
        draw_credits( state->common.font, &s->is_credits_open );
    }

    EndDrawing();
}

void _menu_load( State* state ) {
    auto* s = &state->menu;

    s->texture = LoadTexture( "resources/textures/menu_spritesheet.png" );
    SetTextureFilter( s->texture, TEXTURE_FILTER_POINT );
    s->first  = LoadTexture( "resources/textures/menu_background.png" );
    SetTextureFilter( s->first, TEXTURE_FILTER_POINT );
    s->second = LoadTexture( "resources/textures/menu_background2.png" );
    SetTextureFilter( s->second, TEXTURE_FILTER_POINT );

    for( size_t i = 0; i < ARRAY_LEN(s->buttons); ++i ) {
        auto* btn = s->buttons + i;

        btn->speed = 4.0f;
        btn->set( ANIM_BUTTON_PLAY_BIG_DESELECT + (i * 2) );
    }
}
void _menu_unload( State* state ) {
    auto* s = &state->menu;

    UnloadTexture( s->texture );
    UnloadTexture( s->first );
    UnloadTexture( s->second );
}
