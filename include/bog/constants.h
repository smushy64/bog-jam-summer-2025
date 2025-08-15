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

_readonly Color COLOR_TEXT_BOX_BACKGROUND = { 40, 4, 16, 176 };

_readonly Vector4 TEXT_BOX_PADDING = { 10.0f, 40.0f, 10.0f, 40.0f };

_readonly float FONT_SIZE = 28.0f;

_readonly float SCENE_TRANSITION_TIME = 2.0f;

enum {
    TEX_MENU,

    TEX_COUNT
};

struct TextureLoadParams {
    const char* path;
    int         filter = TEXTURE_FILTER_POINT;
};

static TextureLoadParams TEXTURE_LOAD_PARAMS[] = {
    { "resources/textures/menu_spritesheet.png" }, /* TEX_MENU */
};

#endif /* header guard */
