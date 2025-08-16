/**
 * @file   ui.cpp
 * @brief  UI implementation.
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   August 08, 2025
*/
#include "bog/ui.h"
#include "bog/state.h"

struct StateUI {
    static constexpr float TEXT_BASE_TIME = 0.005f;

    float text_display_speed = 1.0f;

    List<UI_Word> words;
} __UI = {};

Rectangle draw_settings( Settings* settings, Font font, bool* is_open ) {
    Rectangle rect = {};

    Vector2 screen = { (float)GetScreenWidth(), (float)GetScreenHeight() };
    Vector2 mouse  = GetMousePosition();

    rect.width  = 480.0f;
    rect.height = 300.0f;

    rect.x = (screen.x / 2.0f) - (rect.width / 2.0f);
    rect.y = (screen.y / 2.0f) - (rect.height / 2.0f);

    DrawRectangleRec( rect, Color{ 40, 4, 16, 255 });
    DrawRectangleRec( padding( rect, 10.0f ), Color{ 249, 29, 100, 255 });

    Vector2 title_size = MeasureTextEx( font, "Settings", font.baseSize, 1.0f );

    Vector2 title_position = {};
    title_position.x = rect.x + ((rect.width / 2.0f) - (title_size.x / 2.0f));
    title_position.y = rect.y + 20.0f;

    DrawTextPro( font, "Settings", title_position, {}, 0.0f, font.baseSize, 1.0f, WHITE );

    Rectangle bar  = {};
    Rectangle fill = {};
    Rectangle control_area = {};
    float fill_padding = 5.0f;
    float text_padding = 24.0f;

    Vector2 pos = title_position;
    pos.y += 40.0f + text_padding;
    pos.x = rect.x + 40.0f;

    *(Vector2*)&bar.x = *(Vector2*)&pos.x;

    bar.x      = pos.x + 150.0f;
    bar.width  = (rect.x + (rect.width - 40.0f)) - bar.x;
    bar.height = font.baseSize;

    DrawTextPro( font, "Volume: ", pos, {}, 0.0f, font.baseSize, 1.0f, WHITE );
    DrawRectangleRec( bar, BLACK );
    control_area = fill = padding( bar, fill_padding );
    fill.width *= settings->volume;
    DrawRectangleRec( fill, WHITE );

    if( CheckCollisionPointRec( mouse, control_area ) ) {
        float delta = (mouse.x - control_area.x) / control_area.width;

        if( IsMouseButtonDown( MOUSE_BUTTON_LEFT ) ) {
            settings->volume = delta;
        }
    }

    bar.y = pos.y += font.baseSize + text_padding;

    DrawTextPro( font, "SFX: ", pos, {}, 0.0f, font.baseSize, 1.0f, WHITE );
    DrawRectangleRec( bar, BLACK );
    control_area = fill = padding( bar, fill_padding );
    fill.width *= settings->sfx;
    DrawRectangleRec( fill, WHITE );

    if( CheckCollisionPointRec( mouse, control_area ) ) {
        float delta = (mouse.x - control_area.x) / control_area.width;

        if( IsMouseButtonDown( MOUSE_BUTTON_LEFT ) ) {
            settings->sfx = delta;
        }
    }

    bar.y = pos.y += font.baseSize + text_padding;

    DrawTextPro( font, "Music: ", pos, {}, 0.0f, font.baseSize, 1.0f, WHITE );
    DrawRectangleRec( bar, BLACK );
    control_area = fill = padding( bar, fill_padding );
    fill.width *= settings->music;
    DrawRectangleRec( fill, WHITE );

    if( CheckCollisionPointRec( mouse, control_area ) ) {
        float delta = (mouse.x - control_area.x) / control_area.width;

        if( IsMouseButtonDown( MOUSE_BUTTON_LEFT ) ) {
            settings->music = delta;
        }
    }

    bar.y = pos.y += font.baseSize + text_padding;

    const char* return_text = "Close";
    Vector2 return_size = MeasureTextEx( font, return_text, font.baseSize, 1.0f );
    if( CheckCollisionPointRec( mouse, { pos.x, pos.y, return_size.x, return_size.y } ) ) {
        DrawTextPro( font, return_text, pos, {}, 0.0f, font.baseSize, 1.0f, WHITE );

        if( IsMouseButtonPressed( MOUSE_BUTTON_LEFT ) ) {
            *is_open = false;
        }
    } else {
        DrawTextPro( font, return_text, pos, {}, 0.0f, font.baseSize, 1.0f, Color{ 200, 200, 255, 170 });
    }
    bar.y = pos.y += font.baseSize + text_padding;

    return rect;
}

