#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 tex;
layout(location = 2) in vec3 norm;

smooth out vec2 UV;
smooth out vec3 normal;
smooth out vec3 position;

uniform mat3 normal_model;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = (projection * view * model) * vec4(pos, 1.0);
	position = gl_Position.xyz;
    UV = tex;
    normal = normal_model * norm;
}
