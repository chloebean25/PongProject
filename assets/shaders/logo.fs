#version 330 core
out vec4 FragColor;
in vec3 ourColor;
in vec2 TexCoord;
uniform sampler2D MAIN_TEXTURE;
uniform vec4 COLOR;

void main()
{
   vec4 image = texture(MAIN_TEXTURE, TexCoord);

   if (image.a < 0.1)
      discard;

   FragColor = image * COLOR;
}
