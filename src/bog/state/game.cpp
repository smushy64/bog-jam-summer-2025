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

void draw_scene_title( Font font, const char* scene_name, float percent );

void _game_update( State* state ) {
    auto* s     = &state->game;
    auto* scene = &s->scene;

    float volume_music = state->common.settings.volume * state->common.settings.music;
    float volume_sfx   = state->common.settings.volume * state->common.settings.sfx;
    (void)volume_sfx;

    float   dt     = GetFrameTime();
    Vector2 mouse  = GetMousePosition();
    Vector2 screen = { (float)GetScreenWidth(), (float)GetScreenHeight() };

    bool left_pressed, left_down;
    left_pressed = left_down = false;

    if( !s->is_paused ) {
        left_pressed = IsMouseButtonPressed( MOUSE_BUTTON_LEFT );
        left_down    = IsMouseButtonDown( MOUSE_BUTTON_LEFT );
    }

    bool on_scene_change = s->scene_id != scene->id;
    bool on_node_change  = s->node_id  != scene->current_node;

    bool scene_transition_finished = s->scene_change_timer >= SCENE_TRANSITION_TIME;

    int target_node = scene->current_node;

    Node* node = nullptr;

    if( s->is_paused ) {
        text_set_display_speed( TEXT_SPEED );
    } else {
        node = scene->get_current();
        if( left_down ) {
            text_set_display_speed( TEXT_SPEED_FAST );
        } else {
            text_set_display_speed( TEXT_SPEED );
        }
    }

    if( node ) switch( node->type ) {
        case NodeType::STORY: {
            auto* story = &node->story;

            if( on_node_change ) {
                s->display_text = {};

                s->text = story->text.to_string( scene->string );

                if( story->character.len ) {
                    s->character_name = story->character.to_string( scene->string );
                } else {
                    s->character_name = {};
                }

                if( story->animation.clear ) {
                    s->character_name    = {};
                    s->current_character = -1;
                    for( size_t i = 0; i < ARRAY_LEN(s->characters); ++i ) {
                        s->characters[i].is_enabled = false;
                    }
                }
            }
            if( on_scene_change ) {
                s->scene_change_timer = 0.0f;
            }

            if(
                scene_transition_finished && (
                    (
                        s->display_text.is_complete( s->text ) &&
                        CheckCollisionPointRec( mouse, s->text_box ) &&
                        left_pressed
                    ) ||
                    ( !s->text.len )
                )
            ) {
                target_node = scene_jump_calculate_next( scene );
            }

            String animation_name = story->animation.name.to_string( scene->string );
            switch( story->animation.side ) {
                case AnimationSide::LEFT: {
                    s->current_character = 0;
                } break;
                case AnimationSide::CENTER: {
                    s->current_character = 1;
                } break;
                case AnimationSide::RIGHT: {
                    s->current_character = 2;
                } break;

                case AnimationSide::KEEP:
                case AnimationSide::COUNT:
                    break;
            }

            if( animation_name.len ) {
                int animation_id = -1;
                if( animation_from_string( animation_name, &animation_id ) ) {
                    s->characters[s->current_character].is_enabled = true;
                    s->characters[s->current_character].anim.set_once( animation_id );
                }
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
                        "%s = %s: Jump to %i/%i",
                        key.buf, is_true ? "true" : "false", obj->scene, obj->node );
                } else {
                    target_node = scene_jump_calculate_next( scene );
                    TraceLog(
                        LOG_INFO,
                        "%s = %s: Jump to %i/%i",
                        key.buf, is_true ? "true" : "false", -1, target_node );
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
        case NodeType::FORK: {
            auto* f = &node->fork;
            
            if( on_node_change ) {
                Slice<ForkOption> options = {
                    f->len, (ForkOption*)(scene->storage + f->byte_offset)
                };

                for( int i = 0; i < options.len; ++i ) {
                    String text = options[i].text.to_string( scene->string );

                    s->buttons.push( text );
                }
            }
        } break;

        case NodeType::FADE: {
            if( on_node_change ) {
                s->fade_is_reverse = node->fade.reverse;

                if( s->fade_is_reverse ) {
                    s->fade_timer = FADE_TIME;
                } else {
                    s->fade_timer = 0.0f;
                }
                TraceLog( LOG_INFO, "begin fade" );
            }

            bool fade_complete = false;
            if( s->fade_is_reverse ) {
                fade_complete = s->fade_timer <= 0.0f;
            } else {
                fade_complete = s->fade_timer >= FADE_TIME;
            }

            if( fade_complete ) {
                target_node = scene_jump_calculate_next( scene );
                TraceLog( LOG_INFO, "end fade" );
            }
        } break;
        case NodeType::NONE:
        case NodeType::COUNT: {
            target_node = scene_jump_calculate_next( scene );
        } break;
    }

    // NOTE(alicia): draw -------------------------------------------
    BeginDrawing();
    ClearBackground( Color{27, 27, 27, 255} );

    if( s->current_music >= 0 ) {
        UpdateMusicStream( s->music[s->current_music] );
    }

    auto& font     = state->common.font;
    auto& tex_menu = s->textures[TEX_MENU];

    auto& tex_background = s->textures[TEX_BG1 + s->kv.read( "bg" )];

    DrawTexturePro(
        tex_background,
        { 0, 0, (float)tex_background.width, (float)tex_background.height },
        { 0, 0, screen.x, screen.y }, {}, 0.0f, WHITE );

    float text_box_y, text_box_height;

    text_box_height = (font.baseSize * 6.0f);
    text_box_y      = (screen.y - text_box_height) - 60.0f;

    auto draw_character = [dt,screen]( int side, bool is_current, Color& tint, AnimationTimeline& timeline, Texture* textures ) -> Rectangle {
        Color target_tint = is_current ? WHITE : COLOR_CHARACTER_DIM;
        tint = ColorLerp( tint, target_tint, dt * 10.0f );

        auto frame = timeline.update( dt );

        auto& tex = textures[frame.texture];

        Rectangle src, dst;
        src = dst = {};

        src = frame.src;

        *(Vector2*)&dst.width = *(Vector2*)&src.width * 4.0f;
        dst.y = screen.y - dst.height;

        switch( side ) {
            case 0: {
                if( src.width > 134 ) {
                    dst.x -= 80.0f;
                }
                // src.width = -src.width;
            } break;
            case 1: {
                dst.x = (screen.x / 2.0f) - (dst.width / 2.0f);
            } break;
            case 2: {
                dst.x = screen.x - dst.width;
            } break;
        }

        DrawTexturePro( tex, src, dst, {}, 0.0f, tint );

        return dst;
    };

    if( s->char_left.is_enabled ) {
        draw_character(
            0,
            s->current_character == 0,
            s->char_left.tint,
            s->char_left.anim,
            s->textures );
    }

    if( s->char_center.is_enabled ) {
        draw_character(
            1,
            s->current_character == 1,
            s->char_center.tint,
            s->char_center.anim,
            s->textures );
    }

    if( s->char_right.is_enabled ) {
        draw_character(
            2,
            s->current_character == 2,
            s->char_right.tint,
            s->char_right.anim,
            s->textures );
    }

    float fade_time = s->fade_timer;
    if( fade_time < 0.0f ) {
        fade_time = 0.0f;
    }
    if( fade_time > FADE_TIME ) {
        fade_time = FADE_TIME;
    }
    float t = fade_time / FADE_TIME;

    Color color = ColorLerp( BLACK, {}, t );

    DrawRectangleRec( { 0.0f, 0.0f, screen.x, screen.y }, color );

    Rectangle text_area = text_box_draw( tex_menu, text_box_y, text_box_height, &s->text_box );

    if( scene_transition_finished ) {

        if( s->character_name.buf && s->character_name.len ) {
            Rectangle bg = text_measure( font, s->character_name, {} );

            Vector2 position = {};
            position.x = (s->text_box.x + (s->text_box.width / 2.0f)) - (bg.width / 2.0f);
            position.y = s->text_box.y;

            *(Vector2*)&bg.x = position;

            bg = margin( bg, 10.0f, 20.0f );

            DrawRectangleRec( bg, COLOR_TEXT_BOX_BACKGROUND );

            text_draw( font, s->character_name, position );
        }

        if( IsMouseButtonPressed( MOUSE_BUTTON_RIGHT ) ) {
            s->display_text.len = s->text.len;
        }

        text_draw(
            font, s->text,
            *(Vector2*)&text_area.x,
            &text_area, &s->display_text, s->is_paused ? 0.0f : dt );
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
        case NodeType::FORK: if( scene_transition_finished ) {
            auto* f = &node->fork;

            int selected = s->buttons.update_and_draw(
                font, s->textures, screen, mouse, left_pressed, dt );

            bool advance_to_next_node = false;

            if( selected >= 0 ) {
                advance_to_next_node = true;

                s->buttons.reset();

                Slice<ForkOption> options = {
                    f->len, (ForkOption*)(scene->storage + f->byte_offset)
                };

                ForkOption* option = options.buf + selected;

                switch( option->type ) {
                    case ForkActionType::JUMP  : {
                        // TODO(alicia): jump
                        target_node = option->jump.node;

                        advance_to_next_node = false;
                    } break;
                    case ForkActionType::WRITE : {
                        String key = option->write.key.to_string( scene->string );

                        s->kv.write( key, option->write.value );
                    } break;

                    case ForkActionType::NONE  :
                    case ForkActionType::COUNT :
                        break;
                }

            }

            if( advance_to_next_node ) {
                target_node = scene_jump_calculate_next( scene );
            }
        } break;

        case NodeType::FADE: {
        } break;
        case NodeType::CONTROL:
        case NodeType::WRITE:
        case NodeType::NONE:
        case NodeType::COUNT:
            break;
    }

    if( !s->is_paused ) {
        if( scene_transition_finished ) {
            Rectangle src = COORD_PAUSE_BUTTON;
            Rectangle dst = {};

            dst.x = 20.0f;
            dst.y = 400.0f;

            *(Vector2*)&dst.width = *(Vector2*)&src.width * 2.0f;

            Color tint = Color{127, 127, 127, 255};
            if( CheckCollisionPointRec( mouse, dst ) ) {
                tint = WHITE;

                if( IsMouseButtonPressed( MOUSE_BUTTON_LEFT ) ) {
                    s->is_paused = true;
                }
            }

            DrawTexturePro( tex_menu, src, dst, {}, 0.0f, tint );
        } else {
            String title = scene->title.to_string( scene->string );
            if( title.buf ) {
                draw_scene_title( state->common.font, title.buf, s->scene_change_timer / SCENE_TRANSITION_TIME );
            }
        }
    }

    if( s->is_paused ) {
        Rectangle dim = { 0, 0 };
        *(Vector2*)&dim.width = *(Vector2*)&screen;
        DrawRectangleRec( dim, Color{0, 0, 0, 200} );

        Rectangle src, dst;
        src = dst = {};

        src = COORD_PAUSE_LOGO;
        
        *(Vector2*)&dst.width = *(Vector2*)&src.width * 2.0f;

        dst.x = (screen.x / 2.0f) - (dst.width);
        dst.y = (screen.y / 2.0f) - (dst.height / 2.0f);

        DrawTexturePro( tex_menu, src, dst, {}, 0.0f, WHITE );

        Rectangle act_src, act_dst;

        act_src = src;
        act_dst = dst;

        act_src = COORD_PAUSE_ACT;
        int which_act = s->kv.read( "act" );
        act_src.y += act_src.height * which_act;

        *(Vector2*)&act_dst.width = *(Vector2*)&act_src.width * 2.0f;

        act_dst.x +=  94.0f;
        act_dst.y  = ((dst.y + dst.height) - act_dst.height) - 12.0f;

        DrawTexturePro( tex_menu, act_src, act_dst, {}, 0.0f, WHITE );

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

        Vector2 button_position = {
            (dst.x + ((dst.width / 4.0f) * 3.0f)) - 8.0f,
            dst.y + 36.0f
        };

        _readonly int MAX_BUTTONS = 4;

        for( int i = 0; i < MAX_BUTTONS; ++i ) {
            // skip save button for now
            if( i == 2 ) {
                continue;
            }
            Rectangle rect = draw_button( button_position.x, button_position.y, i );
            if( CheckCollisionPointRec( mouse, rect ) && !s->is_settings_open ) {
                s->anim[i].set_once( ANIM_BUTTON_PLAY_SELECT + (i * 2) );

                if( IsMouseButtonPressed( MOUSE_BUTTON_LEFT ) ) {
                    switch( i ) {
                        // resume
                        case 0: {
                            s->is_paused = false;
                        } break;
                        // settings
                        case 1: {
                            s->is_settings_open = true;
                        } break;
                        // save
                        case 2: {

                        } break;
                        // quit
                        case 3: {
                            state->type = StateType::MAIN_MENU;
                        } break;
                    }
                }
            } else {
                s->anim[i].set_once( ANIM_BUTTON_PLAY_DESELECT + (i * 2) );
            }
            button_position.y += rect.height;
        }
    }

    if( s->is_settings_open ) {
        draw_settings( &state->common.settings, state->common.font, &s->is_settings_open );
    }

    int new_music = s->kv.read( "music" );
    if( s->current_music != new_music ) {
        if( s->current_music >= 0 ) {
            StopMusicStream( s->music[s->current_music] );
        }

        s->current_music = new_music;
        TraceLog( LOG_INFO, "Switched to music: %s", __MUSIC_PATHS[new_music] );

        PlayMusicStream( s->music[s->current_music] );
    }
    if( s->last_music_volume != volume_music ) {
        if( s->current_music >= 0 ) {
            SetMusicVolume( s->music[s->current_music], volume_music );
        }
    }

    EndDrawing();

    // NOTE(alicia): post -------------------------------------------

    s->scene_id = scene->id;
    s->node_id  = scene->current_node;

    if( !s->is_paused ) {
        s->elapsed            += dt;
        s->scene_change_timer += dt;
        scene->current_node = target_node;

        if( node && node->type == NodeType::FADE ) {
            if( s->fade_is_reverse ) {
                s->fade_timer -= dt;
            } else {
                s->fade_timer += dt;
            }
        }
    }

    if( s->kv.read( "one-playthrough" ) ) {
        state->common.game_finished_once = true;
    }

    if( !s->kv.read( "start-game" ) || s->kv.read( "game-finished" ) ) {
        state->type = StateType::MAIN_MENU;
    }

}

void draw_scene_title( Font font, const char* scene_name, float percent ) {
    Rectangle screen_rect = { 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() };

    Color background = BLACK;

    float t = percent;
    if( percent > 0.8f ) {
        t = 1.0f - ((t - 0.8f) / 0.2f);
    } else {
        t = 1.0f;
    }

    background = ColorAlpha( background, Lerp( 0.0f, 0.9f, t ) );

    DrawRectangleRec(screen_rect, background);

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

    for( size_t i = 0; i < ARRAY_LEN(s->characters); ++i ) {
        s->characters[i].tint = WHITE;
    }

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

    for( int i = 0; i < MUS_COUNT; ++i ) {
        auto* music = state->game.music + i;
        *music = LoadMusicStream( __MUSIC_PATHS[i] );
    }

    s->scene.current_node = START_NODE;

    s->current_music = -1;
    s->kv.write( "music", 1 );
    s->kv.write( "start-game", 1 );
}
void _game_unload( State* state ) {
    auto* s = &state->game;
    for( int i = 0; i < TEX_COUNT; ++i ) {
        UnloadTexture( s->textures[i] );
    }
    for( int i = 0; i < MUS_COUNT; ++i ) {
        if( i == s->current_music ) {
            StopMusicStream( s->music[i] );
        }
        UnloadMusicStream( s->music[i] );
    }
    s->scene.free();
    s->kv.free();
    s->buttons.free();
}

void ButtonList::push( String text ) {
    Button button = {};
    if( text.buf && text.len ) {
        button.text = string_offset_push( &string, text );
    }
    button.animation.set_once( ANIM_BUTTON_GENERIC_DESELECT );
    button.animation.speed = 10.0f;

    buttons.push( button );
}
int ButtonList::update_and_draw(
    Font font, Texture* textures, Vector2 screen, Vector2 mouse, bool left_pressed, float dt
) {
    int result = -1;

    // NOTE(alicia): negative inf
    float max_width = -1.0f / 0.0f;
    float height    = font.baseSize * (29.0f / 16.0f);

    for( int i = 0; i < buttons.len; ++i ) {
        auto& button = buttons[i];

        if( button.text.len ) {
            String text = button.text.to_string( string );

            Vector2 size = MeasureTextEx( font, text.buf, font.baseSize, 1.0f );

            if( size.x > max_width ) {
                max_width = size.x;
            }
        }
    }

    _readonly float BUTTON_PADDING_X = 40.0f;
    _readonly float BUTTON_PADDING_Y = 10.0f;

    Rectangle button_rect = {};
    button_rect.width  = max_width + BUTTON_PADDING_X;
    button_rect.height = height + BUTTON_PADDING_Y;

    button_rect.x = (screen.x / 2.0f) - (button_rect.width / 2.0f);
    button_rect.y = 80.0f;

    float margin = 16.0f;

    for( int i = 0; i < buttons.len; ++i ) {
        auto& button = buttons[i];

        bool is_hovering = CheckCollisionPointRec( mouse, button_rect );

        const Animation* anim_middle = nullptr;
        if( is_hovering ) {
            button.animation.set_once( ANIM_BUTTON_GENERIC_EDGE_SELECT );
            anim_middle = &animation_get( ANIM_BUTTON_GENERIC_MIDDLE_SELECT );
        } else {
            button.animation.set_once( ANIM_BUTTON_GENERIC_EDGE_DESELECT );
            anim_middle = &animation_get( ANIM_BUTTON_GENERIC_MIDDLE_DESELECT );
        }

        auto frame = button.animation.update( dt );
        auto tex   = textures[frame.texture];

        if( is_hovering && left_pressed ) {
            result = i;
        }

        Rectangle src = {}, dst = {};
        src = frame.src;

        dst = button_rect;

        dst.width  = button_rect.height;
        dst.x     -= dst.width;

        DrawTexturePro( tex, src, dst, {}, 0.0f, WHITE );

        dst.x = button_rect.x + button_rect.width;
        src.width = -src.width;

        DrawTexturePro( tex, src, dst, {}, 0.0f, WHITE );

        src = anim_middle->frames[button.animation.frame].src;
        dst = button_rect;

        DrawTexturePro( tex, src, dst, {}, 0.0f, WHITE );

        String text = button.text.to_string( string );

        Vector2 text_size = MeasureTextEx( font, text.buf, font.baseSize, 1.0f );

        Vector2 text_position = {};
        text_position.x = (button_rect.x + (button_rect.width / 2.0f)) - (text_size.x / 2.0f);
        text_position.y = (button_rect.y + (button_rect.height / 2.0f)) - (text_size.y / 2.0f);

        DrawTextPro( font, text.buf, text_position, {}, 0.0f, font.baseSize, 1.0f, WHITE );

        button_rect.y += margin + button_rect.height;
    }

    return result;
}

