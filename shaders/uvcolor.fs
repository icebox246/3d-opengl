#version 330 core

smooth in vec2 UV;
smooth in vec3 normal;

out vec4 outColor;

void main() {
    vec3 light_direction = normalize(vec3(-0.5, -1, -1));

    float ambient_strength = 0.1;
    float diffuse = max(dot(-light_direction, normal), 0.0);

    outColor = (ambient_strength + diffuse) * vec4(UV.x, 0.7, UV.y, 1.0);
}
