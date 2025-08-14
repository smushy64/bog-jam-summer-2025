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

    if( state->common.is_first_frame ) {
        scene_load( "resources/scenes/schema.json", scene );
    }

    bool scene_change = s->scene_id != scene->id;
    bool node_change  = s->node_id  != scene->current_node;

    s->scene_id = scene->id;
    s->node_id  = scene->current_node;

    (void)node_change, (void)scene_change;

    if( scene_change ) {
        s->is_scene_transition = true;
    }

    auto* node = scene->nodes + scene->current_node;

    Vector2 screen = { (float)GetScreenWidth(), (float)GetScreenHeight() };
    Vector2 mouse  = GetMousePosition();

    int target_node = scene->current_node;

    switch( node->type ) {
        case NodeType::STORY: {
            s->text = node->story.text.to_string( scene->string );
            if( node_change ) {
                s->display_text = {};
            }

            if(
                s->display_text.is_complete( s->text ) &&
                CheckCollisionPointRec( mouse, s->text_box ) &&
                IsMouseButtonPressed( MOUSE_BUTTON_LEFT )
            ) {
                // TODO(alicia): proper continue
                target_node++;
            }
        } break;
        case NodeType::CONTROL: {
            switch( node->control.type ) {
                case ControlType::JUMP: {
                    // TODO(alicia): proper jump code.
                    target_node = node->control.jump.node;
                    TraceLog(
                        LOG_INFO,
                        "Jump to %i/%i",
                        node->control.jump.scene, node->control.jump.node );
                } break;
                case ControlType::CONDITIONAL: {
                    String key = node->control.conditional.key.to_string( scene->string );

                    bool condition_result = s->kv.read( key );

                    ConditionalJump* obj = nullptr;
                    if( condition_result ) {
                        obj = &node->control.conditional.if_true;
                    } else {
                        obj = &node->control.conditional.if_false;
                    }

                    if( obj->does_something ) {
                        // TODO(alicia): proper jump code.
                        target_node = obj->node;
                        TraceLog(
                            LOG_INFO,
                            "%s: Jump to %i/%i",
                            condition_result ? "true" : "false", obj->scene, obj->node );
                    } else {
                        // TODO(alicia): proper jump code.
                        target_node++;
                        TraceLog(
                            LOG_INFO,
                            "%s: Jump to %i/%i",
                            condition_result ? "true" : "false", -1, target_node );
                    }
                } break;
                case ControlType::COUNT:
                    break;
            }
        } break;
        case NodeType::FORK: {
            // TODO(alicia): 
            target_node++;
            TraceLog( LOG_INFO, "Fork TODO" );
        } break;
        case NodeType::WRITE: {
            String key = node->write.key.to_string( scene->string );
            s->kv.write( key, node->write.value );
            TraceLog( LOG_INFO, "Write %i to %s", node->write.value, key.buf );
            // TODO(alicia): proper continue
            target_node++;
        } break;

        case NodeType::NONE:
        case NodeType::COUNT: {
            // TODO(alicia): proper continue
            target_node++;
        } break;
    }

    BeginDrawing();
    ClearBackground( RAYWHITE );

    auto& tex = s->textures[TEX_MENU];

    float y_offset, height;

    height   = (state->common.font.baseSize * 6.0f);
    y_offset = (screen.y - height) - 60.0f;

    Rectangle text_area = text_box_draw( tex, y_offset, height, &s->text_box );

    text_draw(
        state->common.font,
        s->text, *(Vector2*)&text_area.x,
        &text_area, &s->display_text );

    switch( node->type ) {
        case NodeType::STORY   : {
            if( s->display_text.is_complete( s->text ) ) {
                _readonly float CONTINUE_BOX_SIZE = 30.0f;

                Rectangle continue_box = {};
                continue_box.x = (text_area.x + text_area.width) - CONTINUE_BOX_SIZE;
                continue_box.y = (text_area.y + text_area.height) - CONTINUE_BOX_SIZE;
                continue_box.width = continue_box.height = CONTINUE_BOX_SIZE;

                DrawRectangleRec( continue_box, WHITE );
            }
        } break;
        case NodeType::CONTROL : {
        } break;
        case NodeType::FORK    : {
            // TODO(alicia): display buttons
        } break;
        case NodeType::WRITE   : {
        } break;

        case NodeType::NONE:
        case NodeType::COUNT: {
        } break;
    }

    if( scene->current_node >= scene->nodes.len ) {
        Panic( "handle completing all nodes!" );
    }

    EndDrawing();

    scene->current_node = target_node;
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

