#ifdef _WIN32
#include <windows.h>
#endif

#include <math.h>
#include <time.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "cpup/io.h"
#include "cpup/vec.h"
#include "cpup/math.h"
#include "cpup/types.h"
#include "cpup/model.h"
#include "cpup/shader.h"
#include "cpup/window.h"
#include "cpup/inputmanager.h"
#include "cpup/opengl.h"

#include "cpup/scene.h"

#include "ball.h"
#include "paddle.h"

AppContext app;

void BackgroundDraw(AppContext* _app, Entity* _entity)
{
    Matrix4 transform = IdentityMatrix4();
    Mat4Translate(&transform, _entity->transform.position);
    Mat4Scale(&transform, _entity->transform.scale);

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

int main(int argc, char *argv[])
{
    srand(time(NULL));

    if (InitCanis() > 0)
        return 1;

    app.windowWidth = 600;
    app.windowHeight = 600;
    
    if (InitWindow(&app) > 0)
        return 1;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Scene* scene = SceneInit();
    app.scene = scene;
    
    Image iconImage = IOLoadImage("assets/textures/canis_engine_icon.tga");
    Image containerImage = IOLoadImage("assets/textures/container.tga");
    Image circleImage = IOLoadImage("assets/textures/circle.tga");
    Image squareImage = IOLoadImage("assets/textures/square.tga");
    Image gridImage = IOLoadImage("assets/textures/grid.tga");
    
    // build and compile our shader programs
    u32 shaderProgram = GenerateShaderFromFiles("assets/shaders/logo.vs", "assets/shaders/logo.fs");
    u32 gridShaderProgram = GenerateShaderFromFiles("assets/shaders/grid.vs", "assets/shaders/grid.fs");

    printf("shaderID: %i\n", shaderProgram);
    printf("gridShaderID: %i\n", gridShaderProgram);

    float ve[] = {
        // positions            // texture coords
         0.5f,  0.5f, 0.0f,     1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,     1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,     0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,     0.0f, 1.0f  // top left 
    };
    unsigned int in[] = {
        0, 1, 3,
        1, 2, 3
    };

    f32* vertices = vec_init(32, sizeof(f32));
    vec_append(&vertices, ve, 32);

    u32* indices = vec_init(6, sizeof(u32));
    vec_append(&indices, in, 6);
    
    Model model = BuildModel(&vertices, &indices, STATIC_DRAW);

    Entity background = {0};
    background.transform.position = InitVector3(app.windowWidth * 0.5f, app.windowHeight * 0.5f, -1.0f);
    background.transform.scale = InitVector3((float)app.windowWidth, (float)app.windowHeight, 1.0f);
    background.color = InitVector4(1.0f, 1.0f, 1.0f, 1.0f);
    background.image = &gridImage;
    background.model = &model;
    background.shaderId = gridShaderProgram;
    background.Draw = BackgroundDraw;

    Entity* ball = Spawn(&scene);
    ball->transform.position = InitVector3(app.windowWidth * 0.5f, app.windowHeight * 0.5f, 0.0f);
    ball->data = calloc(1, sizeof(Ball));
    ball->image = &circleImage;
    ball->model = &model;
    ball->shaderId = shaderProgram;
    ball->Start = BallStart;
    ball->Update = BallUpdate;
    ball->Draw = BallDraw;
    ball->OnDestroy = BallOnDestroy;

    Entity* leftPaddle = Spawn(&scene);
    leftPaddle->name = "leftPaddle";
    leftPaddle->transform.position = InitVector3(16.0f, app.windowHeight * 0.5f, 0.0f);
    leftPaddle->color = InitVector4(1.0f, 0.0f, 0.0f, 1.0f);
    leftPaddle->data = calloc(1, sizeof(Paddle));
    leftPaddle->image = &squareImage;
    leftPaddle->model = &model;
    leftPaddle->shaderId = shaderProgram;
    leftPaddle->Start = PaddleStart;
    leftPaddle->Update = PaddleUpdate;
    leftPaddle->Draw = PaddleDraw;
    leftPaddle->OnDestroy = PaddleOnDestroy;

    Entity* rightPaddle = Spawn(&scene);
    rightPaddle->name = "rightPaddle";
    rightPaddle->transform.position = InitVector3(app.windowWidth - 16.0f, app.windowHeight * 0.5f, 0.0f);
    rightPaddle->color = InitVector4(0.0f, 0.0f, 1.0f, 1.0f);
    rightPaddle->data = calloc(1, sizeof(Paddle));
    rightPaddle->image = &squareImage;
    rightPaddle->model = &model;
    rightPaddle->shaderId = shaderProgram;
    rightPaddle->Start = PaddleStart;
    rightPaddle->Update = PaddleUpdate;
    rightPaddle->Draw = PaddleDraw;
    rightPaddle->OnDestroy = PaddleOnDestroy;
    
    bool running = true;
    f32 time = 0.0f;
    while(running) {
        // imput
        InputManagerNewFrame(&app);
        //printf("FPS: %f Entity Count: %i\n", 1.0f/app.deltaTime, vec_count(&scene->entities));

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
                running = false;
        }

        // render
        ClearWindow();

        if (app.time != 0.0f)
            app.deltaTime = (SDL_GetTicksNS() * 1e-9) -  app.time;
        
        app.time = SDL_GetTicksNS() * 1e-9;

        SceneStart(&app, &scene);

        app.projection = Mat4Orthographic(0.0f, (float)app.windowWidth, 0.0f, (float)app.windowHeight, 0.001f, 100.0f); 
        app.view = IdentityMatrix4(); 
        Mat4Translate(&app.view, InitVector3(0.0f, 0.0f, -0.5f));

        SceneUpdate(&app, &scene);

        BackgroundDraw(&app, &background);
        SceneDraw(&app, &scene);

        SwapWindow(&app);
    }

    FreeModel(model);

    free(iconImage.data);
    free(containerImage.data);
    free(circleImage.data);
    free(squareImage.data);
    free(gridImage.data);

    SceneFree(&scene);

    FreeWindow(&app);

    DeleteShader(shaderProgram);
    DeleteShader(gridShaderProgram);
    return 0;
}