Rectangle text_box_draw( Texture tex, float y_offset, float height, Rectangle* out ) {
    Vector2 screen = { (float)GetScreenWidth(), (float)GetScreenHeight() };

    Rectangle src_trim_top, dst_trim_top;

    src_trim_top = COORD_TEXT_BOX_TRIM_TOP;

    *(Vector2*)&dst_trim_top.width = fit_to_dst( screen, *(Vector2*)&src_trim_top.width );
    dst_trim_top.x = 0.0f;
    dst_trim_top.y = y_offset - dst_trim_top.height - TEXT_BOX_PADDING.x;

    Rectangle src_trim_bot, dst_trim_bot;

    src_trim_bot = COORD_TEXT_BOX_TRIM_BOT;

    *(Vector2*)&dst_trim_bot.width = fit_to_dst( screen, *(Vector2*)&src_trim_bot.width );
    dst_trim_bot.x = 0.0f;
    dst_trim_bot.y = (y_offset + height) + TEXT_BOX_PADDING.z;

    Rectangle background = {};
    background.x = 0.0f;
    background.y = dst_trim_top.y + ( dst_trim_top.height - (dst_trim_top.height / 4.0f));
    background.width = screen.x;
    background.height = dst_trim_bot.y - background.y;

    DrawRectangleRec( background, COLOR_TEXT_BOX_BACKGROUND );
    DrawTexturePro( tex, src_trim_top, dst_trim_top, {}, 0.0f, WHITE );
    DrawTexturePro( tex, src_trim_bot, dst_trim_bot, {}, 0.0f, WHITE );

    Rectangle text_area = { 0.0f, y_offset, screen.x, height };

    if( out ) {
        *out = {
            0.0f,
            dst_trim_top.y,
            screen.x,
            (dst_trim_bot.y + dst_trim_bot.height) - dst_trim_top.y
        };
    }

    return padding( text_area, 0.0f, TEXT_BOX_PADDING.y, 0.0f, TEXT_BOX_PADDING.w );
}

float text_set_display_speed( float speed ) {
    return __UI.text_display_speed = speed;
}
float text_get_display_speed() {
    return __UI.text_display_speed;
}

float text_display_time() {
    return __UI.text_display_speed * __UI.TEXT_BASE_TIME;
}

