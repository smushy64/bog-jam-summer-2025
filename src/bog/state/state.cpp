/**
 * @file   state.cpp
 * @brief  State.
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   August 08, 2025
*/
#include "bog/state.h"

void state_set( State* state, StateType type ) {
    switch( state->type ) {
        case StateType::INVALID  : {
            state->common.font = LoadFontEx(
                "resources/fonts/martian-mono/MartianMono-Regular.ttf", 24.0f, 0, 0 );
        } break;
        case StateType::INTRO    : _intro_unload( state ); break;
        case StateType::MAIN_MENU: _menu_unload( state ); break;
        case StateType::GAME     : _game_unload( state ); break;
    }

    state->type = type;

    switch( type ) {
        case StateType::INVALID  : break;
        case StateType::INTRO    : _intro_load( state ); break;
        case StateType::MAIN_MENU: _menu_load( state ); break;
        case StateType::GAME     : _game_load( state ); break;
    }
}

void state_update( State* state ) {
    switch( state->type ) {
        case StateType::INVALID  : break;
        case StateType::INTRO    : _intro_update( state ); break;
        case StateType::MAIN_MENU: _menu_update( state ); break;
        case StateType::GAME     : _game_update( state ); break;
    }
}


