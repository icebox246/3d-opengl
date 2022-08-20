#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 tex;
layout(location = 2) in vec3 norm;

smooth out vec2 UV;
smooth out vec3 normal;
smooth out vec3 vertex_position;
smooth out vec3 position;
smooth out vec3 world_position;
smooth out vec4 sun_position;

uniform mat3 normal_model;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 sun_view;
uniform mat4 sun_projection;

void main() {
    gl_Position = ((projection * view) * model) * vec4(pos, 1.0);
    vertex_position = pos;
    position = gl_Position.xyz;
    world_position = vec3(model * vec4(pos, 1.0));
    sun_position = (sun_projection * sun_view) * vec4(world_position, 1.0);
    UV = tex;
    normal = normal_model * norm;
}
