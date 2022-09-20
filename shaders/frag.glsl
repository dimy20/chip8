#version 330 core

uniform sampler2D aTexture;
in vec2 text_coord;

out vec4 FragColor;

void main()
{
    FragColor = texture(aTexture, text_coord);
}
