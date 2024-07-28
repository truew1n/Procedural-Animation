#version 400 core

layout (location = 0) in vec3 LPosition;
layout (location = 1) in vec2 LUV;

out vec3 Position;
out vec2 UV;

uniform mat4 Projection;
uniform mat4 View;
uniform vec3 WorldPosition;

void main()
{
    Position = LPosition;
    UV = LUV;
    gl_Position = Projection * View * vec4(LPosition + WorldPosition, 1.0f);
}