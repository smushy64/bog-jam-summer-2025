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

void draw_scene_title( Font font, const char* scene_name );

void _game_update( State* state ) {
    auto* s     = &state->game;
    auto* scene = &s->scene;

    float   dt     = GetFrameTime();
    Vector2 mouse  = GetMousePosition();
    Vector2 screen = { (float)GetScreenWidth(), (float)GetScreenHeight() };

    bool left_pressed = IsMouseButtonPressed( MOUSE_BUTTON_LEFT );
    bool left_down    = IsMouseButtonDown( MOUSE_BUTTON_LEFT );

    bool on_scene_change = s->scene_id != scene->id;
    bool on_node_change  = s->node_id  != scene->current_node;

    bool scene_transition_finished = s->scene_change_timer >= SCENE_TRANSITION_TIME;

    int target_node = scene->current_node;

    Node* node = scene->get_current();
    if( node ) switch( node->type ) {
        case NodeType::STORY: {
            auto* story = &node->story;

            if( on_node_change ) {
                s->display_text = {};

                s->text = story->text.to_string( scene->string );

                if( story->character.len ) {
                    s->character_name = story->character.to_string( scene->string );
                } else if( story->animation.clear ) {
                    s->character_name = {};
                }
            }
            if( on_scene_change ) {
                s->scene_change_timer = 0.0f;
            }

            if(
                scene_transition_finished &&
                s->display_text.is_complete( s->text ) &&
                CheckCollisionPointRec( mouse, s->text_box ) &&
                left_pressed
            ) {
                target_node = scene_jump_calculate_next( scene );
            }
        } break;
        case NodeType::CONTROL: switch( node->control.type ) {
            case ControlType::JUMP: {
                auto* c = &node->control.jump;

                // TODO(alicia): jump to other scene/node
                target_node = c->node;
                TraceLog(
                    LOG_INFO, "Jump to %i/%i",
                    c->scene, c->node );
            } break;
            case ControlType::CONDITIONAL: {
                auto* c = &node->control.conditional;

                String key = c->key.to_string( scene->string );

                ConditionalJump* obj = nullptr;

                bool is_true = s->kv.read( key ) != 0;
                if( is_true ) {
                    obj = &c->if_true;
                } else {
                    obj = &c->if_false;
                }

                if( obj->does_something ) {
                    // TODO(alicia): jump to other scene/node
                    target_node = obj->node;
                    TraceLog(
                        LOG_INFO,
                        "%s: Jump to %i/%i",
                        is_true ? "true" : "false", obj->scene, obj->node );
                } else {
                    target_node = scene_jump_calculate_next( scene );
                    TraceLog(
                        LOG_INFO,
                        "%s: Jump to %i/%i",
                        is_true ? "true" : "false", -1, target_node );
                }
            } break;
            case ControlType::COUNT:
                break;
        } break;
        case NodeType::WRITE: {
            auto* w = &node->write;

            String key = w->key.to_string( scene->string );
            s->kv.write( key, w->value );

            TraceLog( LOG_INFO, "Wrote %i to '%s'", w->value, key.buf );

            target_node = scene_jump_calculate_next( scene );
        } break;
        case NodeType::FORK:

        case NodeType::NONE:
        case NodeType::COUNT:
            break;
    }

    // NOTE(alicia): draw -------------------------------------------
    BeginDrawing();
    ClearBackground( Color{27, 27, 27, 255} );

    auto& font     = state->common.font;
    auto& tex_menu = s->textures[TEX_MENU];

    float text_box_y, text_box_height;

    text_box_height = (font.baseSize * 6.0f);
    text_box_y      = (screen.y - text_box_height) - 60.0f;

    Rectangle text_area = text_box_draw( tex_menu, text_box_y, text_box_height, &s->text_box );

    if( scene_transition_finished ) {

        if( s->character_name.buf && s->character_name.len ) {
            Vector2 name_size = MeasureTextEx( font, s->character_name.buf, font.baseSize, 1.0f );

            Vector2 position = {};
            position.x = (s->text_box.x + (s->text_box.width / 2.0f)) - (name_size.x / 2.0f);
            position.y = s->text_box.y;

            DrawTextPro(
                font, s->character_name.buf, position, {}, 0.0f, font.baseSize, 1.0f, WHITE );
        }

        text_draw( font, s->text, *(Vector2*)&text_area.x, &text_area, &s->display_text );
    }

    Rectangle src_decoration = COORD_DECORATION;
    Rectangle dst_decoration = { 10, 10 };
    *(Vector2*)&dst_decoration.width = *(Vector2*)&src_decoration.width * 2.0f;

    DrawTexturePro( tex_menu, src_decoration, dst_decoration, {}, 0.0f, WHITE );

    dst_decoration.x     = (screen.x - dst_decoration.width) - dst_decoration.x;
    src_decoration.width = -src_decoration.width;

    DrawTexturePro( tex_menu, src_decoration, dst_decoration, {}, 0.0f, WHITE );

    if( node ) switch( node->type ) {
        case NodeType::STORY: {
            if( s->display_text.is_complete( s->text ) ) {
                float t = sin( s->elapsed * 8.0f );

                if( t < 0.0f ) {
                    t = 0.0f;
                }

                Color tint = ColorAlpha( WHITE, Lerp( 0.2f, 1.0f, 1.0f - t ) );

                Rectangle src, dst;

                src = COORD_CONTINUE_BUTTON;

                *(Vector2*)&dst.width = *(Vector2*)&src.width * 4.0f;

                dst.x = (text_area.x + text_area.width)  - dst.width;
                dst.y = (text_area.y + text_area.height) - dst.height;

                DrawTexturePro( tex_menu, src, dst, {}, 0.0f, tint );
            }
        } break;
        case NodeType::CONTROL:
        case NodeType::FORK:
        case NodeType::WRITE:

        case NodeType::NONE:
        case NodeType::COUNT:
            break;
    }

    if( s->scene_change_timer < SCENE_TRANSITION_TIME ) {
        String title = scene->title.to_string( scene->string );
        if( title.buf ) {
            draw_scene_title( state->common.font, title.buf );
        }
    }

    DrawText( TextFormat( "scene: %i | %i\nnode: %i | %i", s->scene_id, scene->id, s->node_id, scene->current_node ), 10, 10, 24, BLUE );

    EndDrawing();
    // NOTE(alicia): post -------------------------------------------

    s->scene_id = scene->id;
    s->node_id  = scene->current_node;

    s->elapsed            += dt;
    s->scene_change_timer += dt;

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
    auto* s = &state->game;

    s->elapsed = 0.0f;

    s->scene_id = -1;
    s->node_id  = -1;

    s->anim_button_play.speed     =
    s->anim_button_settings.speed =
    s->anim_button_credits.speed  =
    s->anim_button_quit.speed     = 4.0f;

    s->anim_button_play.set( ANIM_BUTTON_PLAY_SELECT );
    s->anim_button_settings.set( ANIM_BUTTON_SETTINGS_SELECT );
    s->anim_button_credits.set( ANIM_BUTTON_CREDITS_SELECT );
    s->anim_button_quit.set( ANIM_BUTTON_QUIT_SELECT );

    // TODO(alicia): enumerate scenes and load first scene found.
    scene_load( "resources/scenes/scene-01.json", &s->scene );

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

void _old_update( State* state ) {
    auto* s     = &state->game;
    auto* scene = &s->scene;

    float dt = GetFrameTime();

    if( state->common.is_first_frame ) {
        scene_load( "resources/scenes/schema.json", scene );
    }

    bool scene_change = s->scene_id != scene->id;
    bool node_change  = s->node_id  != scene->current_node;

    bool should_move_to_next_scene = false;

    s->scene_id = scene->id;
    s->node_id  = scene->current_node;

    (void)node_change, (void)scene_change;

    Node* node = nullptr;
    for( int i = 0; i < scene->nodes.len; ++i ) {
        if( scene->nodes[i].id == scene->current_node ) {
            node = scene->nodes + i;
        }
    }

    Vector2 screen = { (float)GetScreenWidth(), (float)GetScreenHeight() };
    Vector2 mouse  = GetMousePosition();

    int target_node = scene->current_node;

    if( node ) {

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
                    target_node = scene_jump_calculate_next( scene );
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
                            target_node = scene_jump_calculate_next( scene );
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
            } break;
            case NodeType::WRITE: {
                String key = node->write.key.to_string( scene->string );
                s->kv.write( key, node->write.value );
                TraceLog( LOG_INFO, "Write %i to %s", node->write.value, key.buf );
                target_node = scene_jump_calculate_next( scene );
            } break;

            case NodeType::NONE:
            case NodeType::COUNT: {
                target_node = scene_jump_calculate_next( scene );
            } break;
        }
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

    Rectangle src_decoration = {
        2, 169,
        28, 28
    };
    Rectangle dst_decoration = { 10, 10 };
    *(Vector2*)&dst_decoration.width = *(Vector2*)&src_decoration.width * 2.0f;

    DrawTexturePro( tex, src_decoration, dst_decoration, {}, 0.0f, WHITE );

    dst_decoration.x = (screen.x - dst_decoration.width) - dst_decoration.x;
    src_decoration.width = -src_decoration.width;

    DrawTexturePro( tex, src_decoration, dst_decoration, {}, 0.0f, WHITE );

    auto draw_button = [s,dt]( float x, float y, int which, float size = 2.0f ) -> Rectangle {
        auto  src = s->anim[which].update( dt );
        auto& tex = s->textures[src.texture];

        Rectangle dst = {};
        dst.x = x;
        dst.y = y;
        *(Vector2*)&dst.width = (*(Vector2*)&src.src.width) * size;

        DrawTexturePro( tex, src.src, dst, {}, 0.0f, WHITE );

        return dst;
    };

    Vector2   button_pos  = { 200.0f, 10.0f };
    Rectangle button_rect = {};

    float button_padding = 2.0f;
    for( int i = 0; i < 4; ++i ) {
        button_rect = draw_button( button_pos.x, button_pos.y, i );
        if( CheckCollisionPointRec( mouse, button_rect ) ) {
            s->anim[i].set_once( ANIM_BUTTON_PLAY_SELECT + (i * 2) );
        } else {
            s->anim[i].set_once( ANIM_BUTTON_PLAY_DESELECT + (i * 2) );
        }
        button_pos.y += button_rect.height + button_padding;
    }

    if( node ) {

        switch( node->type ) {
            case NodeType::STORY   : {
                if( s->display_text.is_complete( s->text ) ) {
                    // NOTE(alicia): draw continue box

                    float t = sin( s->elapsed * 8.0f );
                    if( t < 0.0f ) {
                        t = 0.0f;
                    }
                    Color tint = ColorAlpha( WHITE, Lerp( 0.2f, 1.0f, 1.0f - t ) );

                    Rectangle src, dst;
                    src = { 125, 32, 11, 11 };

                    *(Vector2*)&dst.width = *(Vector2*)&src.width * 4.0f;

                    dst.x = (text_area.x + text_area.width)  - dst.width;
                    dst.y = (text_area.y + text_area.height) - dst.height;

                    DrawTexturePro( tex, src, dst, {}, 0.0f, tint );
                }
            } break;
            case NodeType::CONTROL : {
            } break;
            case NodeType::FORK    : {
                // TODO(alicia): display buttons
                // TODO(alicia): fork buttons
                target_node = scene_jump_calculate_next( scene );
                TraceLog( LOG_INFO, "Fork TODO" );
            } break;
            case NodeType::WRITE   : {
            } break;

            case NodeType::NONE:
            case NodeType::COUNT: {
            } break;
        }
    }

    EndDrawing();

    if( target_node < 0 ) {
        // TODO(alicia): flag that we need to find the next scene
        TraceLog( LOG_INFO, "switch scene here!" );
        scene->current_node = target_node;
    } else {
        scene->current_node = target_node;
    }

    s->elapsed += dt;

}

