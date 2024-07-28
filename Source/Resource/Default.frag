#version 400 core

out vec4 FragColor;

in vec3 Position;
in vec2 UV;

uniform sampler2D Diffuse;

void main()
{
    FragColor = vec4(UV.y, UV.x, 0.0f, 1.0f);
    // FragColor = texture(Diffuse, UV);
}
