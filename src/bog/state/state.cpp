/**
 * @file   state.cpp
 * @brief  State.
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   August 08, 2025
*/
#include "bog/state.h"
#include "bog/ui.h"

void state_set( State* state, StateType old_type ) {
    switch( old_type ) {
        case StateType::INVALID  : {
            state->common.font = LoadFontEx(
                "resources/fonts/martian-mono/MartianMono-Regular.ttf",
                FONT_SIZE, 0, 0 );
            state->common.settings.sfx    =
            state->common.settings.music  = 0.5f;

#if defined(IS_DEBUG)
            state->common.settings.volume = 0.0f;
#else
            state->common.settings.volume = 0.3f;
#endif
        } break;
        case StateType::INTRO    : _intro_unload( state ); break;
        case StateType::MAIN_MENU: _menu_unload( state ); break;
        case StateType::GAME     : _game_unload( state ); break;
    }

    state->common.is_first_frame = true;

    switch( state->type ) {
        case StateType::INVALID  : break;
        case StateType::INTRO    :
            state->intro = {};
            _intro_load( state );
            break;
        case StateType::MAIN_MENU:
            state->menu = {};
            _menu_load( state );
            break;
        case StateType::GAME     :
            state->game = {};
            _game_load( state );
            break;
    }
}

void state_update( State* state ) {
    switch( state->type ) {
        case StateType::INVALID  : break;
        case StateType::INTRO    : _intro_update( state ); break;
        case StateType::MAIN_MENU: _menu_update( state ); break;
        case StateType::GAME     : _game_update( state ); break;
    }

    state->common.is_first_frame = false;
}


