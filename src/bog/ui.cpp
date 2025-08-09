/**
 * @file   ui.cpp
 * @brief  UI implementation.
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   August 08, 2025
*/
#include "bog/ui.h"

struct StateUI {
    static constexpr float TEXT_BASE_TIME = 0.005f;

    float text_display_speed = 1.0f;

    List<UI_Word> words;
} __UI = {};

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

Rectangle text_draw(
    Font              font,
    String            string,
    Vector2           position,
    Rectangle*        bounds_ptr,
    DisplayTextState* state
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

        state->timer += GetFrameTime();
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

