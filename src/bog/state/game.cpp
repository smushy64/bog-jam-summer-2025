/**
 * @file   game.cpp
 * @brief  State/Game
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   August 08, 2025
*/
#include "bog/state.h"
#include "bog/collections.h" // IWYU pragma: keep
#include "bog/ui.h"

#include "bog/scene.h"

#define MIN_HEIGHT (100.0f)

void text_test( State* state );

void draw_scene_title( Font font, const char* scene_name );

float offset_y = MIN_HEIGHT;
void _game_update( State* state ) {
    auto* s     = &state->game;
    auto* scene = &s->scene;

    BeginDrawing();

    if( state->common.is_first_frame ) {
        scene_load( "resources/scenes/schema.json", scene );
        ClearBackground( RAYWHITE );
    }

    bool scene_change = s->scene_id != scene->id;
    bool node_change  = s->node_id  != scene->current_node;

    s->scene_id = scene->id;
    s->node_id  = scene->current_node;

    if( scene_change ) {
        s->is_scene_transition = true;
    }

    auto* node = scene->nodes + scene->current_node;

    ClearBackground( RAYWHITE );
    switch( node->type ) {
        case NodeType::STORY   : {
            auto& tex = s->textures[TEX_MENU];

            Rectangle src, dst;
            src.x      = src.y       = 0;
            src.width  = tex.width;
            src.height = tex.height;

            src.y      = 220;
            src.height = 16;
            src.width  = 352;

            dst.x      = dst.y       = 0;
            dst.width  = src.width;
            dst.height = src.height;

            *(Vector2*)&dst.width = fit_to_dst(
                { (float)GetScreenWidth(), (float)GetScreenHeight() },
                *(Vector2*)&dst.width );

            Rectangle src2, dst2;

            src2.x      = src2.y      = 0;
            src2.width  = tex.width;
            src2.height = tex.height;

            src2.y      = 279;
            src2.height = 6;
            src2.width  = 352;

            dst2.x      = dst2.y       = 0;
            dst2.width  = src2.width;
            dst2.height = src2.height;

            dst2.y += 200;

            *(Vector2*)&dst2.width = fit_to_dst(
                { (float)GetScreenWidth(), (float)GetScreenHeight() },
                *(Vector2*)&dst2.width );

            Rectangle bg = {};

            bg.x      = 0;
            bg.y      = (dst.y + (dst.height - (dst.height / 4.0f)));
            bg.width  = GetScreenWidth();
            bg.height = dst2.y - bg.y;

            DrawRectangleRec( bg, Color{ 40, 4, 16, 176 } );
            DrawTexturePro( tex, src, dst, {}, 0.0f, WHITE );
            DrawTexturePro( tex, src2, dst2, {}, 0.0f, WHITE );
        } break;
        case NodeType::CONTROL :
        case NodeType::FORK    :
        case NodeType::WRITE   :

        case NodeType::NONE:
        case NodeType::COUNT: {
            scene->current_node++;
        } break;
    }


    if( scene->current_node >= scene->nodes.len ) {
        Panic( "handle completing all nodes!" );
    }

    EndDrawing();
}

void draw_scene_title( Font font, const char* scene_name ) {
    Rectangle screen_rect = { 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() };

    DrawRectangleRec(screen_rect, Color{0, 0, 0, 230});

    Vector2 text_size = MeasureTextEx( font, scene_name, font.baseSize, 1.0f );

    Vector2 text_position;
    text_position.x = (screen_rect.width  / 2.0f) - (text_size.x / 2.0f);
    text_position.y = (screen_rect.height / 2.0f) - (text_size.y / 2.0f);

    DrawTextPro(
        font,
        scene_name,
        text_position,
        {}, 0.0f,
        font.baseSize, 1.0f, WHITE );
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

void text_test( State* state ) {
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

    String test_string = "";

    text_draw(
        state->common.font,
        test_string,
        *(Vector2*)&text_area.x,
        &text_area,
        &state->game.display_text );

    if( state->game.display_text.len >= test_string.len ) {
        state->game.display_text = {};
    }


}

