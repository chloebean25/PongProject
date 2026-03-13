#pragma once
#include "cpup/canis.h"
#include "cpup/scene.h"
#include "cpup/model.h"
#include "cpup/inputmanager.h"
#include <SDL3/SDL.h>

typedef struct {
    int var;
    float bounceTimer;
} Paddle;

void PaddleStart(AppContext* _app, Entity* _entity) {
    //_entity->color = InitVector4(1.0f, 0.0f, 0.0f, 1.0f);
    _entity->transform.rotation = 0.0f;
    _entity->transform.scale = InitVector3(32.0f, 128.0f, 1.0f);

    Paddle* paddleData = (Paddle*)_entity->data;
    paddleData->bounceTimer = 0.0f;
}

void PaddleUpdate(AppContext* _app, Entity* _entity) {
    Paddle* paddleData = (Paddle*)_entity->data;

    float speed = 300.0f;
    //left paddle controls
    if (strcmp(_entity->name, "leftPaddle") == 0) {

        if (GetKey(_app, SDL_SCANCODE_W))
            _entity->transform.position.y += speed * _app->deltaTime;

        if (GetKey(_app, SDL_SCANCODE_S))
            _entity->transform.position.y -= speed * _app->deltaTime;
    }
    //right paddle controls
     if (strcmp(_entity->name, "rightPaddle") == 0) {

        if (GetKey(_app, SDL_SCANCODE_UP))
            _entity->transform.position.y += speed * _app->deltaTime;

        if (GetKey(_app, SDL_SCANCODE_DOWN))
            _entity->transform.position.y -= speed * _app->deltaTime;
    }

    if (paddleData->bounceTimer > 0.0f)
    {
        paddleData->bounceTimer -= _app->deltaTime;
        _entity->transform.scale = InitVector3(42.0f, 110.0f, 1.0f);
    }
    else
    {
        _entity->transform.scale = InitVector3(32.0f, 128.0f, 1.0f);
    }

    float halfHeight = _entity->transform.scale.y * 0.5f;

    if (_entity->transform.position.y - halfHeight < 0.0f)
        _entity->transform.position.y = halfHeight;

    if (_entity->transform.position.y + halfHeight > _app->windowHeight)
        _entity->transform.position.y = _app->windowHeight - halfHeight;
}

void PaddleDraw(AppContext* _app, Entity* _entity) {
    Matrix4 transform = IdentityMatrix4(); // the order is important
    Mat4Translate(&transform, _entity->transform.position);
    Mat4Rotate(&transform, _entity->transform.rotation * DEG2RAD, InitVector3(0.0f, 0.0f, 1.0f));
    Mat4Scale(&transform, InitVector3(_entity->transform.scale.x, _entity->transform.scale.y, _entity->transform.scale.z));

    BindShader(_entity->shaderId);

    ShaderSetFloat(_entity->shaderId, "TIME", _app->time);
    ShaderSetMatrix4(_entity->shaderId, "VIEW", _app->view);
    ShaderSetMatrix4(_entity->shaderId, "PROJECTION", _app->projection);

    ShaderSetVector4(_entity->shaderId, "COLOR", _entity->color);
    ShaderBindTexture(_entity->shaderId, _entity->image->id, "MAIN_TEXTURE", 0);
    ShaderSetMatrix4(_entity->shaderId, "TRANSFORM", transform);
    DrawModel(*_entity->model);

    UnBindShader();
}

void PaddleOnDestroy(AppContext* _app, Entity* _entity) {

}