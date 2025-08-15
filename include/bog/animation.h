#if !defined(BOG_ANIMATION_H)
#define BOG_ANIMATION_H
/**
 * @file   animation.h
 * @brief  Animations.
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   August 14, 2025
*/
#include "bog/prelude.h"
#include "bog/constants.h"
#include "bog/collections.h"

struct AnimationFrame;
struct Animation;

constexpr
float animation_calculate_length( int count, const AnimationFrame* frames );

const Animation& animation_get( int animation );

String animation_to_string( int animation );
bool string_to_animation( String string, int* out );

enum AnimationCap {
    ANIM_NONE,

    ANIM_BUTTON_GENERIC_SELECT,
    ANIM_BUTTON_GENERIC_DESELECT,

    ANIM_BUTTON_PLAY_BIG_SELECT,
    ANIM_BUTTON_PLAY_BIG_DESELECT,

    ANIM_BUTTON_SETTINGS_BIG_SELECT,
    ANIM_BUTTON_SETTINGS_BIG_DESELECT,

    ANIM_BUTTON_CREDITS_BIG_SELECT,
    ANIM_BUTTON_CREDITS_BIG_DESELECT,

    ANIM_BUTTON_QUIT_BIG_SELECT,
    ANIM_BUTTON_QUIT_BIG_DESELECT,

    ANIM_BUTTON_PLAY_SELECT,
    ANIM_BUTTON_PLAY_DESELECT,

    ANIM_BUTTON_SETTINGS_SELECT,
    ANIM_BUTTON_SETTINGS_DESELECT,

    ANIM_BUTTON_CREDITS_SELECT,
    ANIM_BUTTON_CREDITS_DESELECT,

    ANIM_BUTTON_QUIT_SELECT,
    ANIM_BUTTON_QUIT_DESELECT,

    ANIM_COUNT
};

struct AnimationFrame {
    Rectangle src;
    int       texture = TEX_MENU;
    float     time    = 0.06f;
};

struct Animation {
    int                   frame_count;
    const AnimationFrame* frames;
    float                 length;
};

enum {
    ANIM_OP_ONCE,
    ANIM_OP_LOOP,

    ANIM_OP_COUNT
};

struct AnimationTimeline {
    int animation;
    int frame;
    int op;

    float timer;
    float total_timer;
    float speed;

    float progress() {
        const auto& anim = animation_get( animation );

        float length = animation_calculate_length( anim.frame_count, anim.frames );
        if( !length || !speed ) {
            return 1.0f;
        }

        return total_timer / (length * (1.0f / speed));
    }

    AnimationFrame get_frame( const Animation* anim = nullptr ) {
        if( !anim ) {
            anim = &animation_get( animation );
        }
        if( !anim->frame_count ) {
            return {};
        }

        return anim->frames[this->frame];
    }

    AnimationFrame update( float dt ) {
        const auto& anim = animation_get( animation );
        auto src = get_frame( &anim );

        timer       += dt * speed;
        total_timer += dt * speed;
        if( timer >= anim.length ) {
            timer = 0.0f;
            switch( op ) {
                case ANIM_OP_ONCE: {
                    frame++;
                    if( frame >= anim.frame_count ) {
                        frame = (anim.frame_count ? anim.frame_count - 1 : 0);
                    }
                } break;
                case ANIM_OP_LOOP: {
                    frame++;
                    if( frame >= anim.frame_count ) {
                        frame = 0;
                    }
                } break;
                default:
                    break;
            }
        }
        return src;
    }

    int set( int animation, int op = ANIM_OP_ONCE ) {
        this->animation = animation;
        if( this->animation >= ANIM_COUNT ) {
            this->animation = ANIM_NONE;
        }
        this->op = op;

        this->reset();
        return animation;
    }
    int set_once( int animation, int op = ANIM_OP_ONCE ) {
        if( this->animation == animation ) {
            return animation;
        }
        return set( animation, op );
    }

    void reset() {
        frame       = 0;
        timer       = 0.0f;
        total_timer = 0.0f;
    }

    bool is_complete() {
        switch( op ) {
            case ANIM_OP_ONCE: {
                const auto& anim = animation_get( animation );
                return frame >= (anim.frame_count ? anim.frame_count - 1 : 0);
            } break;
            default:
                break;
        }
        return false;
    }
};

inline constexpr
float animation_calculate_length( int count, const AnimationFrame* frames ) {
    float result = 0.0f;

    for( int i = 0; i < count; ++i ) {
        result += frames[i].time;
    }

    return result;
}

