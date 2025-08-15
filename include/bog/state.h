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
#include "bog/scene.h"
#include "bog/variable.h"
#include "bog/constants.h"
#include "bog/animation.h"

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

enum class GameStepType {
    NONE,
    CHANGE_SCENE,
    PLAY_SCENE,
};

struct GameState {
    float elapsed;

    Texture   textures[TEX_COUNT];
    Scene     scene;
    StorageKV kv;

    int scene_id = -1, node_id = -1;

    String character_name;
    String text;

    Rectangle text_box;

    DisplayTextState display_text;
    float scene_change_timer;

    union {
        struct {
            AnimationTimeline anim_button_play;
            AnimationTimeline anim_button_settings;
            AnimationTimeline anim_button_credits;
            AnimationTimeline anim_button_quit;
        };
        AnimationTimeline anim[4];
    };
};

struct Common {
    bool is_first_frame;
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
