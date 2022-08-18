#version 330 core

smooth in vec2 UV;
smooth in vec3 normal;

uniform sampler2D tex;

out vec4 outColor;

void main() {
    vec3 light_direction = normalize(vec3(-0.5, -1, -1));

    float ambient_strength = 0.2;
    float diffuse = max(dot(-light_direction, normal), 0.0);

    outColor = (ambient_strength + diffuse) * texture(tex, UV);
}