inline
String animation_to_string( int animation ) {
    switch( (AnimationCap)animation ) {
        case ANIM_BUTTON_GENERIC_SELECT        : return "button_generic_select";
        case ANIM_BUTTON_GENERIC_DESELECT      : return "button_generic_deselect";
        case ANIM_BUTTON_PLAY_BIG_SELECT       : return "button_play_big_select";
        case ANIM_BUTTON_PLAY_BIG_DESELECT     : return "button_play_big_deselect";
        case ANIM_BUTTON_SETTINGS_BIG_SELECT   : return "button_settings_big_select";
        case ANIM_BUTTON_SETTINGS_BIG_DESELECT : return "button_settings_big_deselect";
        case ANIM_BUTTON_CREDITS_BIG_SELECT    : return "button_credits_big_select";
        case ANIM_BUTTON_CREDITS_BIG_DESELECT  : return "button_credits_big_deselect";
        case ANIM_BUTTON_QUIT_BIG_SELECT       : return "button_quit_big_select";
        case ANIM_BUTTON_QUIT_BIG_DESELECT     : return "button_quit_big_deselect";
        case ANIM_BUTTON_PLAY_SELECT           : return "button_play_select";
        case ANIM_BUTTON_PLAY_DESELECT         : return "button_play_deselect";
        case ANIM_BUTTON_SETTINGS_SELECT       : return "button_settings_select";
        case ANIM_BUTTON_SETTINGS_DESELECT     : return "button_settings_deselect";
        case ANIM_BUTTON_CREDITS_SELECT        : return "button_credits_select";
        case ANIM_BUTTON_CREDITS_DESELECT      : return "button_credits_deselect";
        case ANIM_BUTTON_QUIT_SELECT           : return "button_quit_select";
        case ANIM_BUTTON_QUIT_DESELECT         : return "button_quit_deselect";

        case ANIM_NONE:
        case ANIM_COUNT:
            break;
    }
    return "";
}
inline
bool string_to_animation( String string, int* out ) {
    for( int i = ANIM_NONE + 1; i < ANIM_COUNT; ++i ) {
        if( string_cmp( string, animation_to_string( i ) ) ) {
            *out = i;
            return true;
        }
    }
    return false;
}

_readonly AnimationFrame __ANIM_NONE[] = {
    { {0, 0, 0, 0}, 0, 0.0f }
};

_readonly AnimationFrame __ANIM_BUTTON_GENERIC_SELECT[] = {
    {
        { 1304, 0, 136, 16 }, /* src rect */
    },
    {
        { 1304, 16, 136, 16 }, /* src rect */
    },
    {
        { 1304, 16 + 16, 136, 16 }, /* src rect */
    },
    {
        { 1304, 16 + 16 + 16, 136, 16 }, /* src rect */
    },
    {
        { 1304, 16 + 16 + 16 + 16, 136, 16 }, /* src rect */
    },
};

_readonly AnimationFrame __ANIM_BUTTON_GENERIC_DESELECT[] = {
    {
        { 1304, 16 + 16 + 16 + 16, 136, 16 }, /* src rect */
    },
    {
        { 1304, 16 + 16 + 16, 136, 16 }, /* src rect */
    },
    {
        { 1304, 16 + 16, 136, 16 }, /* src rect */
    },
    {
        { 1304, 16, 136, 16 }, /* src rect */
    },
    {
        { 1304, 0, 136, 16 }, /* src rect */
    },
};

_readonly AnimationFrame __ANIM_BUTTON_PLAY_BIG_SELECT[] = {
    {
        { 0, 330, 180, 23 },
    },
    {
        { 0 + 180, 330, 180, 23 },
    },
    {
        { 0 + 180 + 180, 330, 180, 23 },
    },
    {
        { 0 + 180 + 180 + 180, 330, 180, 23 },
    },
    {
        { 0 + 180 + 180 + 180 + 180, 330, 180, 23 },
    },
};

_readonly AnimationFrame __ANIM_BUTTON_PLAY_BIG_DESELECT[] = {
    {
        { 0 + 180 + 180 + 180 + 180, 330, 180, 23 },
    },
    {
        { 0 + 180 + 180 + 180 + 180 + 180, 330, 180, 23 },
    },
    {
        { 0 + 180 + 180 + 180 + 180 + 180 + 180, 330, 180, 23 },
    },
    {
        { 0 + 180 + 180 + 180 + 180 + 180 + 180 + 180, 330, 180, 23 },
    },
    {
        { 0, 330, 180, 23 },
    },
};

_readonly AnimationFrame __ANIM_BUTTON_SETTINGS_BIG_SELECT[] = {
    {
        { 0, 330 + 23, 180, 23 },
    },
    {
        { 0 + 180, 330 + 23, 180, 23 },
    },
    {
        { 0 + 180 + 180, 330 + 23, 180, 23 },
    },
    {
        { 0 + 180 + 180 + 180, 330 + 23, 180, 23 },
    },
    {
        { 0 + 180 + 180 + 180 + 180, 330 + 23, 180, 23 },
    },
};

