#if !defined(BOG_CONSTANTS_H)
#define BOG_CONSTANTS_H
/**
 * @file   constants.h
 * @brief  Constants.
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   August 14, 2025
*/
#include "bog/prelude.h"

_readonly Rectangle COORD_TEXT_BOX_TRIM_TOP = { 0, 220, 352, 16 };
_readonly Rectangle COORD_TEXT_BOX_TRIM_BOT = { 0, 279, 352, 6 };

_readonly Rectangle COORD_DECORATION = { 2, 169, 28, 28 };

_readonly Rectangle COORD_CONTINUE_BUTTON = { 125, 32, 11, 11 };

_readonly Rectangle COORD_PAUSE_BUTTON = { 125, 2, 21, 21 };

_readonly Rectangle COORD_PAUSE_LOGO = { 0, 0, 93, 113 };
_readonly Rectangle COORD_PAUSE_ACT  = { 0, 123, 36, 8 };

_readonly Rectangle COORD_LOGO = { 611, 168, 94, 38 };

_readonly Color COLOR_TEXT_BOX_BACKGROUND = { 40, 4, 16, 176 };
_readonly Color COLOR_CHARACTER_DIM = { 170, 170, 170, 255 };


_readonly Vector4 TEXT_BOX_PADDING = { 10.0f, 40.0f, 10.0f, 40.0f };

_readonly float FONT_SIZE = 28.0f;

#if defined(IS_DEBUG)
_readonly int START_NODE = 0;
#else
_readonly int START_NODE = 0;
#endif

#if 0
_readonly float SCENE_TRANSITION_TIME = 0.0f;
#else
_readonly float SCENE_TRANSITION_TIME = 2.0f;
#endif

_readonly float FADE_TIME = 2.0f;

_readonly float TEXT_SPEED      = 8.0f;
_readonly float TEXT_SPEED_FAST = 0.001f;

enum {
    TEX_MENU,
    TEX_JADE,
    TEX_BACKGROUND_ROOM,
    TEX_4M,
    TEX_BOOMBA,
    TEX_CEB,
    TEX_STEFAN,
    TEX_ZUMA,

    TEX_COUNT
};

struct TextureLoadParams {
    const char* path;
    int         filter = TEXTURE_FILTER_POINT;
};

static TextureLoadParams TEXTURE_LOAD_PARAMS[] = {
    { "resources/textures/menu_spritesheet.png" },       /* TEX_MENU */
    { "resources/textures/jade_spritesheet.png" },       /* TEX_JADE */
    { "resources/textures/backgrounds_spritesheet.png" }, /* TEX_BACKGROUND_ROOM */
    { "resources/textures/4m_sprite.png" },
    { "resources/textures/boomba_sprite.png" },
    { "resources/textures/ceb_sprite.png" },
    { "resources/textures/stefan_sprite.png" },
    { "resources/textures/zuma_sprite.png" },
};

#endif /* header guard */
