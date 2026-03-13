#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D MAIN_TEXTURE;
uniform vec4 COLOR;
uniform float TIME;

void main()
{
    // bigger tiles
    vec2 uv = TexCoord * 2.0;

    // scroll up-right
    uv += vec2(-TIME * 0.2, -TIME * 0.2);

    // force repeating
    uv = fract(uv);

    vec4 image = texture(MAIN_TEXTURE, uv);

    // recolor grayscale checker texture
    float brightness = image.r;

    vec3 darkColor = vec3(0.03, 0.01, 0.06);
    vec3 lightColor = vec3(0.75, 0.30, 1.00);

    vec3 finalColor = mix(darkColor, lightColor, brightness);

    FragColor = vec4(finalColor, 1.0);
}