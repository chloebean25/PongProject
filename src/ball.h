#pragma once
#include "cpup/canis.h"
#include "cpup/scene.h"
#include "cpup/model.h"
#include "cpup/inputmanager.h"
#include "paddle.h"

#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct {
    int leftScore;
    int rightScore;
    int gameOver;
    float rainTimer;
    float trailTimer;
    Vector4 winColor;
    Vector3 trail[8];
} Ball;

Entity* SpawnBall(AppContext* _app, Entity* _entity);
void BallDraw(AppContext* _app, Entity* _entity);

void UpdateBallTrail(Ball* ballData, Entity* _entity)
{
    for (int i = 7; i > 0; i--)
    {
        ballData->trail[i] = ballData->trail[i - 1];
    }
    ballData->trail[0] = _entity->transform.position;
}

void ResetBallTrail(Ball* ballData, Entity* _entity)
{
    for (int i = 0; i < 8; i++)
    {
        ballData->trail[i] = _entity->transform.position;
    }
    ballData->trailTimer = 0.0f;
}

void RainBallUpdate(AppContext* _app, Entity* _entity)
{
    Vector3 delta = Vec2ToVec3(Vec2Mul(_entity->velocity, _app->deltaTime));
    _entity->transform.position = Vec3Add(_entity->transform.position, delta);

    if (_entity->transform.position.y < -50.0f)
    {
        Destroy(_app, &(_app->scene), _entity->id);
    }
}

Entity* SpawnRainBall(AppContext* _app, Entity* _entity, Vector4 color)
{
    Entity* ball = Spawn(&(_app->scene));
    ball->transform.position = InitVector3((float)(20 + rand() % (_app->windowWidth - 40)), _app->windowHeight + (float)(rand() % 120), 0.0f);
    ball->transform.scale = InitVector3(20.0f, 20.0f, 1.0f);
    ball->velocity = InitVector2(0.0f, -(150.0f + (rand() % 150)));

    ball->image = _entity->image;
    ball->model = _entity->model;
    ball->shaderId = _entity->shaderId;
    ball->color = color;

    ball->Update = RainBallUpdate;
    ball->Draw = BallDraw;

    return ball;
}

Vector4 GetPaddleColor(AppContext* _app, const char* paddleName)
{
    for (int i = 0; i < vec_count(&_app->scene->entities); i++)
    {
        Entity* other = &_app->scene->entities[i];

        if (other->name == NULL)
            continue;

        if (strcmp(other->name, paddleName) == 0)
            return other->color;
    }

    return InitVector4(1.0f, 1.0f, 1.0f, 1.0f);
}

void BallStart(AppContext* _app, Entity* _entity)
{
    _entity->color = InitVector4(1.0f, 1.0f, 1.0f, 1.0f);
    _entity->transform.scale = InitVector3(32.0f, 32.0f, 1.0f);

    Ball* ballData = (Ball*)_entity->data;

    if (ballData)
    {
        ballData->leftScore = 0;
        ballData->rightScore = 0;
        ballData->gameOver = 0;
        ballData->rainTimer = 0.0f;
        ballData->trailTimer = 0.0f;
        ballData->winColor = InitVector4(1.0f, 1.0f, 1.0f, 1.0f);

        ResetBallTrail(ballData, _entity);
    }
}

bool AABB(Entity* a, Entity* b)
{
    float aLeft   = a->transform.position.x - a->transform.scale.x * 0.5f;
    float aRight  = a->transform.position.x + a->transform.scale.x * 0.5f;
    float aTop    = a->transform.position.y + a->transform.scale.y * 0.5f;
    float aBottom = a->transform.position.y - a->transform.scale.y * 0.5f;

    float bLeft   = b->transform.position.x - b->transform.scale.x * 0.5f;
    float bRight  = b->transform.position.x + b->transform.scale.x * 0.5f;
    float bTop    = b->transform.position.y + b->transform.scale.y * 0.5f;
    float bBottom = b->transform.position.y - b->transform.scale.y * 0.5f;

    return (aLeft < bRight &&
            aRight > bLeft &&
            aTop > bBottom &&
            aBottom < bTop);
}

