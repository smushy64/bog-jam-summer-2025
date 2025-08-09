#if !defined(BOG_STATE_H)
#define BOG_STATE_H
/**
 * @file   state.h
 * @brief  Game State.
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   August 08, 2025
*/
#include "bog/prelude.h"
#include "bog/ui.h"

enum class StateType {
    INVALID,
    INTRO,
    MAIN_MENU,
    GAME,

    DEFAULT = GAME
};

struct IntroState {

};

struct MainMenuState {

};

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

struct GameState {
    DisplayTextState display_text;
    Texture textures[TEX_COUNT];
};

struct Common {
    Font font;
};

struct State {
    StateType type;
    union {
        IntroState    intro;
        MainMenuState menu;
        GameState     game;
    };
    Common common;
};

struct Memory {
    union {
        State state;
        u8 raw[sizeof(State)];
    };
};

void state_set( State* state, StateType type );

void state_update( State* state );

void _intro_load( State* state );
void _intro_unload( State* state );
void _intro_update( State* state );

void _menu_load( State* state );
void _menu_unload( State* state );
void _menu_update( State* state );

void _game_load( State* state );
void _game_unload( State* state );
void _game_update( State* state );

#endif /* header guard */
