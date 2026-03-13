#pragma once
#include "math.h"
#include "model.h"
#include "shader.h"

typedef struct {
    Vector3 position;
    float   rotation;
    Vector3 scale;
} Transform;

struct Image;
struct Shader;
typedef struct AppContext AppContext;

typedef struct Entity Entity; 

struct Entity {
    int         id;
    char*       name;
    Transform   transform;
    u32         shaderId;
    Image*      image;
    Model*      model;
    Vector2     velocity;
    Vector4     color;
    
    void*       data;

    void (*Start)(AppContext*, Entity*);
    void (*Update)(AppContext*, Entity*);
    void (*Draw)(AppContext*, Entity*);
    void (*OnDestroy)(AppContext*, Entity*);
};