void BallUpdate(AppContext* _app, Entity* _entity)
{
    Ball* ballData = (Ball*)_entity->data;

    if (ballData->gameOver)
    {
        ballData->rainTimer += _app->deltaTime;

        if (ballData->rainTimer < 40.0f)
        {
            if (rand() % 2 == 0)
            {
                SpawnRainBall(_app, _entity, ballData->winColor);
            }
        }

        return;
    }

    if (GetKeyDown(_app, SDL_SCANCODE_P))
    {
        SpawnBall(_app, _entity);
    }

    if (Vec2EqualsZero(_entity->velocity) && GetKeyDown(_app, SDL_SCANCODE_SPACE))
    {
        i32 startingDirection = rand() % 4;

        static Vector2 directions[4] = {
            (Vector2){0.72f, 0.72f},
            (Vector2){0.72f, -0.72f},
            (Vector2){-0.72f, 0.72f},
            (Vector2){-0.72f, -0.72f},
        };

        _entity->velocity = Vec2Mul(directions[startingDirection], 150.0f);
    }

    if (_entity->transform.position.y - _entity->transform.scale.y * 0.5f <= 0.0f && _entity->velocity.y < 0.0f)
        _entity->velocity.y *= -1.0f;

    if (_entity->transform.position.y + _entity->transform.scale.y * 0.5f >= _app->windowHeight && _entity->velocity.y > 0.0f)
        _entity->velocity.y *= -1.0f;

    for (int i = 0; i < vec_count(&_app->scene->entities); i++)
    {
        Entity* other = &_app->scene->entities[i];

        if (other == _entity)
            continue;

        if (other->name == NULL)
            continue;

        if (strcmp(other->name, "leftPaddle") == 0 ||
            strcmp(other->name, "rightPaddle") == 0)
        {
            if (AABB(_entity, other))
            {
                _entity->color = other->color;

                Paddle* paddleData = (Paddle*)other->data;
                paddleData->bounceTimer = 0.12f;

                float speedX = fabsf(_entity->velocity.x);
                if (speedX < 150.0f)
                    speedX = 150.0f;

                if (strcmp(other->name, "leftPaddle") == 0)
                {
                    _entity->velocity.x = speedX;
                    _entity->transform.position.x =
                        other->transform.position.x + other->transform.scale.x * 0.5f + _entity->transform.scale.x * 0.5f + 4.0f;
                }
                else
                {
                    _entity->velocity.x = -speedX;
                    _entity->transform.position.x =
                        other->transform.position.x - other->transform.scale.x * 0.5f - _entity->transform.scale.x * 0.5f - 4.0f;
                }

                break;
            }
        }
    }

    if (_entity->transform.position.x + _entity->transform.scale.x * 0.5f < 0)
    {
        ballData->rightScore++;

        char title[128];
        sprintf(title, "Red: %d  Blue: %d", ballData->leftScore, ballData->rightScore);
        SDL_SetWindowTitle(_app->window, title);

        if (ballData->rightScore >= 5)
        {
            ballData->gameOver = 1;
            ballData->rainTimer = 0.0f;
            ballData->winColor = GetPaddleColor(_app, "rightPaddle");
            _entity->velocity = InitVector2(0, 0);
            return;
        }

        _entity->transform.position = InitVector3(_app->windowWidth * 0.5f, _app->windowHeight * 0.5f, 0.0f);
        _entity->velocity = InitVector2(0, 0);
        ResetBallTrail(ballData, _entity);
        return;
    }

    if (_entity->transform.position.x - _entity->transform.scale.x * 0.5f > _app->windowWidth)
    {
        ballData->leftScore++;

        char title[128];
        sprintf(title, "Red: %d  Blue: %d", ballData->leftScore, ballData->rightScore);
        SDL_SetWindowTitle(_app->window, title);

        if (ballData->leftScore >= 5)
        {
            ballData->gameOver = 1;
            ballData->rainTimer = 0.0f;
            ballData->winColor = GetPaddleColor(_app, "leftPaddle");
            _entity->velocity = InitVector2(0, 0);
            return;
        }

        _entity->transform.position = InitVector3(_app->windowWidth * 0.5f, _app->windowHeight * 0.5f, 0.0f);
        _entity->velocity = InitVector2(0, 0);
        ResetBallTrail(ballData, _entity);
        return;
    }

    Vector3 delta = Vec2ToVec3(Vec2Mul(_entity->velocity, _app->deltaTime));
    _entity->transform.position = Vec3Add(_entity->transform.position, delta);

    ballData->trailTimer += _app->deltaTime;
    if (ballData->trailTimer >= 0.10f)
    {
        ballData->trailTimer = 0.0f;
        UpdateBallTrail(ballData, _entity);
    }
}

