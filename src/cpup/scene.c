#include "scene.h"
#include "canis.h"

#include <string.h>

#include "vec.h"
#include "shader.h"

Scene* SceneInit() {
    Scene* scene = calloc(1, sizeof(Scene));
    scene->entities = vec_init(200, sizeof(Entity));
    scene->startEntities = vec_init(200, sizeof(Entity));
    return scene;
}

void SceneStart(AppContext* _app, Scene** _scene) {
    int count = vec_count(&(*_scene)->startEntities);
    for (int i = 0; i < count; i++) {
        if ((*_scene)->startEntities[i].Start != NULL) {
            (*_scene)->startEntities[i].Start(_app, &(*_scene)->startEntities[i]);
        }

        vec_add(&(*_scene)->entities, &(*_scene)->startEntities[i]);
    }

    vec_clear(&(*_scene)->startEntities);
}

void SceneUpdate(AppContext* _app, Scene** _scene) {
    int count = vec_count(&(*_scene)->entities);
    for (int i = 0; i < count; i++) {
        if ((*_scene)->entities[i].Update != NULL) {
            (*_scene)->entities[i].Update(_app, &(*_scene)->entities[i]);
        }
    }
}

void SceneDraw(AppContext* _app, Scene** _scene) {
    int count = vec_count(&(*_scene)->entities);
    for (int i = 0; i < count; i++) {
        if ((*_scene)->entities[i].Draw != NULL) {
            (*_scene)->entities[i].Draw(_app, &(*_scene)->entities[i]);
        }
    }
}

Scene* SceneFree(Scene** _scene) {
    vec_free(&(*_scene)->entities);
    free(*_scene);
    *_scene = NULL;
}

Entity* Spawn(Scene** _scene) {
    Entity entity;
    vec_add(&(*_scene)->startEntities, &entity);
    int index = vec_count(&(*_scene)->startEntities) - 1;

    Entity* e = &(*_scene)->startEntities[index];
    e->id = (*_scene)->nextId++;
    e->name = NULL;
    e->transform.position = InitVector3(0.0f, 0.0f, 0.0f);
    e->transform.rotation = 0.0f;
    e->transform.scale = InitVector3(1.0f, 1.0f, 1.0f);
    e->velocity = InitVector2(0.0f, 0.0f);
    e->color = InitVector4(0.0f, 0.0f, 0.0f, 0.0f);
    e->data = NULL;
    e->Start = NULL;
    e->Update = NULL;
    e->OnDestroy = NULL;

    return e;
};

Entity* Find(Scene** _scene, const char* _name) {
    if (_scene == NULL || *_scene == NULL || _name == NULL || _name[0] == '\0')
        return NULL;

    int count = vec_count(&(*_scene)->entities);

    for (int i = 0; i < count; i++)
        if ((*_scene)->entities[i].name != NULL && strcmp(_name, (*_scene)->entities[i].name) == 0)
            return &(*_scene)->entities[i];
    
    count = vec_count(&(*_scene)->startEntities);
    
    for (int i = 0; i < count; i++)
        if ((*_scene)->startEntities[i].name != NULL && strcmp(_name, (*_scene)->startEntities[i].name) == 0)
            return &(*_scene)->startEntities[i];
    
    return NULL;
}

Entity* GetEntity(Scene** _scene, int _id) {
    if (_scene == NULL || *_scene == NULL)
        return NULL;

    int count = vec_count(&(*_scene)->entities);
    for (int i = 0; i < count; i++) {
        if ((*_scene)->entities[i].id == _id)
            return &(*_scene)->entities[i];
    }

    count = vec_count(&(*_scene)->startEntities);
    for (int i = 0; i < count; i++) {
        if ((*_scene)->startEntities[i].id == _id)
            return &(*_scene)->startEntities[i];
    }

    return NULL;
}

void Destroy(AppContext* _app, Scene** _scene, int _id) {
    int count = vec_count(&(*_scene)->entities);
    for (int i = 0; i < count; i++) {
        if ((*_scene)->entities[i].id == _id) {
            (*_scene)->entities[i].OnDestroy(_app, &(*_scene)->entities[i]);
            
            if ((*_scene)->entities[i].data != NULL) {
                free((*_scene)->entities[i].data);
                (*_scene)->entities[i].data = NULL;
            }

            vec_remove_at(&(*_scene)->entities, i);
            return;
        }
    }
}