_readonly AnimationFrame __ANIM_BUTTON_SETTINGS_BIG_DESELECT[] = {
    {
        { 0 + 180 + 180 + 180 + 180, 330 + 23, 180, 23 },
    },
    {
        { 0 + 180 + 180 + 180 + 180 + 180, 330 + 23, 180, 23 },
    },
    {
        { 0 + 180 + 180 + 180 + 180 + 180 + 180, 330 + 23, 180, 23 },
    },
    {
        { 0 + 180 + 180 + 180 + 180 + 180 + 180 + 180, 330 + 23, 180, 23 },
    },
    {
        { 0, 330 + 23, 180, 23 },
    },
};

_readonly AnimationFrame __ANIM_BUTTON_CREDITS_BIG_SELECT[] = {
    {
        { 0, 330 + 23 + 23, 180, 23 },
    },
    {
        { 0 + 180, 330 + 23 + 23, 180, 23 },
    },
    {
        { 0 + 180 + 180, 330 + 23 + 23, 180, 23 },
    },
    {
        { 0 + 180 + 180 + 180, 330 + 23 + 23, 180, 23 },
    },
    {
        { 0 + 180 + 180 + 180 + 180, 330 + 23 + 23, 180, 23 },
    },
};

_readonly AnimationFrame __ANIM_BUTTON_CREDITS_BIG_DESELECT[] = {
    {
        { 0 + 180 + 180 + 180 + 180, 330 + 23 + 23, 180, 23 },
    },
    {
        { 0 + 180 + 180 + 180 + 180 + 180, 330 + 23 + 23, 180, 23 },
    },
    {
        { 0 + 180 + 180 + 180 + 180 + 180 + 180, 330 + 23 + 23, 180, 23 },
    },
    {
        { 0 + 180 + 180 + 180 + 180 + 180 + 180 + 180, 330 + 23 + 23, 180, 23 },
    },
    {
        { 0, 330 + 23 + 23, 180, 23 },
    },
};

_readonly AnimationFrame __ANIM_BUTTON_QUIT_BIG_SELECT[] = {
    {
        { 0, 330 + 23 + 23 + 23, 180, 23 },
    },
    {
        { 0 + 180, 330 + 23 + 23 + 23, 180, 23 },
    },
    {
        { 0 + 180 + 180, 330 + 23 + 23 + 23, 180, 23 },
    },
    {
        { 0 + 180 + 180 + 180, 330 + 23 + 23 + 23, 180, 23 },
    },
    {
        { 0 + 180 + 180 + 180 + 180, 330 + 23 + 23 + 23, 180, 23 },
    },
};

_readonly AnimationFrame __ANIM_BUTTON_QUIT_BIG_DESELECT[] = {
    {
        { 0 + 180 + 180 + 180 + 180, 330 + 23 + 23 + 23, 180, 23 },
    },
    {
        { 0 + 180 + 180 + 180 + 180 + 180, 330 + 23 + 23 + 23, 180, 23 },
    },
    {
        { 0 + 180 + 180 + 180 + 180 + 180 + 180, 330 + 23 + 23 + 23, 180, 23 },
    },
    {
        { 0 + 180 + 180 + 180 + 180 + 180 + 180 + 180, 330 + 23 + 23 + 23, 180, 23 },
    },
    {
        { 0, 330 + 23 + 23 + 23, 180, 23 },
    },
};

_readonly AnimationFrame __ANIM_BUTTON_PLAY_SELECT[] = {
    {
        { 0, 543, 82, 12 }
    },
    {
        { 0 + 82, 543, 82, 12 }
    },
    {
        { 0 + 82 + 82, 543, 82, 12 }
    },
    {
        { 0 + 82 + 82 + 82, 543, 82, 12 }
    },
};
_readonly AnimationFrame __ANIM_BUTTON_PLAY_DESELECT[] = {
    {
        { 0 + 82 + 82 + 82, 543, 82, 12 }
    },
    {
        { 0 + 82 + 82 + 82 + 82, 543, 82, 12 }
    },
    {
        { 0 + 82 + 82 + 82 + 82 + 82, 543, 82, 12 }
    },
    {
        { 0 + 82 + 82 + 82 + 82 + 82 + 82, 543, 82, 12 }
    },
};

_readonly AnimationFrame __ANIM_BUTTON_SETTINGS_SELECT[] = {
    {
        { 0, 543 + 13, 82, 12 }
    },
    {
        { 0 + 82, 543 + 13, 82, 12 }
    },
    {
        { 0 + 82 + 82, 543 + 13, 82, 12 }
    },
    {
        { 0 + 82 + 82 + 82, 543 + 13, 82, 12 }
    },
};
_readonly AnimationFrame __ANIM_BUTTON_SETTINGS_DESELECT[] = {
    {
        { 0 + 82 + 82 + 82, 543 + 13, 82, 12 }
    },
    {
        { 0 + 82 + 82 + 82 + 82, 543 + 13, 82, 12 }
    },
    {
        { 0 + 82 + 82 + 82 + 82 + 82, 543 + 13, 82, 12 }
    },
    {
        { 0 + 82 + 82 + 82 + 82 + 82 + 82, 543 + 13, 82, 12 }
    },
};