// NOTE(alicia): largely from raylib/MeasureTextEx
Vector2 text_measure_slice( Font font, String string, float font_size, float spacing ) {
    char*   text      = string.buf;
    Vector2 text_size = {};

    int size              = string.len;
    int temp_byte_counter = 0;
    int byte_counter      = 0;

    float text_width      = 0.0f;
    float temp_text_width = 0.0f;

    float text_height = font_size;
    float scale_factor = font_size / (float)font.baseSize;

    int letter = 0;
    int index  = 0;

    for( int i = 0; i < size; ) {
        byte_counter++;

        int codepoint_byte_count = 0;
        letter = GetCodepointNext( &text[i], &codepoint_byte_count );
        index  = GetGlyphIndex( font, letter );

        i += codepoint_byte_count;

        if( letter != '\n' ) {
            if( font.glyphs[index].advanceX > 0 ) {
                text_width += font.glyphs[index].advanceX;
            } else {
                text_width += (font.recs[index].width + font.glyphs[index].offsetX);
            }
        } else {
            if( temp_text_width < text_width ) {
                temp_text_width = text_width;
            }
            byte_counter = 0;
            text_width   = 0;

            text_height += (font_size + 2);
        }

        if( temp_byte_counter < byte_counter ) {
            temp_byte_counter = byte_counter;
        }
    }

    if( temp_text_width < text_width ) {
        temp_text_width = text_width;
    }

    text_size.x = temp_text_width*scale_factor + (float)( (temp_byte_counter - 1) * spacing );
    text_size.y = text_height;

    return text_size;
}
int text_split_words( Font font, String text, float font_size, List<UI_Word>* out_words ) {
    int word_count = 0;
    String str = text;

    auto push_word = [&font,&word_count,out_words,font_size]( String str ) {
        UI_Word word = {};

        word.type       = UI_WordType::TEXT;
        word.text.value = str;
        word.text.size  = text_measure_slice( font, word.text.value, font_size );

        word_count++;
        out_words->push( word );
    };

    while( str.len ) {
        String substr = str;

        bool whitespace_found = false;
        if( find_set( str, " \n", &substr.len ) ) {
            whitespace_found = true;
        }

        str = advance( str, substr.len + 1 );
        if( whitespace_found ) {
            // include the space/new-line in the string
            substr.len++;
        }

        while( substr.len ) {
            int cmd_start = 0;
            if( find_char( substr, '<', &cmd_start ) ) {

                int cmd_end = 0;
                if( !find_char( substr, '>', &cmd_end ) ) {
                    push_word( substr );
                    substr.len = 0;
                    continue;
                }

                String before_cmd = truncate( substr, cmd_start );
                if( before_cmd.len ) {
                    push_word( before_cmd );
                }

                String cmd = advance( truncate( substr, cmd_end ), cmd_start + 1 );

                if( find_string( cmd, "rgba:" ) ) {
                    cmd = advance( cmd, "rgba:" );

                    UI_Word word = {};
                    word.type                = UI_WordType::COMMAND_COLOR;
                    word.command_color.color = parse_color( cmd );

                    word_count++;
                    out_words->push( word );
                }

                substr = advance( substr, cmd_end + 1 );
            } else {
                push_word( substr );
                substr.len = 0;
            }
        }

    }

    return word_count;
}

struct TextCommandState {
    Color tint = WHITE;
};

Vector2 __word_draw(
    Font              font,
    UI_Word           word,
    Vector2           position,
    float             font_size,
    TextCommandState* state
) {
    switch( word.type ) {
        case UI_WordType::TEXT: {
            Vector2 pos = position;

            String str = word.text.value;
            for( int i = 0; i < str.len; ++i ) {
                auto info = GetGlyphInfo( font, str[i] );
                DrawTextCodepoint( font, str[i], pos, font_size, state->tint );

                pos.x += info.advanceX;
            }

            return pos;
        } break;
        case UI_WordType::COMMAND_COLOR: {
            state->tint = word.command_color.color;
        } break;
    }

    return position;
}

Rectangle text_measure( Font font, String string, Vector2 position ) {
    float font_size = font.baseSize;

    Rectangle bounds = { 0.0f, 0.0f, (float)GetScreenWidth(), (float)GetScreenHeight() };

    float start_x = position.x;
    if( start_x < bounds.x ) {
        start_x = bounds.x;
    }
    float start_y = position.y;
    if( start_y < bounds.y ) {
        start_y = bounds.y;
    }

    Rectangle rect = {};
    *(Vector2*)&rect.x = { start_x, start_y };

    __UI.words.reset();
    if( !text_split_words( font, string, font_size, &__UI.words ) ) {
        return rect;
    }

    int max_chars = string.len;

    float max_x = start_x, max_y = start_y;

    Rectangle word_rect = {};
    *(Vector2*)&word_rect.x = { start_x, start_y };

    for( int i = 0; i < __UI.words.len; ++i ) {
        if( !max_chars ) {
            break;
        }
        auto* word = __UI.words + i;

        switch( word->type ) {
            case UI_WordType::TEXT: {
                *(Vector2*)&word_rect.width = word->text.size;

                if( (word_rect.x + word_rect.width) >= (bounds.x + bounds.width) ) {
                    word_rect.x = start_x;
                    word_rect.y += word_rect.height;
                }

                if( word->text.value.len > max_chars ) {
                    word->text.value.len = max_chars;

                    word->text.size = text_measure_slice( font, word->text.value, font_size );
                    *(Vector2*)&word_rect.width = word->text.size;
                }

                float end_x, end_y;
                end_x = (word_rect.x + word_rect.width);
                end_y = (word_rect.y + word_rect.height);
                if( end_x > max_x ) {
                    max_x = end_x;
                }
                if( end_y > max_y ) {
                    max_y = end_y;
                }

                max_chars -= word->text.value.len;

                word_rect.x += word_rect.width;
            } break;
            default:
                break;
        }
    }

    rect.width  = max_x - start_x;
    rect.height = max_y - start_y;

    return rect;
}

