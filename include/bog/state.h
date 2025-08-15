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

    DEFAULT = MAIN_MENU
};

struct IntroState {

};

struct MainMenuState {
    Texture texture;
    bool is_settings_open;

    union {
        struct {
            AnimationTimeline button_play;
            AnimationTimeline button_settings;
            AnimationTimeline button_credits;
            AnimationTimeline button_quit;
        };
        AnimationTimeline buttons[4];
    };
};

enum class GameStepType {
    NONE,
    CHANGE_SCENE,
    PLAY_SCENE,
};

struct Button {
    AnimationTimeline animation;
    StringOffset      text;
};

struct ButtonList {
    List<Button> buttons;
    List<char>   string;

    void reset() {
        buttons.reset();
        string.reset();
    }
    void free() {
        buttons.free();
        string.free();
    }

    void push( String text );
    int update_and_draw(
        Font font, Texture* textures, Vector2 screen, Vector2 mouse, bool left_pressed, float dt );
};

struct Settings {
    float volume;
    float sfx;
    float music;
};

struct Character {
    bool is_enabled;
    AnimationTimeline anim;
    Color tint;
};

enum {
    MUS_MAIN,
    MUS_MAIN_LOOP,
    MUS_DARK,

    MUS_COUNT
};

_readonly const char* __MUSIC_PATHS[] = {
    "resources/audio/music/mus-main.mp3",
    "resources/audio/music/mus-main-loop.mp3",
    "resources/audio/music/mus-dark-loop.mp3"
};

struct GameState {
    float elapsed;
    float fade_timer;

    Texture   textures[TEX_COUNT];
    Scene     scene;
    StorageKV kv;

    int scene_id = -1, node_id = -1;

    String character_name;
    String text;

    int current_character = -1;
    union {
        struct {
            Character char_left;
            Character char_center;
            Character char_right;
        };
        Character characters[3];
    };

    Rectangle text_box;

    DisplayTextState display_text;
    float scene_change_timer;

    ButtonList buttons;

    bool is_paused;
    bool is_settings_open;

    union {
        struct {
            AnimationTimeline anim_button_play;
            AnimationTimeline anim_button_settings;
            AnimationTimeline anim_button_credits;
            AnimationTimeline anim_button_quit;
        };
        AnimationTimeline anim[4];
    };

    int   current_music = -1;
    Music music[MUS_COUNT];

    float last_music_volume;
};

struct Common {
    bool is_first_frame;
    Font font;
    Settings settings;
};

struct State {
    bool should_quit;
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
