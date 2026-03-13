#pragma once

#include "math.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_scancode.h>

typedef struct Scene Scene;

typedef struct AppContext {
    void* window;
    void* glContext;
    Scene* scene;
    i32 windowWidth;
    i32 windowHeight;
    Matrix4 projection;
    Matrix4 view;
    f32 time;
    f32 deltaTime;
    bool previousKeys[SDL_SCANCODE_COUNT];
    bool currentKeys[SDL_SCANCODE_COUNT];
} AppContext;

extern i32 InitCanis();
