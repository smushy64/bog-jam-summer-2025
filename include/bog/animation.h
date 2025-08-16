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

String string_from_animation( int animation );
bool animation_from_string( String string, int* out );

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

    ANIM_BUTTON_GENERIC_EDGE_SELECT,
    ANIM_BUTTON_GENERIC_EDGE_DESELECT,

    ANIM_BUTTON_GENERIC_MIDDLE_SELECT,
    ANIM_BUTTON_GENERIC_MIDDLE_DESELECT,

    ANIM_JADE_BASE,
    ANIM_JADE_HAPPY,
    ANIM_JADE_SMILE,
    ANIM_JADE_NEUTRAL,
    ANIM_JADE_CREEPY_BASE,
    ANIM_JADE_CREEPY_NEUTRAL,
    ANIM_JADE_CREEPIEST_BASE,
    ANIM_JADE_CREEPIEST_NEUTRAL,
    ANIM_JADE_CREEPIEST_NEUTRAL_EXP,
    ANIM_JADE_CREEPY_NEUTRAL_EXP,
    ANIM_JADE_BLANK_NEUTRAL_EXP,
    ANIM_JADE_SMILE_EXP,
    ANIM_JADE_NEUTRAL_EXP,
    ANIM_JADE_HAPPY_EXP,
    ANIM_JADE_BASE_EXP,
    ANIM_JADE_SURPRISE,

    ANIM_4M,

    ANIM_ZUMA,

    ANIM_STEFAN,

    ANIM_BOOMBA,

    ANIM_CEB,

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
String string_from_animation( int animation ) {
    switch( (AnimationCap)animation ) {
        case ANIM_BUTTON_GENERIC_SELECT          : return "button_generic_select";
        case ANIM_BUTTON_GENERIC_DESELECT        : return "button_generic_deselect";
        case ANIM_BUTTON_PLAY_BIG_SELECT         : return "button_play_big_select";
        case ANIM_BUTTON_PLAY_BIG_DESELECT       : return "button_play_big_deselect";
        case ANIM_BUTTON_SETTINGS_BIG_SELECT     : return "button_settings_big_select";
        case ANIM_BUTTON_SETTINGS_BIG_DESELECT   : return "button_settings_big_deselect";
        case ANIM_BUTTON_CREDITS_BIG_SELECT      : return "button_credits_big_select";
        case ANIM_BUTTON_CREDITS_BIG_DESELECT    : return "button_credits_big_deselect";
        case ANIM_BUTTON_QUIT_BIG_SELECT         : return "button_quit_big_select";
        case ANIM_BUTTON_QUIT_BIG_DESELECT       : return "button_quit_big_deselect";
        case ANIM_BUTTON_PLAY_SELECT             : return "button_play_select";
        case ANIM_BUTTON_PLAY_DESELECT           : return "button_play_deselect";
        case ANIM_BUTTON_SETTINGS_SELECT         : return "button_settings_select";
        case ANIM_BUTTON_SETTINGS_DESELECT       : return "button_settings_deselect";
        case ANIM_BUTTON_CREDITS_SELECT          : return "button_credits_select";
        case ANIM_BUTTON_CREDITS_DESELECT        : return "button_credits_deselect";
        case ANIM_BUTTON_QUIT_SELECT             : return "button_quit_select";
        case ANIM_BUTTON_QUIT_DESELECT           : return "button_quit_deselect";
        case ANIM_BUTTON_GENERIC_EDGE_SELECT     : return "button_generic_edge_select";
        case ANIM_BUTTON_GENERIC_EDGE_DESELECT   : return "button_generic_edge_deselect";
        case ANIM_BUTTON_GENERIC_MIDDLE_SELECT   : return "button_generic_middle_select";
        case ANIM_BUTTON_GENERIC_MIDDLE_DESELECT : return "button_generic_middle_deselect";

        case ANIM_JADE_BASE                  : return "jade_base";
        case ANIM_JADE_HAPPY                 : return "jade_happy";
        case ANIM_JADE_SMILE                 : return "jade_smile";
        case ANIM_JADE_NEUTRAL               : return "jade_neutral";
        case ANIM_JADE_CREEPY_BASE           : return "jade_creepy_base";
        case ANIM_JADE_CREEPY_NEUTRAL        : return "jade_creepy_neutral";
        case ANIM_JADE_CREEPIEST_BASE        : return "jade_creepiest_base";
        case ANIM_JADE_CREEPIEST_NEUTRAL     : return "jade_creepiest_neutral";
        case ANIM_JADE_CREEPIEST_NEUTRAL_EXP : return "jade_creepiest_neutral_exp";
        case ANIM_JADE_CREEPY_NEUTRAL_EXP    : return "jade_creepy_neutral_exp";
        case ANIM_JADE_BLANK_NEUTRAL_EXP     : return "jade_blank_neutral_exp";
        case ANIM_JADE_SMILE_EXP             : return "jade_smile_exp";
        case ANIM_JADE_NEUTRAL_EXP           : return "jade_neutral_exp";
        case ANIM_JADE_HAPPY_EXP             : return "jade_happy_exp";
        case ANIM_JADE_BASE_EXP              : return "jade_base_exp";
        case ANIM_JADE_SURPRISE        : return "jade_surprise";

        case ANIM_4M     : return "4m";
        case ANIM_ZUMA   : return "zuma";
        case ANIM_STEFAN : return "stefan";
        case ANIM_BOOMBA : return "boomba";
        case ANIM_CEB    : return "ceb";

        case ANIM_NONE:
        case ANIM_COUNT:
            break;
    }
    return "";
}
inline
bool animation_from_string( String string, int* out ) {
    for( int i = ANIM_NONE + 1; i < ANIM_COUNT; ++i ) {
        if( string_cmp( string, string_from_animation( i ) ) ) {
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
        { 0, 329, 180, 25 },
    },
    {
        { 0 + 180, 329, 180, 25 },
    },
    {
        { 0 + 180 + 180, 329, 180, 25 },
    },
    {
        { 0 + 180 + 180 + 180, 329, 180, 25 },
    },
    {
        { 0 + 180 + 180 + 180 + 180, 329, 180, 25 },
    },
};

_readonly AnimationFrame __ANIM_BUTTON_PLAY_BIG_DESELECT[] = {
    {
        { 0 + 180 + 180 + 180 + 180, 329, 180, 25 },
    },
    {
        { 0 + 180 + 180 + 180 + 180 + 180, 329, 180, 25 },
    },
    {
        { 0 + 180 + 180 + 180 + 180 + 180 + 180, 329, 180, 25 },
    },
    {
        { 0 + 180 + 180 + 180 + 180 + 180 + 180 + 180, 329, 180, 25 },
    },
    {
        { 0, 329, 180, 25 },
    },
};

_readonly AnimationFrame __ANIM_BUTTON_SETTINGS_BIG_SELECT[] = {
    {
        { 0, 363, 180, 25 },
    },
    {
        { 0 + 180, 363, 180, 25 },
    },
    {
        { 0 + 180 + 180, 363, 180, 25 },
    },
    {
        { 0 + 180 + 180 + 180, 363, 180, 25 },
    },
    {
        { 0 + 180 + 180 + 180 + 180, 363, 180, 25 },
    },
};

_readonly AnimationFrame __ANIM_BUTTON_SETTINGS_BIG_DESELECT[] = {
    {
        { 0 + 180 + 180 + 180 + 180, 363, 180, 25 },
    },
    {
        { 0 + 180 + 180 + 180 + 180 + 180, 363, 180, 25 },
    },
    {
        { 0 + 180 + 180 + 180 + 180 + 180 + 180, 363, 180, 25 },
    },
    {
        { 0 + 180 + 180 + 180 + 180 + 180 + 180 + 180, 363, 180, 25 },
    },
    {
        { 0, 363, 180, 25 },
    },
};

_readonly AnimationFrame __ANIM_BUTTON_CREDITS_BIG_SELECT[] = {
    {
        { 0, 397, 180, 25 },
    },
    {
        { 0 + 180, 397, 180, 25 },
    },
    {
        { 0 + 180 + 180, 397, 180, 25 },
    },
    {
        { 0 + 180 + 180 + 180, 397, 180, 25 },
    },
    {
        { 0 + 180 + 180 + 180 + 180, 397, 180, 25 },
    },
};

_readonly AnimationFrame __ANIM_BUTTON_CREDITS_BIG_DESELECT[] = {
    {
        { 0 + 180 + 180 + 180 + 180, 397, 180, 25 },
    },
    {
        { 0 + 180 + 180 + 180 + 180 + 180, 397, 180, 25 },
    },
    {
        { 0 + 180 + 180 + 180 + 180 + 180 + 180, 397, 180, 25 },
    },
    {
        { 0 + 180 + 180 + 180 + 180 + 180 + 180 + 180, 397, 180, 25 },
    },
    {
        { 0, 397, 180, 25 },
    },
};

_readonly AnimationFrame __ANIM_BUTTON_QUIT_BIG_SELECT[] = {
    {
        { 0, 431, 180, 25 },
    },
    {
        { 0 + 180, 431, 180, 25 },
    },
    {
        { 0 + 180 + 180, 431, 180, 25 },
    },
    {
        { 0 + 180 + 180 + 180, 431, 180, 25 },
    },
    {
        { 0 + 180 + 180 + 180 + 180, 431, 180, 25 },
    },
};

_readonly AnimationFrame __ANIM_BUTTON_QUIT_BIG_DESELECT[] = {
    {
        { 0 + 180 + 180 + 180 + 180, 431, 180, 25 },
    },
    {
        { 0 + 180 + 180 + 180 + 180 + 180, 431, 180, 25 },
    },
    {
        { 0 + 180 + 180 + 180 + 180 + 180 + 180, 431, 180, 25 },
    },
    {
        { 0 + 180 + 180 + 180 + 180 + 180 + 180 + 180, 431, 180, 25 },
    },
    {
        { 0, 431, 180, 25 },
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

_readonly AnimationFrame __ANIM_BUTTON_GENERIC_EDGE_SELECT[] = {
    {
        { 1304, 0, 29, 16 }, /* src rect */
    },
    {
        { 1304, 16, 29, 16 }, /* src rect */
    },
    {
        { 1304, 16 + 16, 29, 16 }, /* src rect */
    },
    {
        { 1304, 16 + 16 + 16, 29, 16 }, /* src rect */
    },
    {
        { 1304, 16 + 16 + 16 + 16, 29, 16 }, /* src rect */
    },
};

_readonly AnimationFrame __ANIM_BUTTON_GENERIC_EDGE_DESELECT[] = {
    {
        { 1304, 16 + 16 + 16 + 16, 29, 16 }, /* src rect */
    },
    {
        { 1304, 16 + 16 + 16, 29, 16 }, /* src rect */
    },
    {
        { 1304, 16 + 16, 29, 16 }, /* src rect */
    },
    {
        { 1304, 16, 29, 16 }, /* src rect */
    },
    {
        { 1304, 0, 29, 16 }, /* src rect */
    },
};

_readonly AnimationFrame __ANIM_BUTTON_GENERIC_MIDDLE_SELECT[] = {
    {
        { 1333, 0, 16, 16 }, /* src rect */
    },
    {
        { 1333, 16, 16, 16 }, /* src rect */
    },
    {
        { 1333, 16 + 16, 16, 16 }, /* src rect */
    },
    {
        { 1333, 16 + 16 + 16, 16, 16 }, /* src rect */
    },
    {
        { 1333, 16 + 16 + 16 + 16, 16, 16 }, /* src rect */
    },
};

_readonly AnimationFrame __ANIM_BUTTON_GENERIC_MIDDLE_DESELECT[] = {
    {
        { 1333, 16 + 16 + 16 + 16, 16, 16 }, /* src rect */
    },
    {
        { 1333, 16 + 16 + 16, 16, 16 }, /* src rect */
    },
    {
        { 1333, 16 + 16, 16, 16 }, /* src rect */
    },
    {
        { 1333, 16, 16, 16 }, /* src rect */
    },
    {
        { 1333, 0, 16, 16 }, /* src rect */
    },
};

_readonly AnimationFrame __ANIM_JADE_BASE[] = {
    {
        { 0 * 134, 0 * 195, 134, 195 },
        TEX_JADE
    }
};

_readonly AnimationFrame __ANIM_JADE_HAPPY[] = {
    {
        { 1 * 134, 0 * 195, 134, 195 },
        TEX_JADE
    }
};

_readonly AnimationFrame __ANIM_JADE_SMILE[] = {
    {
        { 2 * 134, 0 * 195, 134, 195 },
        TEX_JADE
    }
};

_readonly AnimationFrame __ANIM_JADE_NEUTRAL[] = {
    {
        { 3 * 134, 0 * 195, 134, 195 },
        TEX_JADE
    }
};

_readonly AnimationFrame __ANIM_JADE_CREEPY_BASE[] = {
    {
        { 0 * 134, 1 * 195, 134, 195 },
        TEX_JADE
    }
};

_readonly AnimationFrame __ANIM_JADE_CREEPY_NEUTRAL[] = {
    {
        { 1 * 134, 1 * 195, 134, 195 },
        TEX_JADE
    }
};

_readonly AnimationFrame __ANIM_JADE_CREEPIEST_BASE[] = {
    {
        { 2 * 134, 1 * 195, 134, 195 },
        TEX_JADE
    }
};

_readonly AnimationFrame __ANIM_JADE_CREEPIEST_NEUTRAL[] = {
    {
        { 3 * 134, 1 * 195, 134, 195 },
        TEX_JADE
    }
};

_readonly AnimationFrame __ANIM_JADE_CREEPIEST_NEUTRAL_EXP[] = {
    {
        { 0 * 134, 2 * 195, 134, 195 },
        TEX_JADE
    }
};

_readonly AnimationFrame __ANIM_JADE_CREEPY_NEUTRAL_EXP[] = {
    {
        { 1 * 134, 2 * 195, 134, 195 },
        TEX_JADE
    }
};

_readonly AnimationFrame __ANIM_JADE_BLANK_NEUTRAL_EXP[] = {
    {
        { 2 * 134, 2 * 195, 134, 195 },
        TEX_JADE
    }
};

_readonly AnimationFrame __ANIM_JADE_SMILE_EXP[] = {
    {
        { 3 * 134, 2 * 195, 134, 195 },
        TEX_JADE
    }
};

_readonly AnimationFrame __ANIM_JADE_NEUTRAL_EXP[] = {
    {
        { 0 * 134, 3 * 195, 134, 195 },
        TEX_JADE
    }
};

_readonly AnimationFrame __ANIM_JADE_HAPPY_EXP[] = {
    {
        { 1 * 134, 3 * 195, 134, 195 },
        TEX_JADE
    }
};

_readonly AnimationFrame __ANIM_JADE_BASE_EXP[] = {
    {
        { 2 * 134, 3 * 195, 134, 195 },
        TEX_JADE
    }
};

_readonly AnimationFrame __ANIM_JADE_SURPRISE[] = {
    {
        { 3 * 134, 3 * 195, 134, 195 },
        TEX_JADE
    }
};

_readonly AnimationFrame __ANIM_4M[] = {
    {
        { 0, 0, 134, 195 },
        TEX_4M
    }
};

_readonly AnimationFrame __ANIM_ZUMA[] = {
    {
        { 0, 0, 237, 195 },
        TEX_ZUMA
    }
};

_readonly AnimationFrame __ANIM_STEFAN[] = {
    {
        { 0, 0, 240, 195 },
        TEX_STEFAN
    }
};

_readonly AnimationFrame __ANIM_BOOMBA[] = {
    {
        { 0, 0, 205, 195 },
        TEX_BOOMBA
    }
};

_readonly AnimationFrame __ANIM_CEB[] = {
    {
        { 0, 0, 134, 195 },
        TEX_CEB
    }
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

    MAKE_ANIMATION(BUTTON_GENERIC_EDGE_SELECT),
    MAKE_ANIMATION(BUTTON_GENERIC_EDGE_DESELECT),

    MAKE_ANIMATION(BUTTON_GENERIC_MIDDLE_SELECT),
    MAKE_ANIMATION(BUTTON_GENERIC_MIDDLE_DESELECT),

    MAKE_ANIMATION(JADE_BASE),
    MAKE_ANIMATION(JADE_HAPPY),
    MAKE_ANIMATION(JADE_SMILE),
    MAKE_ANIMATION(JADE_NEUTRAL),
    MAKE_ANIMATION(JADE_CREEPY_BASE),
    MAKE_ANIMATION(JADE_CREEPY_NEUTRAL),
    MAKE_ANIMATION(JADE_CREEPIEST_BASE),
    MAKE_ANIMATION(JADE_CREEPIEST_NEUTRAL),
    MAKE_ANIMATION(JADE_CREEPIEST_NEUTRAL_EXP),
    MAKE_ANIMATION(JADE_CREEPY_NEUTRAL_EXP),
    MAKE_ANIMATION(JADE_BLANK_NEUTRAL_EXP),
    MAKE_ANIMATION(JADE_SMILE_EXP),
    MAKE_ANIMATION(JADE_NEUTRAL_EXP),
    MAKE_ANIMATION(JADE_HAPPY_EXP),
    MAKE_ANIMATION(JADE_BASE_EXP),
    MAKE_ANIMATION(JADE_SURPRISE),

    MAKE_ANIMATION(4M),
    MAKE_ANIMATION(ZUMA),
    MAKE_ANIMATION(STEFAN),
    MAKE_ANIMATION(BOOMBA),
    MAKE_ANIMATION(CEB),

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