void BallDraw(AppContext* _app, Entity* _entity)
{
    Ball* ballData = (Ball*)_entity->data;

    BindShader(_entity->shaderId);

    ShaderSetFloat(_entity->shaderId, "TIME", _app->time);
    ShaderSetMatrix4(_entity->shaderId, "VIEW", _app->view);
    ShaderSetMatrix4(_entity->shaderId, "PROJECTION", _app->projection);

    if (ballData != NULL && !Vec2EqualsZero(_entity->velocity))
    {
        for (int i = 7; i > 0; i--)
        {
            Matrix4 trailTransform = IdentityMatrix4();
            Mat4Translate(&trailTransform, ballData->trail[i]);
            Mat4Rotate(&trailTransform, _entity->transform.rotation * DEG2RAD, InitVector3(0.0f, 0.0f, 1.0f));
            Mat4Scale(&trailTransform, InitVector3(
                _entity->transform.scale.x * 0.65f,
                _entity->transform.scale.y * 0.65f,
                _entity->transform.scale.z));

            Vector4 fadeColor = _entity->color;
            fadeColor.w = 0.45f - (0.05f * i);

            if (fadeColor.w < 0.08f)
                fadeColor.w = 0.08f;

            ShaderSetVector4(_entity->shaderId, "COLOR", fadeColor);
            ShaderBindTexture(_entity->shaderId, _entity->image->id, "MAIN_TEXTURE", 0);
            ShaderSetMatrix4(_entity->shaderId, "TRANSFORM", trailTransform);
            DrawModel(*_entity->model);
        }
    }

    Matrix4 transform = IdentityMatrix4();
    Mat4Translate(&transform, _entity->transform.position);
    Mat4Rotate(&transform, _entity->transform.rotation * DEG2RAD, InitVector3(0.0f, 0.0f, 1.0f));
    Mat4Scale(&transform, InitVector3(_entity->transform.scale.x, _entity->transform.scale.y, _entity->transform.scale.z));

    Vector4 mainColor = _entity->color;
    mainColor.w = 1.0f;

    ShaderSetVector4(_entity->shaderId, "COLOR", mainColor);
    ShaderBindTexture(_entity->shaderId, _entity->image->id, "MAIN_TEXTURE", 0);
    ShaderSetMatrix4(_entity->shaderId, "TRANSFORM", transform);
    DrawModel(*_entity->model);

    UnBindShader();
}


void BallOnDestroy(AppContext* _app, Entity* _entity)
{
    if (_entity->data != NULL)
    {
        free(_entity->data);
        _entity->data = NULL;
    }
}

Entity* SpawnBall(AppContext* _app, Entity* _entity)
{
    Entity* ball = Spawn(&(_app->scene));
    ball->transform.position = InitVector3(_app->windowWidth * 0.5f, _app->windowHeight * 0.5f, 0.0f);
    ball->data = calloc(1, sizeof(Ball));
    ball->image = _entity->image;
    ball->model = _entity->model;
    ball->shaderId = _entity->shaderId;
    ball->Start = BallStart;
    ball->Update = BallUpdate;
    ball->Draw = BallDraw;
    ball->OnDestroy = BallOnDestroy;
    return ball;
}
