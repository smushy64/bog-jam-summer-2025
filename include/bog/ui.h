#if !defined(BOG_UI_H)
#define BOG_UI_H
/**
 * @file   ui.h
 * @brief  UI.
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   August 08, 2025
*/
#include "bog/prelude.h"
#include "bog/collections.h"
#include "bog/constants.h"

struct Settings;

enum class UI_WordType {
    TEXT,
    COMMAND_COLOR
};

struct UI_Word {
    UI_WordType type;
    union {
        struct {
            String  value;
            Vector2 size;
        } text;
        struct {
            Color color;
        } command_color;
    };
};

struct DisplayTextState {
    float timer;
    int   len;

    bool is_complete( String string ) const {
        return len >= string.len;
    }
};

Rectangle draw_settings( Settings* settings, Font font, bool* is_open );
Rectangle draw_credits( Font font, bool* is_open );

Rectangle text_box_draw(
    Texture tex, float y_offset, float height, Rectangle* out_area = nullptr );

float text_set_display_speed( float speed = 1.0f );
float text_get_display_speed();

Vector2 text_measure_slice(
    Font   font,
    String text,
    float  font_size,
    float  opt_spacing = 1.0f );

int text_split_words( Font font, String text, float font_size, List<UI_Word>* out_words );

Rectangle text_measure( Font font, String string, Vector2 position );

Rectangle text_draw(
    Font              font,
    String            string,
    Vector2           position,
    Rectangle*        opt_bounds = nullptr,
    DisplayTextState* opt_state  = nullptr,
    float             dt         = 0.0f );

Vector2 fit_to_dst( Vector2 dst, Vector2 size );

inline
Rectangle rect_new() { return {}; }
inline
Rectangle rect_new( float x, float y, float width, float height ) { return { x, y, width, height }; }
inline
Rectangle rect_new( float x, float y, Vector2 size ) { return rect_new( x, y, size.x, size.y ); }
inline
Rectangle rect_new( Vector2 position, float width, float height ) {
    return rect_new( position.x, position.y, width, height );
}
inline
Rectangle rect_new( Vector2 position, Vector2 size ) {
    return rect_new( position.x, position.y, size.x, size.y );
}

inline
Rectangle margin( Rectangle rect, float top, float right, float bottom, float left ) {
    Rectangle result = rect;

    result.y      -= top;
    result.height += top;

    result.width += right;

    result.height += bottom;

    result.x     -= left;
    result.width += left;

    if( result.width < 0.0f ) {
        result.width = 0.0f;
    }
    if( result.height < 0.0f ) {
        result.height = 0.0f;
    }

    return result;
}
inline
Rectangle margin( Rectangle rect, float top, float horizontal, float bottom ) {
    return margin( rect, top, horizontal, bottom, horizontal );
}
inline
Rectangle margin( Rectangle rect, float vertical, float horizontal ) {
    return margin( rect, vertical, horizontal, vertical, horizontal );
}
inline
Rectangle margin( Rectangle rect, float margin ) {
    return ::margin( rect, margin, margin, margin, margin );
}

inline
Rectangle padding( Rectangle rect, float top, float right, float bottom, float left ) {
    return margin( rect, -top, -right, -bottom, -left );
}
inline
Rectangle padding( Rectangle rect, float top, float horizontal, float bottom ) {
    return margin( rect, -top, -horizontal, -bottom );
}
inline
Rectangle padding( Rectangle rect, float vertical, float horizontal ) {
    return margin( rect, -vertical, -horizontal );
}
inline
Rectangle padding( Rectangle rect, float padding ) {
    return margin( rect, -padding );
}

#endif /* header guard */
