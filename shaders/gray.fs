#version 330 core

smooth in vec2 UV;
smooth in vec3 position;
out vec4 outColor;

void main() {
    float horizon = 60.0;
    float horizon_mult =
        1.0 - pow(1.0 - clamp(length(position) / horizon, 0.0, 1.0), 2.0);
    outColor = vec4(vec3(mix(0.2, 0.7, horizon_mult)), 1);
}