_readonly AnimationFrame __ANIM_BUTTON_CREDITS_SELECT[] = {
    {
        { 0, 543 + 13 + 13, 82, 12 }
    },
    {
        { 0 + 82, 543 + 13 + 13, 82, 12 }
    },
    {
        { 0 + 82 + 82, 543 + 13 + 13, 82, 12 }
    },
    {
        { 0 + 82 + 82 + 82, 543 + 13 + 13, 82, 12 }
    },
};
_readonly AnimationFrame __ANIM_BUTTON_CREDITS_DESELECT[] = {
    {
        { 0 + 82 + 82 + 82, 543 + 13 + 13, 82, 12 }
    },
    {
        { 0 + 82 + 82 + 82 + 82, 543 + 13 + 13, 82, 12 }
    },
    {
        { 0 + 82 + 82 + 82 + 82 + 82, 543 + 13 + 13, 82, 12 }
    },
    {
        { 0 + 82 + 82 + 82 + 82 + 82 + 82, 543 + 13 + 13, 82, 12 }
    },
};

_readonly AnimationFrame __ANIM_BUTTON_QUIT_SELECT[] = {
    {
        { 0, 543 + 13 + 13 + 13, 82, 12 }
    },
    {
        { 0 + 82, 543 + 13 + 13 + 13, 82, 12 }
    },
    {
        { 0 + 82 + 82, 543 + 13 + 13 + 13, 82, 12 }
    },
    {
        { 0 + 82 + 82 + 82, 543 + 13 + 13 + 13, 82, 12 }
    },
};
_readonly AnimationFrame __ANIM_BUTTON_QUIT_DESELECT[] = {
    {
        { 0 + 82 + 82 + 82, 543 + 13 + 13 + 13, 82, 12 }
    },
    {
        { 0 + 82 + 82 + 82 + 82, 543 + 13 + 13 + 13, 82, 12 }
    },
    {
        { 0 + 82 + 82 + 82 + 82 + 82, 543 + 13 + 13 + 13, 82, 12 }
    },
    {
        { 0 + 82 + 82 + 82 + 82 + 82 + 82, 543 + 13 + 13 + 13, 82, 12 }
    },
};

#define MAKE_ANIMATION(name) \
    { \
       .frame_count = ARRAY_LEN(__ANIM_##name), \
       .frames      = __ANIM_##name, \
       .length      = animation_calculate_length(ARRAY_LEN(__ANIM_##name), __ANIM_##name) \
    }

_readonly Animation __ANIMATIONS[] = {
    MAKE_ANIMATION(NONE),

    MAKE_ANIMATION(BUTTON_GENERIC_SELECT),
    MAKE_ANIMATION(BUTTON_GENERIC_DESELECT),

    MAKE_ANIMATION(BUTTON_PLAY_BIG_SELECT),
    MAKE_ANIMATION(BUTTON_PLAY_BIG_DESELECT),

    MAKE_ANIMATION(BUTTON_SETTINGS_BIG_SELECT),
    MAKE_ANIMATION(BUTTON_SETTINGS_BIG_DESELECT),

    MAKE_ANIMATION(BUTTON_CREDITS_BIG_SELECT),
    MAKE_ANIMATION(BUTTON_CREDITS_BIG_DESELECT),

    MAKE_ANIMATION(BUTTON_QUIT_BIG_SELECT),
    MAKE_ANIMATION(BUTTON_QUIT_BIG_DESELECT),

    MAKE_ANIMATION(BUTTON_PLAY_SELECT),
    MAKE_ANIMATION(BUTTON_PLAY_DESELECT),

    MAKE_ANIMATION(BUTTON_SETTINGS_SELECT),
    MAKE_ANIMATION(BUTTON_SETTINGS_DESELECT),

    MAKE_ANIMATION(BUTTON_CREDITS_SELECT),
    MAKE_ANIMATION(BUTTON_CREDITS_DESELECT),

    MAKE_ANIMATION(BUTTON_QUIT_SELECT),
    MAKE_ANIMATION(BUTTON_QUIT_DESELECT),
};

#undef MAKE_ANIMATION

inline
const Animation& animation_get( int animation ) {
    if( animation >= ANIM_COUNT ) {
        return __ANIMATIONS[0];
    }
    return __ANIMATIONS[animation];
}

#endif /* header guard */
