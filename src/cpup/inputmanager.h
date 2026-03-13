#pragma once
#include "canis.h"

void InputManagerNewFrame(AppContext* _app) {

    memcpy(_app->previousKeys, _app->currentKeys, SDL_SCANCODE_COUNT * sizeof(bool));
    const bool* cur = SDL_GetKeyboardState(NULL);
    memcpy(_app->currentKeys, cur, SDL_SCANCODE_COUNT * sizeof(bool));
}

bool GetKey(AppContext* _app, SDL_Scancode _scancode) {
    return (_app->currentKeys[_scancode]);
}

bool GetKeyUp(AppContext* _app, SDL_Scancode _scancode) {
    return (_app->currentKeys[_scancode] == false && _app->previousKeys[_scancode]);
}

bool GetKeyDown(AppContext* _app, SDL_Scancode _scancode) {
    return (_app->currentKeys[_scancode] && _app->previousKeys[_scancode] == false);
}