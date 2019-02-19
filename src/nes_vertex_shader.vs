#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aOffset;
layout (location = 2) in vec3 aColor;

out vec4 vertexColor;

void main() {
    gl_Position = vec4(aPos + aOffset, 0.0, 1.0);
    vertexColor = vec4(aColor, 1.0);
}
