#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 TRANSFORM;
uniform mat4 PROJECTION;
uniform mat4 VIEW;

void main()
{
    gl_Position = PROJECTION * VIEW * TRANSFORM * vec4(aPos, 1.0f);
    TexCoord = aTexCoord;
}