Rectangle text_draw(
    Font              font,
    String            string,
    Vector2           position,
    Rectangle*        bounds_ptr,
    DisplayTextState* state,
    float             dt
) {
    TextCommandState cmd_state = {};
    float font_size            = font.baseSize;

    Rectangle bounds = { 0.0f, 0.0f, (float)GetScreenWidth(), (float)GetScreenHeight() };
    if( bounds_ptr ) {
        bounds = *bounds_ptr;
    }

    float start_x = position.x;
    if( start_x < bounds.x ) {
        start_x = bounds.x;
    }
    float start_y = position.y;
    if( start_y < bounds.y ) {
        start_y = bounds.y;
    }

    Rectangle rect = {};
    *(Vector2*)&rect.x = { start_x, start_y };

    __UI.words.reset();
    if( !text_split_words( font, string, font_size, &__UI.words ) ) {
        return rect;
    }

    int max_chars = string.len;
    if( state ) {
        if( state->len > string.len ) {
            max_chars = string.len;
        } else {
            max_chars = state->len;
        }

        state->timer += dt;
        if( state->timer >= text_display_time() ) {
            state->timer = 0.0f;
            state->len++;
        }
    }

    BeginScissorMode( bounds.x, bounds.y, bounds.width, bounds.height );

    float max_x = start_x, max_y = start_y;

    Rectangle word_rect = {};
    *(Vector2*)&word_rect.x = { start_x, start_y };

    for( int i = 0; i < __UI.words.len; ++i ) {
        if( !max_chars ) {
            break;
        }
        auto* word = __UI.words + i;

        switch( word->type ) {
            case UI_WordType::TEXT: {
                *(Vector2*)&word_rect.width = word->text.size;

                if( (word_rect.x + word_rect.width) >= (bounds.x + bounds.width) ) {
                    word_rect.x = start_x;
                    word_rect.y += word_rect.height;
                }

                if( word->text.value.len > max_chars ) {
                    word->text.value.len = max_chars;

                    word->text.size = text_measure_slice( font, word->text.value, font_size );
                    *(Vector2*)&word_rect.width = word->text.size;
                }

                float end_x, end_y;
                end_x = (word_rect.x + word_rect.width);
                end_y = (word_rect.y + word_rect.height);
                if( end_x > max_x ) {
                    max_x = end_x;
                }
                if( end_y > max_y ) {
                    max_y = end_y;
                }

                __word_draw( font, *word, *(Vector2*)&word_rect.x, font_size, &cmd_state );

                max_chars -= word->text.value.len;

                word_rect.x += word_rect.width;
            } break;
            case UI_WordType::COMMAND_COLOR: {
                __word_draw( font, *word, *(Vector2*)&word_rect.x, font_size, &cmd_state );
            } break;
        }
    }

    EndScissorMode();

    rect.width  = max_x - start_x;
    rect.height = max_y - start_y;

    return rect;
}

Vector2 fit_to_dst( Vector2 dst, Vector2 size ) {
    Vector2 result = {};

    if( size.x > size.y ) {
        float aspect = size.y / size.x;

        result.x = dst.x;
        result.y = dst.x * aspect;
    } else {
        float aspect = size.x / size.y;

        result.x = dst.y * aspect;
        result.y = dst.y;
    }

    return result;
}

