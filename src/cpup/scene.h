#pragma once

#include "entity.h"

typedef struct Scene {
    Entity* entities;
    Entity* startEntities;
    int nextId;
} Scene;

typedef struct AppContext AppContext;

Scene*  SceneInit();
void    SceneStart(AppContext* _app, Scene** _scene);
void    SceneUpdate(AppContext* _app, Scene** _scene);
void    SceneDraw(AppContext* _app, Scene** _scene);
Scene*  SceneFree(Scene** _scene);

Entity* Spawn(Scene** _scene);
void    Destroy(AppContext* _app, Scene** _scene, int _id);

Entity* Find(Scene** _scene, const char* _name);
Entity* GetEntity(Scene** _scene, int _id);
